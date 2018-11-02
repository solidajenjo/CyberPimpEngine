#include "Globals.h"
#include "Application.h"
#include "ModuleWorld.h"
#include "ModuleWindow.h"
#include "Entity.h"
#include "SDL.h"
#include "GL/glew.h"

ModuleWorld::ModuleWorld()
{
}

// Destructor
ModuleWorld::~ModuleWorld()
{
}


// Called every draw update
update_status ModuleWorld::Update()
{
	for (std::vector<Entity*>::iterator it = worldEntities.begin(); it != worldEntities.end(); ++it)
		(*it)->Render();
	return UPDATE_CONTINUE;
}


// Called before quitting
bool ModuleWorld::CleanUp()
{
	for (std::vector<Entity*>::iterator it = worldEntities.begin(); it != worldEntities.end(); ++it)
		delete (*it);
	return true;
}

