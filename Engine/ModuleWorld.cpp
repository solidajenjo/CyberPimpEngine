#include "Globals.h"
#include "Application.h"
#include "ModuleWorld.h"
#include "ModuleRender.h"
#include "ModuleProgram.h"
#include "ModuleCamera.h"
#include "ModuleTextures.h"
#include "ModuleModelLoader.h"
#include "ModuleEditor.h"
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
	std::vector<EntityMesh*> meshes = App->modelLoader->Load("BakerHouse.fbx");
	//std::vector<EntityMesh*> meshes = App->modelLoader->Load("Grid.fbx");

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

	//draw grid
	math::float4x4 identity = float4x4::identity;
	glUniformMatrix4fv(glGetUniformLocation(App->program->program,
		"model"), 1, GL_TRUE, &identity[0][0]);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(&(GLfloat)App->camera->view[0][0]);
	glLineWidth(0.5f);
	glBegin(GL_LINES);
	float d = 200.0f;
	for (float i = -d; i <= d; i += 1.0f)
	{
		glVertex3f(i, 0.0f, -d);
		glVertex3f(i, 0.0f, d);
		glVertex3f(-d, 0.0f, i);
		glVertex3f(d, 0.0f, i);
	}
	glEnd();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //clear texture frame buffer to avoid sub window drawing
	return UPDATE_CONTINUE;
}


// Called before quitting
bool ModuleWorld::CleanUp()
{
	for (std::vector<Entity*>::iterator it = worldEntities.begin(); it != worldEntities.end(); ++it)
		delete (*it);
	return true;
}

