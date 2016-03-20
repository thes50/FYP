#include "MarkovChain.h"

int main()
{
	MarkovChain chain;
	bool status = chain.createFromMidiFile("moonlight-movement1.mid");
	//bool status = chain.createFromMidiFile("tnfdm.mid");
	if (status)
	{
		chain.setBreakdownResolution(30);
		chain.setForceContinueFlag(true);
		chain.analyseMidiFile();
		chain.generateNewMidiFile();
	}
}