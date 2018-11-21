#include "ModuleTime.h"
#include "TimeClock.h"

ModuleTime::~ModuleTime()
{
	RELEASE(time);
}

bool ModuleTime::Init()
{
	LOG("Init Time module");
	time = new TimeClock();
	time->StartMS();
	return true;
}

update_status ModuleTime::Update()
{
	realDeltaTime = time->ReadMS() / 1000.f;
	realClock += realDeltaTime;
	deltaTime = realDeltaTime * gameClockMultiplier;
	gameClock += deltaTime;
	time->ResetMS();
	++framesPassed;
	return UPDATE_CONTINUE;
}
