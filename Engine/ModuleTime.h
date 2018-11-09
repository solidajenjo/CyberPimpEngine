#ifndef _MODULE_TIME_H
#define _MODULE_TIME_H

#include "Module.h"

class Time;

class ModuleTime :
	public Module
{
public:
	
	~ModuleTime();

	bool Init() override;
	update_status Update() override;
	

//members

	float deltaTime = 0.f, 
		realDeltaTime = 0.f, 
		gameClock = 0.f, 
		gameClockMultiplier = 1.f,
		realClock = 0.f;

	int framesPassed = 0;
	Time* time = nullptr;
};

#endif