#include "MarkovChain.h"

MarkovChain::MarkovChain()x
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

void MarkovChain::cleanTables()
{
	cleanTable<std::vector<noteTable>>(noteCountTable, "notes");
	cleanTable<std::vector<intTable>>(velocityCountTable, "velocity");
	cleanTable<std::vector<intTable>>(durationCountTable, "duration");
	cleanTable<std::vector<intTable>>(octaveCountTable, "octave");
}

void MarkovChain::setBreakdownResolution(int resolution)
{
	parsingNumber = resolution;
}

void MarkovChain::setForceContinueFlag(bool state)
{
	forceContinueThroughError = state;
}

void MarkovChain::setOutputFilename(const std::string filename)
{
	outputFilename = filename;
	cleanOutputFilename();
}

int MarkovChain::getBreakdownResolution()
{
	return parsingNumber;
}

bool MarkovChain::isNoteFinishedConstruction(bool noteNamed, bool velocity, bool octave, bool duration)
{
	return (noteNamed && velocity && octave && duration);
}

bool MarkovChain::getForceContinueFlag()
{
	return forceContinueThroughError;
}

NotePair MarkovChain::getNextNote(Note rootA, Note rootB, int index)
{
	Note returnA;
	Note returnB;
	std::string key = rootA.cNote + " " + rootB.cNote;
	
	auto notTable = noteCountTable.at(index);
	auto veloTable = velocityCountTable.at(index);
	auto octoTable = octaveCountTable.at(index);
	auto duraTable = durationCountTable.at(index);

	//Attempt to find next note.
	if (notTable.find(key) == notTable.end())
	{
		noteTable nextBestTable;
		if (notTable.size() == 0)
		{
			nextBestTable = findNextGoodMap<std::vector<noteTable>, noteTable>(noteCountTable, index);

			if (nextBestTable.size() == 0)
			{
				if (!forceContinueThroughError)
				{
					exit(1);
				}
				//No good table found, create standard pair?
				std::map<std::string, int> newMap;
				newMap.insert(std::pair<std::string, int>("A B", 1));
				newMap.insert(std::pair<std::string, int>("B B", 1));
				std::map<std::string, int> newMapTwo;
				newMapTwo.insert(std::pair<std::string, int>("B B", 1));
				newMapTwo.insert(std::pair<std::string, int>("AB", 1));
				nextBestTable.insert(std::pair<std::string, std::map<std::string, int>>("A B", newMap));
				nextBestTable.insert(std::pair<std::string, std::map<std::string, int>>("B B", newMapTwo));
			}
			else
			{
#if _DEBUG 
				std::cout << "Suitable near map found." << std::endl;
#endif
				//Assign new value.
				std::map<std::string, int> newMap;
				newMap.insert(std::pair<std::string, int>(nextBestTable.begin()->first, 1));
				nextBestTable.insert(std::pair<std::string, std::map<std::string, int>>(key, newMap));
				notTable = nextBestTable;
				noteCountTable.at(index) = nextBestTable;
			}
		}
		else
		{
			nextBestTable = notTable;
		}
		
		returnA.cNote = nextBestTable.begin()->first.substr(0, nextBestTable.begin()->first.find(" "));
		key = nextBestTable.begin()->first;
		notTable = nextBestTable;
	}
	
	//Create local copy to update
	std::map<intPair, int> durSubmap;
	std::map<std::string, int> submap = notTable.at(key);

	int velA = Maths::round(rootA.velocity, 5);
	int velB = Maths::round(rootB.velocity, 5);
	if (veloTable.find(intPair(velA, velB)) == veloTable.end())
	{
		//Can't rely on velocity tables existing if they aren't in source MIDI
		if (veloTable.size() == 0)
		{
			std::map<intPair, int> newMap;
			newMap.insert(std::pair<intPair, int>(intPair(50, 50), 1));
			veloTable.insert(std::pair<intPair, std::map<intPair, int>>(intPair(50, 50), newMap));
		}
		velA = veloTable.begin()->first.first;
		velB = veloTable.begin()->first.second;
	}
	std::map<intPair, int> velSubmap = veloTable.at(intPair(velA, velB));

	int octA = rootA.octave;
	int octB = rootB.octave;
	if (octoTable.find(intPair(octA, octB)) == octoTable.end())
	{
		if (octoTable.size() == 0)
		{
			std::map<intPair, int> newMap;
			newMap.insert(std::pair<intPair, int>(intPair(3, 4), 1));
			octoTable.insert(std::pair<intPair, std::map<intPair, int>>(intPair(3, 3), newMap));
		}
		octA = octoTable.begin()->first.first;
		octB = octoTable.begin()->first.second;
	}
	std::map<intPair, int> octSubmap = octoTable.at(intPair(octA, octB));

	intPair durKey((int)Maths::round(rootA.duration, (file->getTicksPerQuarterNote() / 4)), (int)Maths::round(rootB.duration, (file->getTicksPerQuarterNote() / 4)));
	if (duraTable.find(intPair()) == duraTable.end())
	{
		if (duraTable.size() <= 0)
		{
			durSubmap.insert(std::pair<intPair,int>(intPair(file->getTicksPerQuarterNote(), file->getTicksPerQuarterNote()),1));
		}
		else
		{
			durSubmap = duraTable.begin()->second;
		}
	}
	else
	{
		durSubmap = duraTable.at(durKey);
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

bool MarkovChain::doesGranularityNeedSequenceChecking()
{
	/* TODO: Implement (number / note total) check instead */
	//If greater than five return true, meaning the tables should be scanned for potential looping sequences
	return (parsingNumber > 5);
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
			long currTick = file[0][mostPopulatedTrack][i].tick;
			if (previousNoteEnd != currTick)
			{
				int difference = Maths::round((currTick - previousNoteEnd), (file->getTicksPerQuarterNote() / 4));
				if (isWithinSuitableRange(difference))
				{
					pauseCounter.addPause(difference);
				}
			}
			previousNoteEnd = currTick;
		}
		else if (file[0][mostPopulatedTrack][i].isTempo())
		{
			//Add support for shifts in tempo
			tempoQuartPerMin = file[0][mostPopulatedTrack][i].getTempoBPM();
		}
	}
	
	int size = noteList.size() / parsingNumber;
	std::vector<std::vector<int>> wrappingIndexContainer;

	for (int i = 1; i < (parsingNumber+1); i++)
	{
		auto it1 = std::next(noteList.begin(), (i - 1) * size);
		auto it2 = std::next(noteList.begin(), i * size);
		wrappingIndexContainer.push_back(std::vector<int>(it1,it2));
	}

	for (int ind = 0; ind < parsingNumber; ind++)
	{
		auto container = wrappingIndexContainer.at(ind);

		for (int i = 2; i < container.size(); i++)
		{
			auto& evt = file[0][mostPopulatedTrack][container.at(i)];
			auto& prevEvt = file[0][mostPopulatedTrack][container.at(i - 1)];
			auto& twoPrevEvt = file[0][mostPopulatedTrack][container.at(i - 2)];
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

			if (noteCountTable.size() <= ind)
			{
				noteCountTable.push_back(noteTable());
				durationCountTable.push_back(intTable());
				octaveCountTable.push_back(intTable());
				velocityCountTable.push_back(intTable());
			}

			if (isWithinSuitableRange(duration) && isWithinSuitableRange(prevDuration) && isWithinSuitableRange(twoPrevDuration))
			{
				if (durationCountTable.at(ind).find(durPair) == durationCountTable.at(ind).end())
				{
					durationCountTable.at(ind).insert(std::make_pair(durPair, std::map<intPair, int>()));
				}
				if (durationCountTable.at(ind).at(durPair).find(durOldPair) == durationCountTable.at(ind).at(durPair).end())
				{
					durationCountTable.at(ind).at(durPair).insert(std::pair<intPair, int>(durOldPair, 1));
				}
				else
				{
					durationCountTable.at(ind).at(durPair).find(durOldPair)++;
				}
			}

			//Inserts current Note + Child pair if not found.
 			if (noteCountTable.at(ind).find(childThisCombined) == noteCountTable.at(ind).end())
			{
				noteCountTable.at(ind).insert(std::make_pair(childThisCombined, std::map<std::string, int>()));
			}
			if (noteCountTable.at(ind).at(childThisCombined).find(grandChildCombined) == noteCountTable.at(ind).at(childThisCombined).end())
			{
				noteCountTable.at(ind).at(childThisCombined).insert(std::pair<std::string, int>(grandChildCombined, 1));
			}
			else
			{
				//Key found, increment note occurrence count
				noteCountTable.at(ind).at(childThisCombined).at(grandChildCombined)++;
			}

			if (velocityCountTable.at(ind).find(velPair) == velocityCountTable.at(ind).end())
			{
				velocityCountTable.at(ind).insert(std::make_pair(velPair, std::map<intPair, int>()));
			}
			if (velocityCountTable.at(ind).at(velPair).find(velOldPair) == velocityCountTable.at(ind).at(velPair).end())
			{
				velocityCountTable.at(ind).at(velPair).insert(std::pair<intPair, int>(velOldPair, 1));
			}
			else
			{
				velocityCountTable.at(ind).at(velPair).at(velOldPair)++;
			}

			if (octaveCountTable.at(ind).find(octPair) == octaveCountTable.at(ind).end())
			{
				octaveCountTable.at(ind).insert(std::make_pair(octPair, std::map<intPair, int>()));
			}

			if (octaveCountTable.at(ind).at(octPair).find(octOldPair) == octaveCountTable.at(ind).at(octPair).end())
			{
				octaveCountTable.at(ind).at(octPair).insert(std::pair<intPair, int>(octOldPair, 1));
			}
			else
			{
				octaveCountTable.at(ind).at(octPair).at(octOldPair)++;
			}

			startingPair = NotePair(thisNote, childNote);
		}
	}
	return true;
}

