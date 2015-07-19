#include "FileHandling.h"

const std::string FileHandling::path = "saves/";

bool FileHandling::createDirectoryIfNotValid(std::string path) 
{
	if (!boost::filesystem::is_directory(boost::filesystem::path(path)))
	{
		boost::filesystem::create_directory(boost::filesystem::path(path));
	}
	return true;
}
bool FileHandling::writeToLog(std::string message)
{
	createDirectoryIfNotValid("logs");
	std::string outputFile = "logs/logs.log";
	std::ofstream file;
	file.open(outputFile, std::ios::out | std::ios::app);
	if (file.is_open())
	{
		std::cout << message << std::endl;
		file << message << std::endl;
		file.close();
		return true;
	}
	writeErrorToLog(FILE_HANDLING_ERROR, "Logging");
	return false;
}

bool FileHandling::writeErrorToLog(std::string error, std::string cause)
{
	createDirectoryIfNotValid("logs");
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

bool FileHandling::save(DATA* data)
{
	return FileHandling::save(path, data);
}
bool FileHandling::save(std::string newPath, DATA* data)
{
	std::string outputFile = newPath;
	createDirectoryIfNotValid(outputFile);
	std::ofstream file;
	file.open(outputFile, std::ios::out);
	if (file.is_open())
	{
		file << "OctaveStack" << std::endl;
		for (unsigned i = 0; i < data->OctaveStack->size(); i++)
		{
			//Let each line be a track with each row being a set of values
			for (unsigned y = 0; y < data->OctaveStack->at(i).size(); y++)
			{
				for (unsigned z = 0; data->OctaveStack->at(i).at(y).size(); z++)
				{
					file << data->OctaveStack->at(i).at(y).at(z) << ",";
				}
				file << std::endl;
			}
		}
		file << std::endl;
		file << "Velocities" << std::endl;

		for (unsigned i = 0; i < data->Velocities->size(); i++)
		{
			for (unsigned y = 0; y < data->Velocities->at(i).size(); y++)
			{
				for (unsigned z = 0; data->Velocities->at(i).at(y).size(); z++)
				{
					file << data->Velocities->at(i).at(y).at(z) << ",";
				}
				file << std::endl;
			}
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