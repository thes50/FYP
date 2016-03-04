#ifndef PAUSE_COUNTER_H
#define PAUSE_COUNTER_H

class PauseCounter
{
private:
	int pauseCount;
	long pausesTotal;
	long timeSincePause;

public:
	PauseCounter();
	~PauseCounter();
	void addPause(int length);
	void addTimeSinceLastPause(long time);
	long calculate();
	long getTimeSinceLastPause();
};

#endif