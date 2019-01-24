#ifndef _MODULE_GAMEPLAYSYSTEM_H
#define _MODULE_GAMEPLAYSYSTEM_H

#include "Module.h"
#include "Globals.h"

class ModuleGamePlaySystem :
	public Module
{
public:
	
	bool Init() override;
	update_status Update() override;
	void LogDll(const char *msg);
};


#endif