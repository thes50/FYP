#include "FileHandling.h"

const std::string FileHandling::trackPath = "/tracks/";
const std::string FileHandling::trackCopyPath = "/tracks/copies/";

void FileHandling::createDirectory(const std::string filepath)
{
	std::string filePath = (std::string)boost::filesystem::current_path().string() + filepath;
	if (!boost::filesystem::is_directory(boost::filesystem::path(filePath)))
	{
		boost::filesystem::create_directory(boost::filesystem::path(filePath));
	}
}

bool FileHandling::createCopyOfFile(const std::string file)
{
	if (doesFileExistInDirectory(file, trackPath))
	{
		std::ifstream iStream;
		iStream.open(((std::string)boost::filesystem::current_path().string())+trackPath+file, std::ifstream::binary);
		if (iStream.is_open())	
		{
			if (!doesDirectoryExist(trackCopyPath))
			{
				createDirectory(trackCopyPath);
			}
			std::ofstream oStream;
			oStream.open(((std::string)boost::filesystem::current_path().string())+trackCopyPath + file, std::ofstream::binary);
			if (oStream.is_open())
			{
				iStream.seekg(0, iStream.end);
				long int length = iStream.tellg();
				iStream.seekg(0, iStream.beg);

				if (length > maxSize)
				{
					return false;
				}

				//Store the file contents.
				char* fileBuffer = new char[length];
				iStream.read(fileBuffer, length);
				oStream.write(fileBuffer, length);

				iStream.close();
				oStream.close();
				delete[] fileBuffer;
				return true;
			}
		}
	}
	return false;
}

bool FileHandling::doesDirectoryExist(const std::string file)
{
	std::string currFile = (std::string)boost::filesystem::current_path().string() + file;
	if (boost::filesystem::is_directory(boost::filesystem::path(currFile)))
	{
		return true;
	}
	return false;
}

bool FileHandling::doesFileExistInDirectory(const std::string file, const std::string filepath)
{
	if (doesDirectoryExist(filepath))
	{
		boost::filesystem::path filePath((std::string)boost::filesystem::current_path().string()+filepath);
		boost::filesystem::directory_iterator end;
		for (boost::filesystem::directory_iterator it(filePath); it != end; ++it)
		{
			if (boost::filesystem::is_regular_file(it->status()))
			{
				boost::filesystem::path currPath = *it;
				std::string pathCopy = filePath.string();
				if (currPath.string() == pathCopy.replace(pathCopy.length() - 1, pathCopy.length() - 1, "\\") + file)
				{
					return true;
				}
			}
		}
	}
	return false;
}

bool FileHandling::checkDirectoryAndFile(const std::string path)
{
	std::string pathCopy = path;
	std::string filename = pathCopy.substr(pathCopy.length(), pathCopy.find('//'));
	std::string filepath = pathCopy.substr(0, pathCopy.find(filename));
	return false;
}
