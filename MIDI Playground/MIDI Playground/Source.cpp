#include "MidiFile.h"
#include "Notes.h"

#include <boost\filesystem.hpp>
#include <iostream>
#include <memory> //Upgrade to shared pointers
#include <string>
#include <vector>
#include <numeric>
#include <cmath>
#include <array>

char notes[12] = { 'Ab', 'A', 'Bb', 'B', 'C', 'Db', 'D', 'Eb', 'E', 'F', 'Gb', 'G' };
std::vector<std::array<int, 12>>*  CountedValues = new std::vector<std::array<int, 12>>();
std::vector<boost::filesystem::path>* Files = new std::vector<boost::filesystem::path>();
std::vector<std::vector<std::vector<int>>>* OctaveStack = new std::vector<std::vector<std::vector<int>>>();
std::vector<std::vector<std::vector<int>>>* Velocities  = new std::vector<std::vector<std::vector<int>>>();
std::vector<std::vector<std::vector<int>>>* NoteLengths = new std::vector<std::vector<std::vector<int>>>();
std::vector<std::vector<std::vector<int>>>* NoteContextFour = new std::vector<std::vector<std::vector<int>>>();
std::vector<std::vector<std::vector<int>>>* NoteContextEight = new std::vector<std::vector<std::vector<int>>>();
std::vector<std::vector<std::vector<int>>>* NoteContextSixteen = new std::vector<std::vector<std::vector<int>>>();
int averageVelocity = NULL;

void parseMidiFiles() 
{
	//For each note in each track (max 16) get command byte.
	//If command byte == 0x90 (note-on) then get value.
	//Run contains function to get bool + octave pair
	Notes note;
	std::array<int,12> countVals = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	for (unsigned a = 0; a < Files->size(); a++)
	{
		MidiFile file(Files->at(a).string());
		std::vector<std::vector<int>> trackOctavesPerFile, trackVelocitiesPerFile, trackNoteLengthsPerFile;
		for (int i = 0; i < file.getTrackCount(); i++)
		{
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
		OctaveStack->push_back(trackOctavesPerFile);
		Velocities->push_back(trackVelocitiesPerFile);
		NoteLengths->push_back(trackNoteLengthsPerFile);
		CountedValues->push_back(countVals);
	}
}

int main()
{
	/*
		Boost FileSystem used for multiplatform capability. C++ stdlib implements filesystem but not fully supported yet.
		Alternative is using Win32 API, hence limiting portability.
	*/
	try
	{
		boost::filesystem::path FILE_PATH("tracks/");
		boost::filesystem::directory_iterator end;
		for (boost::filesystem::directory_iterator it(FILE_PATH); it != end; ++it)
		{
			if (boost::filesystem::is_regular_file(it->status()))
			{
				Files->push_back(*it);
			}
		}
	}
	catch (const boost::filesystem::filesystem_error& err)
	{
		std::cout << "An unexpected error occurred: " << err.what() << std::endl;
	} 

	//Create the file, parse it.
	parseMidiFiles();
	for (unsigned i = 0; i < CountedValues->size(); ++i)
	{
		for (int note : CountedValues->at(i))
		{
			std::cout << note << std::endl;
		}
	}
	//Get the average velocity. std::round fixes the standard int rounding error
	//averageVelocity = std::round(std::accumulate(Velocities->begin(), Velocities->end(), 0) / Velocities->size());
}