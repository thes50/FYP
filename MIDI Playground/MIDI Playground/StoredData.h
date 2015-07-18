#ifndef STORED_DATA_H
#define STORED_DATA_H

#include <vector>
#include <array>
#include <boost\filesystem.hpp>

class StoredData
{
public:
	std::vector<std::array<int, 12>>*  CountedValues = new std::vector<std::array<int, 12>>();
	std::vector<boost::filesystem::path>* Files = new std::vector<boost::filesystem::path>();
	std::vector<std::vector<std::vector<int>>>* OctaveStack = new std::vector<std::vector<std::vector<int>>>();
	std::vector<std::vector<std::vector<int>>>* Velocities = new std::vector<std::vector<std::vector<int>>>();
	std::vector<std::vector<std::vector<int>>>* NoteLengths = new std::vector<std::vector<std::vector<int>>>();
	std::vector<std::vector<std::vector<int>>>* NoteContextFour = new std::vector<std::vector<std::vector<int>>>();
	std::vector<std::vector<std::vector<int>>>* NoteContextEight = new std::vector<std::vector<std::vector<int>>>();
	std::vector<std::vector<std::vector<int>>>* NoteContextSixteen = new std::vector<std::vector<std::vector<int>>>();
};

#endif