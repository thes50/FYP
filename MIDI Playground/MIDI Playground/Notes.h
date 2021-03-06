#ifndef NOTES_H
#define NOTES_H

struct NoteReturnGroup
{
	NoteReturnGroup(bool valid, char newNote, int oct);
	bool isValid;
	char note;
	int octave;
};

class Notes {
private:
	static const int Ab[];
	static const int A[];
	static const int Bb[];
	static const int B[];
	static const int C[];
	static const int Db[];
	static const int D[];
	static const int Eb[];
	static const int E[];
	static const int F[];
	static const int Gb[];
	static const int G[];


public:
	NoteReturnGroup contains(int noteVal, char noteSet);

};


#endif