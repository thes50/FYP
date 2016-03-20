#ifndef MarkovChain_HH
#define MarkovChain_HH

#include "MidiFile.h"
#include "Random.h"
#include "Notes.h"
#include "FileHandling.h"
#include "Maths.cpp"
#include "PauseCounter.h"

#include <map>
#include <vector>
#include <string>
#include <numeric>
#include <algorithm>
#include <utility>

#ifdef _DEBUG
#include <iostream>
#endif

typedef std::pair<int, int> intPair;
typedef std::pair<std::string, int> strPair;
typedef std::map<std::string, std::map<std::string, int>> noteTable;
typedef std::map<intPair, std::map<intPair, int>> intTable;

class MarkovChain
{
private:
	//Each markov node needs to take Note, Octave, Velocity and Duration into account. 
	//Only way is to have seperate maps for each type and then treat each value selection as a new node.
	//Using a vector of maps allows for changes in the track to be mapped better when generating the new track
	std::vector<noteTable> noteCountTable;
	std::vector<intTable>  octaveCountTable;
	std::vector<intTable>  velocityCountTable;
	std::vector<intTable>  durationCountTable;
	PauseCounter           pauseCounter;
	int                    mostPopulatedTrack;
	int                    parsingNumber = 1;
	bool                   forceContinueThroughError = true;
	NotePair               startingPair;
	MidiFile               *file;

	NotePair getNextNote                     (Note rootA, Note rootB, int index);
	void writeNote                           (MidiFile& newFile, Note newNote, long& currTick, Notes notes); 
	void writePause                          (long& tick, long length);
	void cleanTables                         ();
	void cleanRootPair                       (NotePair& pair);
	template <class Map> void cleanTable     (Map& map, std::string table);
	template <class T, class P1, class P2> T countInSubmap(T a);
	template <class PMap, class Map> Map findNextGoodMap(PMap& parent, int index);
	bool isNoteFinishedConstruction          (bool noteNamed, bool velocity, bool octave, bool duration);
	bool isWithinSuitableRange               (long value);
	bool doesGranularityNeedSequenceChecking ();

public:
	MarkovChain                   ();
	MarkovChain                   (const std::string filepath);
	MarkovChain                   (MidiFile & midiFile);
	bool createFromMidiFile       (const std::string filepath);
	bool analyseMidiFile          ();
	bool getForceContinueFlag     ();
	void setBreakdownResolution   (int resolution);
	void setForceContinueFlag     (bool state);
	int  getBreakdownResolution   ();
	MidiFile& generateNewMidiFile ();
};

template<class Map>
inline void MarkovChain::cleanTable(Map& map, std::string table)
{
	int index = 0;;
	for (auto& fullMap : map)
	{
		for (auto& submap : fullMap)
		{
			for (auto& pair : submap.second)
			{
				if (pair.first == submap.first && submap.second.size() < 2)
				{
					//submap.second.erase(pair.first);
					fullMap.erase(pair.first);
#ifdef _DEBUG
					std::cout << "Removed value with index: " << index << " from table: " << table << std::endl;
#endif
					goto breakOut;
				}
			}
		}
		breakOut:;
		index++;
	}
}

template<class T, class P1, class P2>
inline T MarkovChain::countInSubmap(T map)
{
	int total = 0;
	std::map<P1, P2> newT;
	for (std::pair<P1, P2> pair : map)
	{
		total += pair.second;
	}
	for (std::pair<P1, P2> pair : map)
	{
		double newAmount = (double)pair.second / (double)total;
		pair.second = std::round(newAmount * 100);
		newT.insert(pair);
	}
	return newT;
}

template <class PMap, class Map> Map MarkovChain::findNextGoodMap(PMap& parent, int index)
{
	//Go the long way to give best chance of finding a suitable map.
	if ((parsingNumber - index) > index)
	{
		for (int i = index; i < (parsingNumber - 1); i++)
		{
			auto map = parent.at(i);
			if (parent.size() > 0)
			{
				return map;
			}
		}
	}
	else
	{
		for (int i = index; i >= 0; i--)
		{
			auto map = parent.at(i);
			if (parent.size() > 0)
			{
				return map;
			}
		}
	}
	return Map();
}
#endif