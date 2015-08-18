#ifndef FILE_HANDLING_H
#define FILE_HANDLING_H

#include "Data.h"
#include "Settings.h"
#include <ctime>
#include <fstream>
#include <sstream>
#include <iostream>
#include <boost/filesystem.hpp>

// Define errors along with preset text
#define UNEXPECTED_ERROR    "An unexpected error has occured "
#define FILE_HANDLING_ERROR "The specified file could not be accessed "
#define COULDNT_CREATE_DIRECTORY "The specified directory could not be found or created."
#define ERROR_IN_THREAD     "Error found in worker thread: "

// Define logging messages
#define LOADED_FILE         "A new file has been loaded "
#define TRACK_FOUND         "A track has been parsed out with Index: "
#define SAVED_CHUNK         "New chunk saved with values length: " 
#define LOADED_CSV	        "A save file has been loaded with line count: "
#define FIRST_LOAD          "Setting up for the first time"
#define LOADING_PREVIOUS_SETTINGS "Loaded settings from previous runtime file"

class FileHandling : Settings
{
public:
	static bool loadCFG                   ();
	static bool loadCFG                   (std::string path, std::string filename);
	static bool createCFG                 ();
	static bool createDirectoryIfNotValid (std::string path);
	static bool doesFileInDirectoryExist  (std::string path = "", std::string filename = "");
	static bool writeToLog                (std::string message);
	static bool writeErrorToLog           (std::string error, std::string cause="");
	static bool save					  (DATA* data);
	static bool save					  (std::string newPath, DATA* data);
	static DATA* load					  ();
	static DATA* load					  (std::string newPath);


private:
	static int contains                   (std::vector<std::string> varArray, std::string toSearch);
	static const std::string path;
};

#endif