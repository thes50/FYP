#ifndef DATA_H
#define DATA_H

#include <vector>
#include <string>
#include <array>

struct DATA
{
	std::vector<std::string>* Files = new std::vector<std::string>();
	std::vector<std::vector<std::vector<int>>>* Octave = new std::vector<std::vector<std::vector<int>>>();
	std::vector<std::vector<std::vector<int>>>* Velocities = new std::vector<std::vector<std::vector<int>>>();
	std::vector<std::vector<std::vector<int>>>* NoteLengths = new std::vector<std::vector<std::vector<int>>>();
};

#endif