#ifndef FILE_HANDLING_H
#define FILE_HANDLING_H

#include "Data.h"
#include <fstream>
#include <iostream>
#include <boost/filesystem.hpp>

// Define errors along with preset text
#define UNEXPECTED_ERROR    "An unexpected error has occured "
#define FILE_HANDLING_ERROR "The specified file could not be accessed "

// Define logging messages
#define LOADED_FILE         "A new file has been loaded "
#define TRACK_FOUND         "A track has been parsed out with Index: "

class FileHandling {
public:
	static bool createDirectoryIfNotValid (std::string path);
	static bool writeToLog                (std::string message);
	static bool writeErrorToLog           (std::string error, std::string cause="");
	static bool save					  (DATA* data);
	static bool save					  (std::string newPath, DATA* data);
	static bool load					  ();
	static bool load					  (std::string newPath);


private:
	static const std::string path;
};

#endif