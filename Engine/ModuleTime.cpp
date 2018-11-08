#include "ModuleTime.h"
#include "Time.h"

ModuleTime::~ModuleTime()
{
	delete time; //TODO: Comprobar que no sea null
}

bool ModuleTime::Init()
{
	LOG("Init Time module");
	time = new Time();
	time->StartMS();
	return true;
}

update_status ModuleTime::Update()
{
	realDeltaTime = time->ReadMS();
	realClock += realDeltaTime;
	deltaTime = realDeltaTime * gameClockMultiplier;
	gameClock += deltaTime;
	time->ResetMS();
	++framesPassed;
	return UPDATE_CONTINUE;
}