void MarkovChain::cleanRootPair(NotePair& pair)
{
	pair.noteA.velocity = Maths::round(pair.noteA.velocity, 5);
	pair.noteB.velocity = Maths::round(pair.noteB.velocity, 5);

	//tpq must be defined before using in round function
	int tpq = (int)file->getTicksPerQuarterNote() / 4;
	if (Maths::round(pair.noteA.duration, tpq) > pair.noteA.duration)
	{
		pair.noteA.duration = file->getTicksPerQuarterNote();
		pair.noteB.duration = file->getTicksPerQuarterNote();
	}
}

void MarkovChain::cleanOutputFilenameSub(std::string & filename, char & toCheck)
{
	size_t position = filename.find(toCheck);
	if (position != std::string::npos)
	{
		filename.erase(position);
		cleanOutputFilenameSub(filename, toCheck);
	}
}

void MarkovChain::cleanOutputFilename()
{
	//Check for invalid characters
	std::vector<char> invalidCharacters = { '\'', '/', ':', '*', '?', '"', '<', '>', '|', '\n' };
	for (char character : invalidCharacters)
	{
		if (outputFilename.find(character) != std::string::npos)
		{
			cleanOutputFilenameSub(outputFilename, character);
		}
	}
	//Check if it contains the .mid extension already, remove it as it'll be appended later
	size_t position = outputFilename.find('.');
	if (position != std::string::npos)
	{
		std::string parsedFilename = outputFilename.substr(0, position);
		if (parsedFilename.size() < 1)
		{
			outputFilename = defaultFilename;
		}
		else
		{
			outputFilename = parsedFilename;
		}
	}
}

