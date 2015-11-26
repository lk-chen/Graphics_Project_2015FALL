#include "timing.h"
#include <ctime>
#include <iostream>

clock_t c_start, c_end;

void startTiming()
{
	c_start = clock();
}

void endTiming()
{
	c_end = clock();
	std::cout << "CPU time used: " << (unsigned int)(1000.0*(c_end - c_start) / CLOCKS_PER_SEC) << std::endl;
}

bool timesUp1s()
{
	auto c_end = clock();
	if ((c_end - c_start) / CLOCKS_PER_SEC >= 1)
	{
		c_start = c_end;
		return true;
	}
	else
	{
		return false;
	}
}