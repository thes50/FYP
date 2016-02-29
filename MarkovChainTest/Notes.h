#ifndef NOTES_HH
#define NOTES_HH
#define NOTE_ON  0x90
#define NOTE_OFF 0x80

#include "MidiEvent.h"
#include <vector>
#include <map>
#include <string>

struct Note
{
public:
	Note(std::string name = "Err", int oct = 0, int vel = 0, double dur = 0, bool isVal = true);
	Note(MidiEvent* evt);
	std::string cNote;
	int  octave;
	int velocity;
	int duration;
	bool isValid = true;
};

struct NotePair
{
public:
	Note noteA;
	Note noteB;
	NotePair(Note a, Note b) { noteA = a; noteB = b; }
	NotePair() {}
};

class Notes
{
private:
	std::string noteLetters[12]{ "Ab", "A", "Bb", "B", "C", "Db", "D", "Eb", "E", "F", "Gb", "G" } ;
	//Map containing <noteName, vector<noteValues>>
	std::map<std::string, std::vector<int>> noteMidiValues;

	Note _contains(const int noteVal, std::string key);
	int _getNoteValue(const Note newNote, std::string key);

public:
	Notes();

	Note getNoteType(MidiEvent* evt);
	int getNoteMidiValue(const Note newNote);
	int getOctave(const Note newNote, const int noteVal);
};

#endif