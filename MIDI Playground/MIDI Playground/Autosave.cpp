#include "Autosave.h"
int Autosave::index = 0;
int Autosave::currentIndex = 0;
DATA* Autosave::dataStruct;
bool Autosave::endThread = false;

void Autosave::Auto()
{
	while (!endThread)
	{
		if (index % 5 == 0 && index != currentIndex)
		{
			//Save then incrememt hidden index to match values.
			FileHandling::save(dataStruct);
			currentIndex++;
		}
		else
		{
			try
			{
				std::this_thread::yield();
			}
			catch (const std::exception &e)
			{
				FileHandling::writeErrorToLog(ERROR_IN_THREAD, e.what());
			}
		}
	}
}
