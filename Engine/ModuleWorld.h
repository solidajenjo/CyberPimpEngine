#ifndef _MODULE_WORLD_H
#define _MODULE_WORLD_H

#include "Module.h"
#include "Globals.h"
#include <vector>

class Entity;

class ModuleWorld : public Module
{
public:
	ModuleWorld();
	~ModuleWorld();

	update_status Update();
	bool CleanUp();

public:
	void* context;
	std::vector<Entity*> worldEntities;
};

#endif