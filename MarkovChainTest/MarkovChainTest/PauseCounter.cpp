#include "PauseCounter.h"

PauseCounter::PauseCounter()
{
	pauseCount = 0;
	pausesTotal = 0;
}

PauseCounter::~PauseCounter()
{
}

void PauseCounter::addPause(int length)
{
	pausesTotal += length;
	pauseCount++;
}

void PauseCounter::addTimeSinceLastPause(long time)
{
	timeSincePause += time;
}

long PauseCounter::calculate()
{
	return (pausesTotal / pauseCount);
}

long PauseCounter::getTimeSinceLastPause()
{
	return timeSincePause;
}