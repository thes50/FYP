#include "MarkovChain.h"

int main()
{
	//fur elise.mid
	//"moonlight-movement1.mid";
	//"tnfdm.mid";
	//rondo alla turca
	std::string track = "moonlight-movement1.mid";
	MarkovChain chainSingle;
	MarkovChain chainBroken;
	bool statusSingle = chainSingle.createFromMidiFile(track);
	bool statusBroken = chainBroken.createFromMidiFile(track);

	//bool status = chain.createFromMidiFile("tnfdm.mid");
	if (statusSingle && statusBroken)
	{
		chainSingle.setOutputFilename("singleChunk");
		chainSingle.setBreakdownResolution(1);
		chainSingle.setForceContinueFlag(true);
		chainSingle.analyseMidiFile();
		chainSingle.generateNewMidiFile();

		chainBroken.setOutputFilename("20Chunks");
		chainBroken.setBreakdownResolution(20);
		chainBroken.setForceContinueFlag(true);
		chainBroken.analyseMidiFile();
		chainBroken.generateNewMidiFile();
	}
}