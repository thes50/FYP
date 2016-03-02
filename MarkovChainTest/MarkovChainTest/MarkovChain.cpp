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
	std::map<intPair, int> durSubmap;
	std::map<std::string, int> submap = noteCountTable.at(key);

	int velA = Maths::round(rootA.velocity, 5);
	int velB = Maths::round(rootB.velocity, 5);
	if (velocityCountTable.find(intPair(velA, velB)) == velocityCountTable.end())
		return NotePair(Note(), Note());
	std::map<intPair, int> velSubmap = velocityCountTable.at(intPair(velA, velB));

	if(octaveCountTable.find(intPair(rootA.octave,rootB.octave)) == octaveCountTable.end())
		return NotePair(Note(), Note());
	std::map<intPair, int> octSubmap = octaveCountTable.at(intPair(rootA.octave, rootB.octave));

	intPair durKey((int)Maths::round(rootA.duration, (file->getTicksPerQuarterNote() / 4)), (int)Maths::round(rootB.duration, (file->getTicksPerQuarterNote() / 4)));
	if (durationCountTable.find(intPair()) == durationCountTable.end())
	{
		durSubmap = durationCountTable.begin()->second;
	}
	else
	{
		durSubmap = durationCountTable.at(durKey);
	}

	int total = 0;
	submap = countInSubmap<std::map<std::string, int>, std::string, int> (submap);
	velSubmap = countInSubmap<std::map<intPair, int>, intPair, int>(velSubmap);
	octSubmap = countInSubmap<std::map<intPair, int>, intPair, int>(octSubmap);
	durSubmap = countInSubmap<std::map<intPair, int>, intPair, int>(durSubmap);

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
				returnB.cNote = returnBVal;
				passedNote = true;
				break;
			}
		}
		for (auto count : velSubmap)
		{
			int chance = Random::getRandomNumber(0, 100);
			if (!passedVel && chance < count.second)
			{
				returnA.velocity = count.first.first;
				returnB.velocity = count.first.second;
				passedVel = true;
			}
		}
		for (auto count : octSubmap)
		{
			int chance = Random::getRandomNumber(0, 100);
			if (!passedOct && chance < count.second)
			{
				returnA.octave = count.first.first;
				returnB.octave = count.first.second;
				passedOct = true;
			}
		}
		for (auto count : durSubmap)
		{
			int chance = Random::getRandomNumber(0, 100);
			if (!passedDur && chance < count.second)
			{
				returnA.duration = count.first.first;
				returnB.duration = count.first.second;
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

bool MarkovChain::isWithinSuitableRange(long value)
{
	return (value > (file->getTicksPerQuarterNote() / 8) && value < (file->getTicksPerQuarterNote() * 2));
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
	std::vector<int> noteList;
	long previousNoteEnd = 0;
	for (int i = 0; i < file[0][mostPopulatedTrack].getSize(); i++)
	{
		if (noteClass.getNoteType(&file[0][mostPopulatedTrack][i]).isValid)
		{
			noteList.push_back(i);
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
	for (int i = 2; i < noteList.size(); i++)
	{
		auto& evt = file[0][mostPopulatedTrack][noteList.at(i)];
		auto& prevEvt = file[0][mostPopulatedTrack][noteList.at(i - 1)];
		auto& twoPrevEvt = file[0][mostPopulatedTrack][noteList.at(i - 2)];
		//Round to length of 1/8th note.
		long duration = Maths::round(evt.getTickDuration(), (file->getTicksPerQuarterNote() / 2));
		long prevDuration = Maths::round(prevEvt.getTickDuration(), file->getTicksPerQuarterNote() / 2);
		long twoPrevDuration = Maths::round(twoPrevEvt.getTickDuration(), file->getTicksPerQuarterNote() / 2);
		Note grandChildNote = noteClass.getNoteType(&twoPrevEvt);
		Note childNote = noteClass.getNoteType(&prevEvt);
		Note thisNote = noteClass.getNoteType(&evt);
		std::string grandChildCombined = grandChildNote.cNote + " " + childNote.cNote;
		std::string childThisCombined = childNote.cNote + " " + thisNote.cNote;
		intPair durPair(prevDuration, duration);
		intPair durOldPair(twoPrevDuration, prevDuration);
		intPair velPair(Maths::round(childNote.velocity, 5), Maths::round(thisNote.velocity, 5));
		intPair velOldPair(Maths::round(grandChildNote.velocity, 5), Maths::round(childNote.velocity, 5));
		intPair octPair(noteClass.getOctave(childNote, noteClass.getNoteMidiValue(childNote)),
			noteClass.getOctave(thisNote, noteClass.getNoteMidiValue(thisNote)));
		intPair octOldPair(noteClass.getOctave(grandChildNote, noteClass.getNoteMidiValue(grandChildNote)),
			noteClass.getOctave(childNote, noteClass.getNoteMidiValue(childNote)));

		if (isWithinSuitableRange(duration) && isWithinSuitableRange(prevDuration) && isWithinSuitableRange(twoPrevDuration))
		{
			if (durationCountTable.find(durPair) == durationCountTable.end())
			{
				durationCountTable.insert(std::make_pair(durPair, std::map<intPair, int>()));
			}
			if (durationCountTable.at(durPair).find(durOldPair) == durationCountTable.at(durPair).end())
			{
				durationCountTable.at(durPair).insert(std::pair<intPair, int>(durOldPair, 1));
			}
			else
			{
				durationCountTable.at(durPair).find(durOldPair)++;
			}
		}

		//Inserts current Note + Child pair if not found.
		if (noteCountTable.find(childThisCombined) == noteCountTable.end())
		{
			noteCountTable.insert(std::make_pair(childThisCombined, std::map<std::string, int>()));
		}
		if (noteCountTable.at(childThisCombined).find(grandChildCombined) == noteCountTable.at(childThisCombined).end())
		{
			noteCountTable.at(childThisCombined).insert(std::pair<std::string, int>(grandChildCombined, 1));
		}
		else
		{
			//Key found, increment note occurrence count
			noteCountTable.at(childThisCombined).at(grandChildCombined)++;
		}

		if (velocityCountTable.find(velPair) == velocityCountTable.end())
		{
			velocityCountTable.insert(std::make_pair(velPair, std::map<intPair, int>()));
		}
		if (velocityCountTable.at(velPair).find(velOldPair) == velocityCountTable.at(velPair).end())
		{
			velocityCountTable.at(velPair).insert(std::pair<intPair, int>(velOldPair, 1));
		}
		else
		{
			velocityCountTable.at(velPair).at(velOldPair)++;
		}

		if (octaveCountTable.find(octPair) == octaveCountTable.end())
		{
			octaveCountTable.insert(std::make_pair(octPair, std::map<intPair, int>()));
		}

		if (octaveCountTable.at(octPair).find(octOldPair) == octaveCountTable.at(octPair).end())
		{
			octaveCountTable.at(octPair).insert(std::pair<intPair, int>(octOldPair, 1));
		}
		else
		{
			octaveCountTable.at(octPair).at(octOldPair)++;
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