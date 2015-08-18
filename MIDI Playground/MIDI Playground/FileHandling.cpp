#include "FileHandling.h"
typedef std::vector<std::string> cat;

const std::string FileHandling::path = "saves/";

bool FileHandling::loadCFG()
{
	return loadCFG("cfg/","config.cfg");
}

bool FileHandling::loadCFG(std::string path, std::string filename)
{
	if (createDirectoryIfNotValid(path)) 
	{
		if (doesFileInDirectoryExist(path,filename))
		{
			std::ifstream file;
			file.open(path + filename, std::fstream::in);
			if (file.is_open())
			{
				//Read file, parse into individual lines
				std::vector<std::string>* lines = new std::vector<std::string>();
				std::string line = "";
				while (std::getline(file, line))
				{
					if (line.size() != 0)
					{
						lines->push_back(line);
					}
				}

				//Parse lines into key=value pairs. Delete original lines vector once parsed.
				std::vector<std::string> parsedLines;
				for (unsigned i = 0; i < lines->size(); i++)
				{
					std::istringstream strStream(lines->at(i));
					std::string item = "";
					while (std::getline(strStream, item, '='))
					{
						parsedLines.push_back(item);
					}
				}
				delete lines;

				if (int pos = contains(parsedLines, "generation") != 0)
					Settings::generation = std::stoi(parsedLines.at(pos));
				if (int pos = contains(parsedLines, "generationMaxLimit") != 0)
					Settings::generationMaxLimit = std::stoi(parsedLines.at(pos));
				return true;
			}
			writeErrorToLog(FILE_HANDLING_ERROR);
		}
	}
	return false;
}

bool FileHandling::createCFG()
{
	if (createDirectoryIfNotValid("cfg/"))
	{
		const std::string defaultSettings = "generation=0 \ngenerationMaxLimit=0\n";
		std::ofstream file;
		file.open("cfg/config.cfg", std::ofstream::out);
		if (file.is_open())
		{
			file << defaultSettings;
			file.close();
			return true;
		}
		file.close();
		writeErrorToLog(FILE_HANDLING_ERROR);
		return false;
	}
	writeErrorToLog(COULDNT_CREATE_DIRECTORY, "Creating CFG Directory");
	return false;
}

bool FileHandling::createDirectoryIfNotValid(std::string path)
{
	//Attempts to create the directory up to five times, cancels otherwise
	for (int i = 0; i < 5; i++)
	{
		if (!boost::filesystem::is_directory(boost::filesystem::path(path)))
		{
			boost::filesystem::create_directory(boost::filesystem::path(path));
		}
		else
		{
			return true;
		}
	}
	writeErrorToLog(COULDNT_CREATE_DIRECTORY);
	return false;
}

bool FileHandling::doesFileInDirectoryExist(std::string path, std::string filename)
{
	if (boost::filesystem::is_directory(boost::filesystem::path(path)))
	{
		boost::filesystem::path filePath(path);
		boost::filesystem::directory_iterator  end;
		for (boost::filesystem::directory_iterator it(filePath); it != end; ++it)
		{
			if (boost::filesystem::is_regular_file(it->status()))
			{
				boost::filesystem::path currPath = *it;
				std::cout << currPath << std::endl;
				if (currPath.string() == path.replace(path.length()-1,path.length()-1,"\\") + filename)
				{
					return true;
				}
			}
		}
	}
	return false;
}

bool FileHandling::writeToLog(std::string message)
{
	time_t t = time(0);
	struct tm* now = localtime(&t);

	createDirectoryIfNotValid("logs");
	std::string outputFile = "logs/logs.log";
	std::ofstream file;
	file.open(outputFile, std::ios::out | std::ios::app);
	if (file.is_open())
	{
		std::cout << message << std::endl;
		file << std::to_string(now->tm_year + 1900)+'-'+std::to_string(now->tm_mon + 1)+'-'+std::to_string(now->tm_mday)+'@'+std::to_string(now->tm_hour) + ':' + std::to_string(now->tm_min) + " : " + message << std::endl;
		file.close();
		return true;
	}
	writeErrorToLog(FILE_HANDLING_ERROR, "Logging");
	return false;
}

