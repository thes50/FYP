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

typedef std::pair<int, int> intPair;
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
	PauseCounter pauseCounter;

	int mostPopulatedTrack;
	const int parsingNumber = 16;
	NotePair startingPair;
	MidiFile *file;
	NotePair getNextNote(Note rootA, Note rootB, int index);
	void writeNote(MidiFile& newFile, Note newNote, long& currTick, Notes notes); 
	void writePause(long& tick, long length);
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