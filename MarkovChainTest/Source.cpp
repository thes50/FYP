#include "MarkovChain.h"

double getPercentageChance()
{
	return 0.0;
}
void buildPercentageTable()
{

}
void buildNoteCountTable()
{
	/*
	Builds a map of maps in order to collect probability of next note being a specified value.
	*/
	/*std::string notesaksldjalksjd = "AEFAEGFEFAEFEAGBCBACADCDEFEABAEFAEGFBACFACDEFCFAEFAEGEADCFFEAFEABHACDEFEABDCF";
	std::map<char, std::map<char, int>>* notesTable = new std::map<char, std::map<char, int>>();
	for (int i = 0; i < notes.length(); i++)
	{
		//Check not end of stream
		if (i != notes.length())
		{
			int nextPos = i + 1;
			//Inserts new key if not found
			if (notesTable->find(notes[i]) == notesTable->end())
			{
				Value localValue;
				notesTable->insert(std::make_pair(notes[i], localValue));
			}

			//Inserts new value(key, value) if not found
			if (notesTable->at(notes[i]).find(notes[nextPos]) == notesTable->at(notes[i]).end())
			{
				notesTable->at(notes[i]).insert(std::pair<int, char>(notes[nextPos], 1));
			}
			else
			{
				notesTable->at(notes[i]).at(notes[nextPos])++;
			}
		}
	}*/
}

int main()
{
	MarkovChain chain;
	bool status = chain.createFromMidiFile("moonlight-movement1.mid");
	if (status)
	{
		chain.analyseMidiFile();
		chain.generateNewMidiFile();
	}
	int a = 0;
	MarkovChain testChain("fileout.mid");
}