#ifndef FILE_HANDLING_H
#define FILE_HANDLING_H
#include <string>
#include <fstream>
#include <iostream>

// Define errors along with preset text
#define UNEXPECTED_ERROR "An unexpected error has occured "
#define FILE_HANDLING_ERROR "The specified file could not be accessed"

class FileHandling {
public:
	static bool writeErrorToLog(std::string error, std::string cause="");
	static bool save();
	static bool save(std::string newPath);
	static bool load();
	static bool load(std::string newPath);
private:
	static bool writeToFile(std::string structureName, std::ofstream& file);
	static const std::string path;
};

#endif