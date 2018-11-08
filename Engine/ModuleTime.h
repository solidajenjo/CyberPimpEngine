#ifndef _MODULE_TIME_H
#define _MODULE_TIME_H

#include "Module.h"

class Time;

class ModuleTime :
	public Module
{
public:
	ModuleTime() : deltaTime(0.f), realDeltaTime(0.f), gameClock(0.f), gameClockMultiplier(1.f), realClock(0.f), framesPassed(0) {};
	~ModuleTime();

	bool Init();
	update_status Update();
	

//members

	float deltaTime, realDeltaTime, gameClock, gameClockMultiplier, realClock;
	UINT64 framesPassed;
	Time* time;
};

#endif