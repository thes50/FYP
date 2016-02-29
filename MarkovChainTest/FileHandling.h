#ifndef FILE_HANDLING_HH
#define FILE_HANDLING_HH

#include "boost/filesystem.hpp"
#include <string>
#include <fstream>

class FileHandling
{
private:
	static const long maxSize = 5242880;
	static void createDirectory(const std::string filepath);

public:
	static const std::string trackPath;
	static const std::string trackCopyPath;
	static bool createCopyOfFile(const std::string file);
	static bool doesDirectoryExist(const std::string file);
	static bool doesFileExistInDirectory(const std::string file, const std::string filepath = trackPath);
	static bool checkDirectoryAndFile(const std::string path);
};

#endif