bool FileHandling::writeErrorToLog(std::string error, std::string cause)
{
	time_t t = time(0);
	struct tm* now = localtime(&t);

	createDirectoryIfNotValid("logs");
	std::string outputFile = "logs/error.log";
	std::ofstream file;
	file.open(outputFile, std::ios::out | std::ios::app);
	if (file.is_open())
	{
		file << error << cause << std::endl;
		std::cout << std::to_string(now->tm_year + 1900) + '-' + std::to_string(now->tm_mon + 1) + '-' + std::to_string(now->tm_mday) + '@' + std::to_string(now->tm_hour) + ':' + std::to_string(now->tm_min) + " : " + error << cause << std::endl;
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
	createDirectoryIfNotValid(newPath);
	std::ofstream file;
	file.open(newPath+"octaveStack.csv", std::ios::out);
	if (file.is_open())
	{
		int totalCount = NULL;
		for (unsigned i = 0; i < data->OctaveStack->size(); ++i)
		{
			//Let each line be a track with each row being a set of values
			for (unsigned y = 0; y < data->OctaveStack->at(i).size(); y++)
			{
				for (unsigned z = 0; z < data->OctaveStack->at(i).at(y).size(); z++)
				{
					file << data->OctaveStack->at(i).at(y).at(z) << ",";
					totalCount++;
				}
				file << std::endl;
			}
		}
		writeToLog(SAVED_CHUNK + std::to_string(totalCount));
		file.close();
	}
	file.open(newPath + "velocities.csv", std::ios::out);
	if (file.is_open())
	{
		int totalCount = NULL;
		for (unsigned i = 0; i < data->Velocities->size(); i++)
		{
			for (unsigned y = 0; y < data->Velocities->at(i).size(); y++)
			{
				for (unsigned z = 0; z < data->Velocities->at(i).at(y).size(); z++)
				{
					file << data->Velocities->at(i).at(y).at(z) << ",";
					totalCount++;
				}
				file << std::endl;
			}
		}
		writeToLog(SAVED_CHUNK + std::to_string(totalCount));
		file.close();
	}
	/*
	for (unsigned i = 0; i < data->NoteLengths->size(); i++)
	{

	}
	*/
	return true;
	FileHandling::writeErrorToLog(FILE_HANDLING_ERROR, "Saving");
	return false;
}

DATA* FileHandling::load()
{
	return load("saves/");
}

DATA* FileHandling::load(std::string newPath)
{
	DATA* data;
	if (doesFileInDirectoryExist(newPath))
	{
		std::ifstream file;
		file.open(newPath + "octaveStack.csv", std::fstream::in);
		if (file.is_open())
		{
			//Read all lines into a vector suitable for sorting later on.
			std::vector<std::string>* lines;
			std::string line = "";
			while (std::getline(file, line))
			{
				if (line.size() != NULL)
				{
					///////////////////////////////////
					auto octaveStack = data->OctaveStack;
					std::string currentLine = "";
					for (unsigned i = index; i < lines->size() && lines->at(i) != "Velocities"; i++)
					{
						std::istringstream strStream(currentLine);
						std::string item = "";
						while (std::getline(strStream, item, ','))
						{
							std::vector<int> values;
							values.push_back(std::stoi(item));
						}
					}
					///////////////////////////////////
				}
			}
			writeToLog(LOADED_CSV + lines->size());
		}
		file.close();
	}
	writeErrorToLog(FILE_HANDLING_ERROR, "Loading");
	return nullptr;
}

int FileHandling::contains(std::vector<std::string> varArray, std::string toSearch)
{
	for (int i = 0; i < varArray.size(); i++)
	{
		if (varArray.at(i) == toSearch)
		{
			return i;
		}
	}
	return NULL;
}

bool loadOctaveStack(std::vector<std::string> *lines, DATA *data, int index)
{
	
	return false;
}