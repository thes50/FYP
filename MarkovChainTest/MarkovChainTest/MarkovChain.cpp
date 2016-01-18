#include "MarkovChain.h"

#define DEBUG
#ifdef DEBUG
#include <iostream>
#endif


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

NotePair MarkovChain::getNextNote(Note rootA, Note rootB)
{
	Note returnA;
	Note returnB;
	std::string key = rootA.cNote + " " + rootB.cNote;

	//Attempt to find next note.
	if (noteCountTable.find(key) == noteCountTable.end())
	{
		returnA.cNote = "Err";
		returnA.isValid = false;
		return NotePair(returnA, Note());
	}
	
	//Create local copy to update
	std::map<std::string, int> submap = noteCountTable.at(key);
	int total = 0;
	for (auto count : submap)
	{
		total += count.second;
	}

	for (auto count : submap)
	{
		//Force floating point division logic to avoid truncation.
		//Convert to percentage
		double newAmount = (double)count.second / (double)total;
		submap.at(count.first) = std::round(newAmount * 100);
	}

	//Has 100 attempts to generate a valid next node
	bool passed = false;
	for (int i = 0; i < 100 && !passed; i++)
	{
		for (auto count : submap)
		{
			//Get %chance of success, if its less than the conditional value in submap then return that as a new note.
			int chance = Random::getRandomNumber(0, 100);
			if (chance < count.second)
			{
				//Split into array
				std::string returnAVal = count.first.substr(0, count.first.find(' '));
				std::string returnBVal = count.first.substr(count.first.find(' ') + 1, count.first.size());

				returnA.cNote = returnAVal;
				returnA.octave = Random::getRandomNumber(5, 5); //Replace with actual markov logic
				returnA.isValid = true;
				returnA.velocity = 60;
				returnA.duration = file->getTicksPerQuarterNote();

				returnB.cNote = returnBVal;
				returnB.octave = Random::getRandomNumber(5, 5); //Replace with actual markov logic
				returnB.isValid = true;
				returnB.velocity = 60;
				returnB.duration = file->getTicksPerQuarterNote();
				passed = true;
				break;
			}
		}
	}
	return NotePair(returnA, returnB);
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

	//Initial scan notes into single structure.
	std::vector<MidiEvent> noteList;
	for (int i = 0; i < file[0][mostPopulatedTrack].getSize(); i++)
	{
		if (noteClass.getNoteType(file[0][mostPopulatedTrack][i]).isValid)
		{
			noteList.push_back(file[0][mostPopulatedTrack][i]);
		}
	}

	//Build map table
	for (int i = 0; i < noteList.size(); i++)
	{
		//Check it isn't the end of the list.
		if (i + 2 >= noteList.size())
		{
			break;
		}

		if (noteList.at(i).isLinked())
		{
			//Add to new Map for later usage. Potentially implement a Positive/Negative system rather than a map of values.
			//Other idea is to take an average of all of the durations and then use this rounded to nearest note.
			int durationInTicks = noteList.at(i).getTickDuration();

			#ifdef DEBUG
			std::cout << durationInTicks << std::endl;
			#endif
		}

		Note grandChildNote = noteClass.getNoteType(noteList.at(i + 2));
		Note childNote = noteClass.getNoteType(noteList.at(i + 1));
		Note thisNote = noteClass.getNoteType(noteList.at(i));
		std::string grandChildCombined = grandChildNote.cNote + " " + childNote.cNote;
		std::string childThisCombined = childNote.cNote + " " + thisNote.cNote;

		//Inserts current Note + Child pair if not found.
		if (noteCountTable.find(childThisCombined) == noteCountTable.end())
		{
			noteCountTable.insert(std::make_pair(childThisCombined, std::map<std::string, int>()));
		}

		//Inserts Next Note + Node After
		if (noteCountTable.at(childThisCombined).find(grandChildCombined) == noteCountTable.at(childThisCombined).end())
		{
			noteCountTable.at(childThisCombined).insert(std::pair<std::string, int>(grandChildCombined, 1));
		}
		else
		{
			//Key found, increment note occurrene count
			noteCountTable.at(childThisCombined).at(grandChildCombined)++;
		}
		startingPair = NotePair(thisNote, childNote);
	}
	return true;
}

MidiFile& MarkovChain::generateNewMidiFile()
{
	Notes noteClass;
	MidiFile newFile;
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

	//Build new track.
	int currTick = 0;
	NotePair rootPair(startingPair.noteA, startingPair.noteB);
	for (int i = 0; i < trackLength; i++)
	{
		rootPair = getNextNote(rootPair.noteA, rootPair.noteB);
		writeNote(newFile, rootPair.noteA, currTick, noteClass);
		writeNote(newFile, rootPair.noteB, currTick, noteClass);
	}
	const std::string newfilename = "AYYLMAO.mid";
	newFile.write(newfilename);
	return newFile;
}

void MarkovChain::writeNote(MidiFile & newFile, Note newNote, int& currTick, Notes noteClass)
{
	uchar note = (uchar)noteClass.getNoteMidiValue(newNote);

	//Write the start note.
	newFile.addEvent(0, currTick, std::vector<uchar>
	({
		NOTE_ON,
		note,
		64
	}));

	//Increment tick value.
	currTick += newNote.duration;

	//Write the ending note
	newFile.addEvent(0, currTick, std::vector<uchar>
	({
		NOTE_OFF,
		note,
			0
	}));
}