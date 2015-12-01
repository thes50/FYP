#include "Notes.h"

Note Notes::getNoteType(MidiEvent evt)
{
	Note newNote;
	if (evt.isNoteOn())
	{
		for (int i = 0; i < 12; i++)
		{
			Note localNote = Notes::_contains(evt.getKeyNumber(), noteLetters[i]);
			if (localNote.isValid)
			{
				newNote.cNote = localNote.cNote;
				newNote.isValid = true;
				newNote.octave = localNote.octave;
				newNote.duration = evt.getDurationInSeconds();
				newNote.velocity = evt.getVelocity();
				return newNote;
			}
		}
	}
	newNote.isValid = false;
	return newNote;
}

int Notes::getNoteMidiValue(Note newNote)
{
	int retVal = 1000;
	if (noteMidiValues.find(newNote.cNote) != noteMidiValues.end())
	{
		retVal = noteMidiValues.at(newNote.cNote).at(newNote.octave);
	}
	return retVal;
}

Note::Note(std::string name, int oct, int vel, double dur, bool isVal)
{
	cNote = name;
	octave = oct;
	velocity = vel;
	duration = dur;
	isValid = isVal;
}

Notes::Notes()
{
	noteMidiValues.insert(std::make_pair("Ab", std::vector<int> { 8, 20, 32, 44, 56, 68, 80, 92, 104, 116 }));
	noteMidiValues.insert(std::make_pair("A",  std::vector<int> { 9, 21, 33, 45, 57, 69, 81, 93, 105, 117 }));
	noteMidiValues.insert(std::make_pair("Bb", std::vector<int> { 10, 22, 34, 46, 58, 70, 82, 94, 106, 118 }));
	noteMidiValues.insert(std::make_pair("B",  std::vector<int> { 11, 23, 35, 47, 59, 71, 83, 95, 107, 119 }));
	noteMidiValues.insert(std::make_pair("C",  std::vector<int> { 0, 12, 24, 36, 48, 60, 72, 84, 96, 108, 120 }));
	noteMidiValues.insert(std::make_pair("Db", std::vector<int> { 1, 13, 25, 37, 49, 61, 73, 85, 97, 109, 121 }));
	noteMidiValues.insert(std::make_pair("D",  std::vector<int> { 2, 14, 26, 38, 50, 62, 74, 86, 98, 110, 122 }));
	noteMidiValues.insert(std::make_pair("Eb", std::vector<int> { 3, 15, 27, 39, 51, 63, 75, 87, 99, 111, 123 }));
	noteMidiValues.insert(std::make_pair("E",  std::vector<int> { 4, 16, 28, 40, 52, 64, 76, 88, 100, 112, 124 }));
	noteMidiValues.insert(std::make_pair("F",  std::vector<int> { 5, 15, 29, 41, 53, 65, 77, 89, 101, 113, 125 }));
	noteMidiValues.insert(std::make_pair("Gb", std::vector<int> { 4, 16, 28, 40, 52, 64, 76, 88, 100, 112, 124 }));
	noteMidiValues.insert(std::make_pair("G",  std::vector<int> { 6, 16, 30, 42, 54, 66, 78, 90, 102, 114, 126 }));
}

Note Notes::_contains(const int noteVal, std::string key)
{
	std::vector<int> localNoteVec = noteMidiValues.at(key);

	if (localNoteVec.size() > 0)
	{
		int i = 0;
		for (int note : localNoteVec) 
		{ 
			if (note == noteVal) 
			{ 
				return Note(key, i, 0, 0, true); 
			} 
			i++;
		}
	}
	return Note("Err", 0, 0, 0, false);
}

int Notes::_getNoteValue(const Note newNote, std::string key)
{
	std::vector<int> localNoteVec = noteMidiValues.at(key);
	int oct = newNote.octave;

	if (localNoteVec.size() <= (oct + 1))
	{
		return localNoteVec.at(oct);
	}
	return 0;
}
