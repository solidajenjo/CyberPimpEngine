#ifndef _MODULE_SCENE_H
#define _MODULE_SCENE_H

#include "Module.h"
#include <vector>

class GameObject;

class ModuleScene :
	public Module
{
public:
	
	bool Init() override;
	update_status Update() override;
	bool CleanUp() override; //clean when a new module is loaded & on exit

//members

	std::vector<GameObject*> sceneGameObjects;
};

#endif