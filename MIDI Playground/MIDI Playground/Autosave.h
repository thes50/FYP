#ifndef AUTOSAVE_H
#define AUTOSAVE_H

#include "FileHandling.h"
#include <exception>
#include <thread>

class Autosave 
{
private:
	static int currentIndex;
public:
	static int index;
	static bool endThread;
	static void Auto();
	static DATA* dataStruct;
};

#endif