#ifndef MarkovChain_HH
#define MarkovChain_HH

#include "MidiFile.h"
#include "Random.h"
#include "Notes.h"
#include "FileHandling.h"

#include <map>
#include <vector>
#include <string>
#include <algorithm>

class MarkovChain
{
private:
	//Each markov node needs to take Note, Octave, Velocity and Duration into account. 
	//Only way is to have seperate maps for each type and then treat each 
	std::map<std::string, std::map<std::string, int>> noteCountTable;
	std::map<std::string, std::map<std::string, int>> octaveCountTable;
	std::map<int, std::map<int, int>> velocityCountTable; //Rounded to nearest Five
	int mostPopulatedTrack;
	NotePair startingPair;
	MidiFile *file;
	NotePair getNextNote(Note rootA, Note rootB);
	void writeNote(MidiFile& newFile, Note newNote, int& currTick, Notes notes);

public:
	MarkovChain();
	MarkovChain(const std::string filepath);
	MarkovChain(MidiFile & midiFile);

	bool createFromMidiFile(const std::string filepath);
	bool analyseMidiFile();
	MidiFile& generateNewMidiFile();
};

#endif