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
	std::map<int, std::map<int, int>> velocityCountTable; //Rounded to nearest Five
	int mostPopulatedTrack;
	MidiFile *file;
	Note getNextNote(Note root);

public:
	MarkovChain();
	MarkovChain(const std::string filepath);
	MarkovChain(MidiFile & midiFile);

	bool createFromMidiFile(const std::string filepath);
	bool analyseMidiFile();
	MidiFile& generateNewMidiFile();
};

#endif