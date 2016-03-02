#include "MarkovChain.h"

int main()
{
	MarkovChain chain;
	bool status = chain.createFromMidiFile("moonlight-movement1.mid");
	if (status)
	{
		chain.analyseMidiFile();
		chain.generateNewMidiFile();
	}
}