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

bool MarkovChain::isNoteFinishedConstruction(bool noteNamed, bool velocity, bool octave, bool duration)
{
	return (noteNamed && velocity && octave && duration);
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
	std::map<int, int> durSubmap;
	std::map<std::string, int> submap = noteCountTable.at(key);
	int vel = Maths::round(rootA.velocity, 5);
	if (velocityCountTable.find(vel) == velocityCountTable.end())
		return NotePair(Note(), Note());
	std::map<int, int> velSubmap = velocityCountTable.at(vel);
	if(octaveCountTable.find(rootB.octave) == octaveCountTable.end())
		return NotePair(Note(), Note());
	std::map<int, int> octSubmap = octaveCountTable.at(rootB.octave);
	if (durationCountTable.find(Maths::round(rootA.duration, (file->getTicksPerQuarterNote() / 4))) == durationCountTable.end())
	{
		durSubmap = durationCountTable.begin()->second;
	}
	else
	{
		durSubmap = durationCountTable.at(Maths::round(rootA.duration, (file->getTicksPerQuarterNote() / 4)));
	}

	int total = 0;
	submap = countInSubmap<std::map<std::string, int>, std::string, int> (submap);
	velSubmap = countInSubmap<std::map<int, int>, int,int>(velSubmap);
	octSubmap = countInSubmap<std::map<int, int>, int, int>(octSubmap);
	durSubmap = countInSubmap<std::map<int, int>, int, int>(durSubmap);

	//Has 100 attempts to generate a valid next node
	bool passedNote = false;
	bool passedVel = false;
	bool passedOct = false;
	bool passedDur = false;
	for (int i = 0; i < 100; i++)
	{
		for (auto count : submap)
		{
			//Get %chance of success, if its less than the conditional value in submap then return that as a new note.
			int chance = Random::getRandomNumber(0, 100);
			if (!passedNote && chance < count.second)
			{
				//Split into array
				std::string returnAVal = count.first.substr(0, count.first.find(' '));
				std::string returnBVal = count.first.substr(count.first.find(' ') + 1, count.first.size());

				returnA.cNote = returnAVal;
				returnA.duration = file->getTicksPerQuarterNote();

				returnB.cNote = returnBVal;
				returnB.duration = file->getTicksPerQuarterNote();
				passedNote = true;
				break;
			}
		}
		for (auto count : velSubmap)
		{
			int chance = Random::getRandomNumber(0, 100);
			if (!passedVel && chance < count.second)
			{
				returnA.velocity = count.first;
				returnB.velocity = count.first;
				passedVel = true;
			}
		}
		for (auto count : octSubmap)
		{
			int chance = Random::getRandomNumber(0, 100);
			if (!passedOct && chance < count.second)
			{
				returnA.octave = count.first;
				returnB.octave = count.first;
				passedOct = true;
			}
		}

		for (auto count : durSubmap)
		{
			int chance = Random::getRandomNumber(0, 100);
			if (!passedDur && chance < count.second)
			{
				returnA.duration = count.first;
				returnB.duration = count.first;
				passedDur = true;
			}
		}

		if (isNoteFinishedConstruction(passedNote, passedVel, passedOct, passedDur))
		{
			returnA.isValid = true;
			returnB.isValid = true;
			break;
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
	file->absoluteTicks();

	int highestNoteCount = 0;

	for (int i = 0; i < file->getTrackCount(); i++)
	{
		if (file[0][i].getSize() > highestNoteCount)
		{
			highestNoteCount = file[0][i].getSize();
			mostPopulatedTrack = i;
		}
	}

	//Initial scan notes into single structure. Sort timings for convenience
	std::vector<MidiEvent> noteList;
	long previousNoteEnd = 0;
	for (int i = 0; i < file[0][mostPopulatedTrack].getSize(); i++)
	{
		if (noteClass.getNoteType(&file[0][mostPopulatedTrack][i]).isValid)
		{
			noteList.push_back(file[0][mostPopulatedTrack][i]);
			if (previousNoteEnd != file[0][mostPopulatedTrack][i].tick)
			{
				int difference = Maths::round((previousNoteEnd - file[0][mostPopulatedTrack][i].tick), (file->getTicksPerQuarterNote() / 4));
				if (pauseCounter.find(difference) == pauseCounter.end())
				{
					pauseCounter.insert(std::pair<int, int>(difference, 1));
				}
				else
				{
					pauseCounter.at(difference)++;
				}
			}
			previousNoteEnd = file[0][mostPopulatedTrack][i].tick + file[0][mostPopulatedTrack][i].getTickDuration();

			//Prevents out of bounds exception
			if (i > 1)
			{
				//Round to length of 1/32nd note.
				long duration = Maths::round(file[0][mostPopulatedTrack][i].getTickDuration(), (file->getTicksPerQuarterNote() / 4));
				if (duration > 0 && duration < (file->getTicksPerQuarterNote() * 4))
				{
					//Added for convenience when using tick based functions
					if (durationCountTable.find(duration) == durationCountTable.end())
					{
						durationCountTable.insert(std::make_pair(duration, std::map<int, int>()));
					}
					if (durationCountTable.at(duration).find(file[0][mostPopulatedTrack][i - 1].getTickDuration()) == durationCountTable.at(duration).end())
					{
						durationCountTable.at(duration).insert(std::make_pair<int, int>(Maths::round(file[0][mostPopulatedTrack][i - 1].getTickDuration(), (file->getTicksPerQuarterNote() / 4)), 1));
					}
					else
					{
						durationCountTable.at(duration).find(Maths::round(file[0][mostPopulatedTrack][i - 1].getTickDuration(), (file->getTicksPerQuarterNote() / 4)))++;
					}
				}
			}
		}
	}

	std::map<int, int> localPauseCounter;
	for (auto val : pauseCounter)
	{
		//Check is valid note duration, quantise as necessary. Check within acceptable range (qrt length/4...4*qrt length)
		if (val.first % (file->getTicksPerQuarterNote() / 4) == 0 && val.first > 0 && val.first < file->getTicksPerQuarterNote() * 4)
		{
			localPauseCounter.insert(std::pair<int,int>(val.first, val.second));
		}
	}
	pauseCounter = localPauseCounter;

	//Build map table
	for (int i = 0; i < noteList.size(); i++)
	{
		//Check it isn't the end of the list.
		if (i + 2 >= noteList.size())
		{
			break;
		}

		Note grandChildNote = noteClass.getNoteType(&noteList.at(i + 2));
		Note childNote = noteClass.getNoteType(&noteList.at(i + 1));
		Note thisNote = noteClass.getNoteType(&noteList.at(i));
		std::string grandChildCombined = grandChildNote.cNote + " " + childNote.cNote;
		std::string childThisCombined = childNote.cNote + " " + thisNote.cNote;

		//Inserts current Note + Child pair if not found.
		if (noteCountTable.find(childThisCombined) == noteCountTable.end())
		{
			noteCountTable.insert(std::make_pair(childThisCombined, std::map<std::string, int>()));
		}
		int vel = 0;
		if (velocityCountTable.find(vel = Maths::round(Maths::average(2, childNote.velocity, thisNote.velocity), 5)) == velocityCountTable.end())
		{
			velocityCountTable.insert(std::make_pair(vel, std::map<int, int>()));
		}
		int octave = noteClass.getOctave(thisNote, noteClass.getNoteMidiValue(thisNote));
		if (octaveCountTable.find(octave) == octaveCountTable.end())
		{
			octaveCountTable.insert(std::make_pair(octave, std::map<int, int>()));
		}

		//Inserts Next Note + Node After
		if (noteCountTable.at(childThisCombined).find(grandChildCombined) == noteCountTable.at(childThisCombined).end())
		{
			noteCountTable.at(childThisCombined).insert(std::pair<std::string, int>(grandChildCombined, 1));
		}
		else
		{
			//Key found, increment note occurrence count
			noteCountTable.at(childThisCombined).at(grandChildCombined)++;
		}
		int velGrand = 0;
		if (velocityCountTable.at(vel).find(velGrand = Maths::round(Maths::average(2, grandChildNote.velocity, childNote.velocity), 5)) == velocityCountTable.at(vel).end())
		{
			velocityCountTable.at(vel).insert(std::pair<int, int>(velGrand, 1));
		}
		else
		{
			velocityCountTable.at(vel).at(velGrand)++;
		}
		int octParent = noteClass.getOctave(childNote, noteClass.getNoteMidiValue(childNote));
		if (octaveCountTable.at(octave).find(octParent) == octaveCountTable.at(octave).end())
		{
			octaveCountTable.at(octave).insert(std::pair<int, int>(octParent, 1));
		}
		else
		{
			octaveCountTable.at(octave).at(octParent)++;
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
	newFile.setTicksPerQuarterNote(file->getTicksPerQuarterNote());

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
		if (rootPair.noteA.cNote == "Err" || rootPair.noteB.cNote == "Err")
		{
			rootPair = startingPair;
		}
		writeNote(newFile, rootPair.noteA, currTick, noteClass);
	}
	const std::string newfilename = "fileout.mid";
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
		(uchar)newNote.velocity
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