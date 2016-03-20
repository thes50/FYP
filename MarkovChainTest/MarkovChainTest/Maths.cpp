#include <cstdarg>

class Maths
{
public:
	static int average(int count, ...)
	{
		//Count starts from 1, not 0.
		va_list args;
		count--;
		va_start(args, count);
		int sum = 0;

		for (int i = 0; i < count; i++)
		{
			sum += va_arg(args, int);
		}

		va_end(args);
		return (sum / count);
	}

	static int average(int count, int sum)
	{
		return (sum / count);
	}

	static int round(unsigned value, long roundTo)
	{
		//Round param1 to closest Param2
		return ((value + roundTo / 2) / roundTo) * roundTo;
	}

	static int roundAverage(int roundTo, int count, int sum)
	{
		return round(average(count, sum), roundTo);
	}

	static int sum(int count, ...)
	{
		va_list args;
		va_start(args, count);
		int sum = 0;

		for (int i = 0; i < count; i++)
		{
			sum += va_arg(args, int);
		}
		va_end(args);
		return sum;
	}
};