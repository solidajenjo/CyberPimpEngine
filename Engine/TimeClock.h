#ifndef _TIME_CLOCK_H_
#define _TIME_CLOCK_H_
#include "SDL/include/SDL_timer.h"

class TimeClock
{
public:

	void StartMS() //start milliseconds timer
	{
		started_ms = true;
		startTime_ms = SDL_GetTicks();
		ms = 0;
	}

	Uint64 ReadMS() const //read milliseconds timer
	{
		if (started_ms)
			return SDL_GetTicks() - startTime_ms;
		else
			return ms;
	}
	void StopMS() { //stop milliseconds timer
		started_ms = false;
		ms = SDL_GetTicks() - startTime_ms;
	}

	void ResetMS()
	{
		startTime_ms = SDL_GetTicks();
	}

	void StartUS() //start microseconds timer
	{
		started_us = true;
		startTicks_us = SDL_GetPerformanceCounter();
		us = 0;
	}

	float ReadUS() const //read microseconds timer
	{
		if (started_us)
			return (SDL_GetPerformanceCounter() - startTicks_us)  / (float)SDL_GetPerformanceFrequency() * 1000.f;
		else
			return us;
	}
	void StopUS() { //start microseconds timer
		started_us = false;
		us = (double) (SDL_GetPerformanceCounter() - startTicks_us)  / (float)SDL_GetPerformanceFrequency() * 1000.f;
	}

	void ResetUS()
	{
		startTicks_us = SDL_GetPerformanceCounter();
	}

	Uint64 ms = 0, startTime_ms = 0, startTicks_us = 0;
	float us = 0.f;
	bool started_ms = false, started_us = false;
};

#endif