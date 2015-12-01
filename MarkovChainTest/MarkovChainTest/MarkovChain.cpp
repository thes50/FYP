#include "MarkovChain.h"

MarkovChain::MarkovChain()
{
	file = nullptr;
}

MarkovChain::MarkovChain(const std::string filepath)
{
	if (FileHandling::checkDirectoryAndFile(filepath))
	{
		FileHandling::createCopyOfFile(filepath);
	}
	file = new MidiFile(filepath);
}

MarkovChain::MarkovChain(MidiFile & midiFile)
{
	file = &midiFile;
}
#include <math.h>
Note MarkovChain::getNextNote(Note root)
{
	Note newNote;
	if (noteCountTable.find(root.cNote) == noteCountTable.end())
	{
		newNote.cNote = "Err";
		newNote.isValid = false;
		return newNote;
	}
	
	std::map<std::string, int> submap = noteCountTable.at(root.cNote);
	int total = 0;
	for (auto count : submap)
	{
		total += count.second;
	}

	for (auto count : submap)
	{
		double newAmount = (double)count.second / (double)total;
		submap.at(count.first) = std::round(newAmount * 100);
	}

	bool passed = false;
	for (int i = 0; i < 100 && !passed; i++)
	{
		for (auto count : submap)
		{
			//Get %chance of success, if its less than the conditional value in submap then return that as a new note.
			int chance = Random::getRandomNumber(0, 100);
			if (chance < count.second)
			{
				newNote.cNote = count.first;
				newNote.octave = Random::getRandomNumber(5, 5); //Replace with actual markov logic
				newNote.isValid = true;
				newNote.velocity = 60;
				passed = true;
				break;
			}
		}
	}
	return newNote;
}

bool MarkovChain::createFromMidiFile(const std::string filepath)
{
	if (FileHandling::createCopyOfFile(filepath))
	{
		std::string newFilepath = (std::string)boost::filesystem::current_path().string() + FileHandling::trackPath + filepath;
		std::replace(newFilepath.begin(), newFilepath.end(), '\\', '/'); //Potentially uneeded

		file = new MidiFile(newFilepath);
		if (file->status() == 1)
		{
			return true;
		}
	}
	return false;
}

bool MarkovChain::analyseMidiFile()
{
	Notes noteClass;

	file->linkNotePairs();
	file->doTimeAnalysis();

	int highestNoteCount = 0;

	for (int i = 0; i < file->getTrackCount(); i++)
	{
		if (file[0][i].getSize() > highestNoteCount)
		{
			highestNoteCount = file[0][i].getSize();
			mostPopulatedTrack = i;
		}
	}

	//Build map table
	for (int i = 0; i < file[0][mostPopulatedTrack].getSize(); i++)
	{
		if (file[0][mostPopulatedTrack][i].isLinked())
		{
			//Add to new Map for later usage. Potentially implement a Positive/Negative system rather than a map of values.
			//Other idea is to take an average of all of the durations and then use this rounded to nearest note.
			int durationInTicks = file[0][mostPopulatedTrack][i].getTickDuration();
			double durationInSeconds = file[0][mostPopulatedTrack][i].getDurationInSeconds();
		}

		Note note1 = noteClass.getNoteType(file[0][mostPopulatedTrack][i]);
		if (noteCountTable.find(note1.cNote) == noteCountTable.end() && note1.isValid)
		{
			//No key value found for note, insert new key.
			noteCountTable.insert(std::make_pair(note1.cNote, std::map<std::string, int>()));
		}
		//Check up to thirty events ahead for the next valid note
		for (int a = 1; a <= 30; a++)
		{
			if ((i + a) < file[0][mostPopulatedTrack].size() && note1.isValid)
			{
				Note nextNote = noteClass.getNoteType(file[0][mostPopulatedTrack][i + a]);
				if (!nextNote.isValid)
				{
					continue;
				}

				if (noteCountTable.at(note1.cNote).find(nextNote.cNote) == noteCountTable.at(note1.cNote).end())
				{
					//Key not found, insert new
					noteCountTable.at(note1.cNote).insert(std::pair<std::string, int>(nextNote.cNote, 1));
				}
				else
				{
					//Key found, increment note occurrene count
					noteCountTable.at(note1.cNote).at(nextNote.cNote)++;
				}
				break;
			}
		}
	}
	return true;
}

MidiFile& MarkovChain::generateNewMidiFile()
{
	Notes noteClass;
	MidiFile& newFile = MidiFile("AYYLMAO.mid");
	newFile.addTrack();
	newFile.setTicksPerQuarterNote(512);

	int trackLength = 0;
	for (int i = 0; i < file[0][mostPopulatedTrack].getSize(); i++)
	{
		if (file[0][mostPopulatedTrack][i].isNoteOn())
		{
			trackLength++;
		}
	}

	int trackLengthShort = std::round(trackLength / 10);
	int trackLengthMin = 0;
	int trackLengtMax = trackLength + trackLengthShort;

	if ((trackLength - trackLengthShort) < 120)
	{
		trackLengthMin = 120;
	}
	else
	{
		trackLengthMin = trackLength - trackLengthShort;
	}

	trackLength = (Random::getRandomNumber(trackLengthMin, trackLengtMax) / 2);
	int currTick = 0;
	Note root("C", 5, 64, 0.0, true);

	for (int i = 0; i < trackLength; i++)
	{
		uchar note = (uchar)noteClass.getNoteMidiValue(root = getNextNote(root));
		//Write the start note.
		newFile.addEvent(0, currTick, std::vector<uchar>
		({
			NOTE_ON, 
			note,
			64 
		}));

		//Increment tick value.
		currTick += newFile.getTicksPerQuarterNote();

		//Write the ending note
		newFile.addEvent(0, currTick, std::vector<uchar>
		({
			NOTE_OFF,
			note,
			0
		}));
	}
	newFile.write(newFile.getFilename());
	return newFile;
}