MidiFile& MarkovChain::generateNewMidiFile()
{
	Notes noteClass;
	MidiFile newFile;
	newFile.addTrack();
	newFile.setTicksPerQuarterNote(file->getTicksPerQuarterNote());
	newFile[0].push_back(MidiEvent());
	newFile[0][0].setMetaTempo(tempoQuartPerMin);

	int trackLength = 0;
	for (int i = 0; i < file[0][mostPopulatedTrack].getSize(); i++)
	{
		if (file[0][mostPopulatedTrack][i].isNoteOn())
		{
			trackLength++;
		}
	}

	//Build new track.
	long currTick = 0;
	int trackLengthIndex = trackLength / parsingNumber;
	NotePair rootPair(startingPair.noteA, startingPair.noteB);
	cleanRootPair(rootPair);

	if (doesGranularityNeedSequenceChecking())
	{
		cleanTables();
	}

	for (int a = 0; a < parsingNumber; a++)
	{
		for (int i = 0; i < trackLengthIndex; i++)
		{
			rootPair = getNextNote(rootPair.noteA, rootPair.noteB, a);
			if (rootPair.noteA.cNote == "Err" || rootPair.noteB.cNote == "Err")
			{
				rootPair = startingPair;
			}
			writeNote(newFile, rootPair.noteA, currTick, noteClass);
		
			int difference = Random::getRandomNumber(0, 100);
			if (difference < 10)
			{
				writePause(currTick, Maths::round(pauseCounter.calculate(), (file->getTicksPerQuarterNote() / 4)));
			}
		}
	}

	const std::string newfilename = (outputFilename + ".mid");
	newFile.write(newfilename);
	return newFile;
}

void MarkovChain::writePause(long& tick, long length)
{
	tick += length;
}

void MarkovChain::writeNote(MidiFile & newFile, Note newNote, long& currTick, Notes noteClass)
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