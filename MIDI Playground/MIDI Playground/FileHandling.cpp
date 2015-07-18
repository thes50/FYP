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

bool FileHandling::save(StoredData* data)
{
	return FileHandling::save(path, data);
}
bool FileHandling::save(std::string newPath, StoredData* data)
{
	std::string outputFile = newPath;
	std::ofstream file;
	file.open(outputFile, std::ios::out);
	if (file.is_open())
	{
		file << "OctaveStack" << std::endl;
		for (unsigned i = 0; i < data->OctaveStack->size(); i++)
		{
			/*
				Let each line be a track with each row being a set of values
			*/
			for (unsigned y = 0; y < data->OctaveStack->at(i).size(); y++)
			{
				for (unsigned z = 0; data->OctaveStack->at(i).at(y).size(); z++)
				{

				}
			}
		}
		for (unsigned i = 0; i < data->Velocities->size(); i++)
		{

		}
		for (unsigned i = 0; i < data->NoteLengths->size(); i++)
		{

		}
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