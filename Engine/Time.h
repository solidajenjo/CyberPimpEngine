#ifndef _TIME_H_
#define _TIME_H_
#include "SDL.h"

class Time
{
public:

	Time() : ms(0), us(0), startTime_ms(0), startTicks_us(0), started_ms(false), started_us(false){};

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

	void StartUS() //start microseconds timer
	{
		started_us = true;
		startTicks_us = SDL_GetPerformanceCounter();
		us = 0;
	}

	Uint64 ReadUS() const //read microseconds timer
	{
		if (started_us)
			return (SDL_GetPerformanceCounter() - startTicks_us)  / (double)SDL_GetPerformanceFrequency() * 1000;
		else
			return us;
	}
	void StopUS() { //start microseconds timer
		started_us = false;
		us = (double) (SDL_GetPerformanceCounter() - startTicks_us)  / (double)SDL_GetPerformanceFrequency() * 1000;
	}

	Uint64 ms, us, startTime_ms, startTicks_us;
	bool started_ms, started_us;
};

#endif