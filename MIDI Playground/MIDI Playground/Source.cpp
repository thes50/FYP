#include "MidiFile.h"
#include "Notes.h"
#include "Autosave.h"

#include <boost\filesystem.hpp>
#include <numeric>
#include <cmath>

int notes[12] = { 'Ab', 'A', 'Bb', 'B', 'C', 'Db', 'D', 'Eb', 'E', 'F', 'Gb', 'G' };
int averageVelocity = NULL;
DATA* dataStructs = new DATA();

void parseMidiFiles() 
{
	//For each note in each track (max 16) get command byte.
	//If command byte == 0x90 (note-on) then get value.
	//Run contains function to get bool + octave pair
	Notes note;

	std::array<int,12> countVals = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	for (unsigned a = 0; a < dataStructs->Files->size(); a++)
	{
		MidiFile file(dataStructs->Files->at(a));
		std::vector<std::vector<int>> trackOctavesPerFile, trackVelocitiesPerFile, trackNoteLengthsPerFile;
		for (int i = 0; i < file.getTrackCount(); i++)
		{
			FileHandling::writeToLog(TRACK_FOUND + std::to_string(i));
			std::vector<int> trackVelocities, trackOctaves, trackNoteLengths;
			for (int y = 0; y < file.getEventCount(i); y++)
			{
				if (file[i][y].getCommandNibble() == 0x90)
				{
					for (int z = 0; z < 12; z++)
					{
						std::pair<bool, int> check = note.contains(file[i][y].getP1(), notes[z]);
						if (check.first)
						{
							trackOctaves.push_back(check.second);
							trackVelocities.push_back(file[i][y].getVelocity());
							trackNoteLengths.push_back(file[i][y].getTickDuration());
							countVals[z]++;
							break;
						}
					}
				}
			}
			trackOctavesPerFile.push_back(trackOctaves);
			trackVelocitiesPerFile.push_back(trackVelocities);
			trackNoteLengthsPerFile.push_back(trackNoteLengths);
		}
		dataStructs->OctaveStack->push_back(trackOctavesPerFile);
		dataStructs->Velocities->push_back(trackVelocitiesPerFile);
		dataStructs->NoteLengths->push_back(trackNoteLengthsPerFile);
		dataStructs->CountedValues->push_back(countVals);
	}
}
int main()
{
	/*
		Boost FileSystem used for multiplatform capability. C++ stdlib implements filesystem but not fully supported yet.
		Alternative is using Win32 API, hence limiting portability. Attempts to load config file, if none found then attempt setup.
	*/
	if (!FileHandling::loadCFG())
	{
		FileHandling::writeToLog(FIRST_LOAD);
		FileHandling::createCFG();
		try
		{
			FileHandling::createDirectoryIfNotValid("tracks");
			boost::filesystem::path FILE_PATH("tracks/");
			boost::filesystem::directory_iterator  end;
			for (boost::filesystem::directory_iterator it(FILE_PATH); it != end; ++it)
			{
				if (boost::filesystem::is_regular_file(it->status()))
				{
					boost::filesystem::path currPath = *it;
					dataStructs->Files->push_back((std::string)currPath.string());
					FileHandling::writeToLog(LOADED_FILE + currPath.string());
				}
			}
		}
		catch (const boost::filesystem::filesystem_error& err)
		{
			FileHandling::writeErrorToLog(UNEXPECTED_ERROR, err.what());
		}

		//Create the file, parse it.
		parseMidiFiles();
	}

	try
	{
		FileHandling::writeToLog(LOADING_PREVIOUS_SETTINGS);
		Autosave::dataStruct = dataStructs;
		std::thread thr(Autosave::Auto);
		thr.detach();

		dataStructs = FileHandling::load();
		//Merge threads ending autosave loop
		Autosave::endThread = true;
		//thr.join();
	}
	catch (const std::exception &e)
	{
		FileHandling::writeErrorToLog(ERROR_IN_THREAD, e.what());
	}
	

	FileHandling::save(dataStructs);
	//Get the average velocity. std::round fixes the standard int rounding error
	//averageVelocity = std::round(std::accumulate(Velocities->begin(), Velocities->end(), 0) / Velocities->size());
}