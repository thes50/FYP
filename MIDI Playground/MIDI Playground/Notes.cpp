#include "Notes.h"

const int Notes::Ab[] = { 8, 20, 32, 44, 56, 68, 80, 92, 104, 116 };
const int Notes::A [] = { 9, 21, 33, 45, 57, 69, 81, 93, 105, 117 };
const int Notes::Bb[] = { 10, 22, 34, 46, 58, 70, 82, 94, 106, 118 };
const int Notes::B [] = { 11, 23, 35, 47, 59, 71, 83, 95, 107, 119 };
const int Notes::C [] = { 0, 12, 24, 36, 48, 60, 72, 84, 96, 108, 120 };
const int Notes::Db[] = { 1, 13, 25, 37, 49, 61, 73, 85, 97, 109, 121 };
const int Notes::D [] = { 2, 14, 26, 38, 50, 62, 74, 86, 98, 110, 122 };
const int Notes::Eb[] = { 3, 15, 27, 39, 51, 63, 75, 87, 99, 111, 123 };
const int Notes::E [] = { 4, 16, 28, 40, 52, 64, 76, 88, 100, 112, 124 };
const int Notes::F [] = { 5, 15, 29, 41, 53, 65, 77, 89, 101, 113, 125 };
const int Notes::Gb[] = { 6, 16, 30, 42, 54, 66, 78, 90, 102, 114, 126 };
const int Notes::G [] = { 7, 17, 31, 43, 55, 67, 79, 91, 103, 115, 127 };

NoteReturnGroup Notes::contains(int noteVal, char noteSet)
{
	if (noteSet == 'Ab')      { int i = 0;  for (int note : Ab) { if (note == noteVal){ return NoteReturnGroup(true, 'Ab', i); } i++; } }
	else if (noteSet == 'A') { int i = 0;  for (int note : A)   { if (note == noteVal){ return NoteReturnGroup(true, 'Ab', i); } i++; } }
	else if (noteSet == 'Bb') { int i = 0;  for (int note : Bb) { if (note == noteVal) { return NoteReturnGroup(true, 'Ab', i); } i++; } }
	else if (noteSet == 'B') { int i = 0;  for (int note : B) { if (note == noteVal) { return NoteReturnGroup(true, 'Ab', i); } i++; } }
	else if (noteSet == 'C')  { int i = 0;  for (int note : C)  { if (note == noteVal){ return NoteReturnGroup(true, 'Ab', i) } i++; } }
	else if (noteSet == 'Db') { int i = 0;  for (int note : Db) { if (note == noteVal){ return NoteReturnGroup(true, 'Ab', i) } i++; } }
	else if (noteSet == 'D')  { int i = 0;  for (int note : D)  { if (note == noteVal){ return NoteReturnGroup(true, 'Ab', i) } i++; } }
	else if (noteSet == 'Eb') { int i = 0;  for (int note : Eb) { if (note == noteVal){ return NoteReturnGroup(true, 'Ab', i) } i++; } }
	else if (noteSet == 'E')  { int i = 0;  for (int note : E)  { if (note == noteVal){ return NoteReturnGroup(true, 'Ab', i) } i++; } }
	else if (noteSet == 'F')  { int i = 0;  for (int note : F)  { if (note == noteVal){ return NoteReturnGroup(true, 'Ab', i) } i++; } }
	else if (noteSet == 'Gb') { int i = 0;  for (int note : Gb) { if (note == noteVal){ return NoteReturnGroup(true, 'Ab', i) } i++; } }
	else if (noteSet == 'G')  { int i = 0;  for (int note : G)  { if (note == noteVal){ return NoteReturnGroup(true, 'Ab', i) } i++; } }
	return NoteReturnGroup(false, 'Err', 0);
}

NoteReturnGroup::NoteReturnGroup(bool valid, char newNote, int oct)
{
	isValid = valid;
	note = newNote;
	octave = oct;
}
