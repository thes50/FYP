#include "FileHandling.h"

const std::string FileHandling::path = "saves/";

bool writeErrorToLog(std::string error, std::string cause)
{
	std::string outputFile = "logs/error.log";
	std::ofstream file;
	file.open(outputFile, std::ios::out | std::ios::app);
	if (file.is_open())
	{
		file << error << cause << std::endl;
		std::cout << error << cause << std::endl;
		file.close();
		return true;
	}
	//No point reporting a write failure given the write function failed.
	return false;
}

bool FileHandling::save()
{
	return FileHandling::save(path);
}
bool FileHandling::save(std::string newPath)
{
	std::string outputFile = newPath;
	std::ofstream file;
	file.open(outputFile, std::ios::out);
	if (file.is_open())
	{
		//foreach struct in StoredData
		file.close();
		return true;
	}
	FileHandling::writeErrorToLog(FILE_HANDLING_ERROR, "Saving");
	return false;
}

bool FileHandling::load()
{
	return false;
}

bool FileHandling::writeToFile(std::string structureName, std::ofstream& file)
{
	if (structureName == "")
	{
		return true;
	}
	return false;
}