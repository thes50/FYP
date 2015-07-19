#ifndef DATA_H
#define DATA_H

#include <vector>
#include <string>
#include <array>

struct DATA
{
	std::vector<std::string>* Files = new std::vector<std::string>();
	std::vector<std::array<int, 12>>*  CountedValues = new std::vector<std::array<int, 12>>();
	std::vector<std::vector<std::vector<int>>>* OctaveStack = new std::vector<std::vector<std::vector<int>>>();
	std::vector<std::vector<std::vector<int>>>* Velocities = new std::vector<std::vector<std::vector<int>>>();
	std::vector<std::vector<std::vector<int>>>* NoteLengths = new std::vector<std::vector<std::vector<int>>>();
	std::vector<std::vector<std::vector<int>>>* NoteContextFour = new std::vector<std::vector<std::vector<int>>>();
	std::vector<std::vector<std::vector<int>>>* NoteContextEight = new std::vector<std::vector<std::vector<int>>>();
	std::vector<std::vector<std::vector<int>>>* NoteContextSixteen = new std::vector<std::vector<std::vector<int>>>();
};

#endif