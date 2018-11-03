#include "Globals.h"
#include "Application.h"
#include "ModuleWorld.h"
#include "ModuleRender.h"
#include "ModuleProgram.h"
#include "ModuleCamera.h"
#include "ModuleTextures.h"
#include "ModuleModelLoader.h"
#include "MathGeoLib.h"
#include "Entity.h"
#include "EntityMesh.h"
#include "SDL.h"
#include "GL/glew.h"

ModuleWorld::ModuleWorld()
{
}

// Destructor
ModuleWorld::~ModuleWorld()
{
}

bool ModuleWorld::Init()
{
	//std::vector<EntityMesh*> meshes = App->modelLoader->Load("Dice.fbx");
	std::vector<EntityMesh*> meshes = App->modelLoader->Load("BakerHouse.fbx");

	for (unsigned i = 0; i < meshes.size(); ++i)
		worldEntities.push_back(meshes[i]);
	return true;
}
// Called every draw update
update_status ModuleWorld::Update()
{
	glBindFramebuffer(GL_FRAMEBUFFER, App->renderer->framebuffer); //Draw to frame buffer

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //clear texture frame buffer
	
	for (std::vector<Entity*>::iterator it = worldEntities.begin(); it != worldEntities.end(); ++it)
		(*it)->Render();
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return UPDATE_CONTINUE;
}


// Called before quitting
bool ModuleWorld::CleanUp()
{
	for (std::vector<Entity*>::iterator it = worldEntities.begin(); it != worldEntities.end(); ++it)
		delete (*it);
	return true;
}

