#include "Random.h"

int Random::getRandomNumber(int min, int max)
{
	std::random_device randDev;
	std::mt19937 randMt(randDev());
	std::uniform_int_distribution<int> distribution(min, max);
	return distribution(randMt);
}
