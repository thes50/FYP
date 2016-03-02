#ifndef ANALYSIS_H
#define ANALYSIS_H
#include <vector>
#include <boost\filesystem.hpp>

namespace Analysis 
{
	namespace NoteVelocities 
	{
		std::vector<std::vector<int>> getNoteVelocityContextFourNote(boost::filesystem::path file);
		std::vector<std::vector<int>> getNoteVelocityContextEightNote(boost::filesystem::path file);
		std::vector<std::vector<int>> getNoteVelocityContextSixteenNote(boost::filesystem::path file);
	}
}

#endif