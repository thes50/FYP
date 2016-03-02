#ifndef MarkovChain_HH
#define MarkovChain_HH

#include "MidiFile.h"
#include "Random.h"
#include "Notes.h"
#include "FileHandling.h"
#include "Maths.cpp"

#include <map>
#include <vector>
#include <string>
#include <numeric>
#include <algorithm>
#include <utility>

typedef std::pair<int, int> intPair;

class MarkovChain
{
private:
	//Each markov node needs to take Note, Octave, Velocity and Duration into account. 
	//Only way is to have seperate maps for each type and then treat each 
	std::map<std::string, std::map<std::string, int>> noteCountTable;
	std::map<intPair, std::map<intPair, int>> octaveCountTable;
	std::map<intPair, std::map<intPair, int>> velocityCountTable; //Rounded to nearest Five
	std::map<intPair, std::map<intPair, int>> durationCountTable;
	std::map<int, int> pauseCounter;

	int mostPopulatedTrack;
	NotePair startingPair;
	MidiFile *file;
	NotePair getNextNote(Note rootA, Note rootB);
	void writeNote(MidiFile& newFile, Note newNote, int& currTick, Notes notes); 
	template <class T, class P1, class P2> T countInSubmap(T a);
	bool isNoteFinishedConstruction(bool noteNamed, bool velocity, bool octave, bool duration);
	bool isWithinSuitableRange(long value);

public:
	MarkovChain();
	MarkovChain(const std::string filepath);
	MarkovChain(MidiFile & midiFile);

	bool createFromMidiFile(const std::string filepath);
	bool analyseMidiFile();
	MidiFile& generateNewMidiFile();
};

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

#endif