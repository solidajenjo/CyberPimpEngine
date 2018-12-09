#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleWindow.h"
#include "ModuleProgram.h"
#include "ModuleFrameBuffer.h"
#include "ModuleScene.h"
#include "ModuleEditorCamera.h"
#include "GameObject.h"
#include "Transform.h"
#include "sdl/include/SDL_video.h" 
#include "glew-2.1.0/include/GL/glew.h"
#include "MathGeoLib/include/Geometry/AABB.h"
#include "MathGeoLib/include/Geometry/LineSegment.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"


// Called before render is available
bool ModuleRender::Init()
{
	bool ret = true;
	LOG("Creating Renderer context");

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

	context = SDL_GL_CreateContext(App->window->window);

	glewInit();

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glEnable(GL_TEXTURE_2D);

	glClearDepth(1.0f);
	glClearColor(0.f, 0.f, 0.f, 1.f);

	App->frameBuffer->Start();
	App->gameFrameBuffer->Start();
	return ret;
}

update_status ModuleRender::PreUpdate()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	
	return UPDATE_CONTINUE;
}

// Called every draw update
update_status ModuleRender::Update()
{
	return UPDATE_CONTINUE;
}

update_status ModuleRender::PostUpdate()
{

	SDL_GL_SwapWindow(App->window->window);

	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleRender::CleanUp()
{
	LOG("Cleaning Module Renderer");
	renderizables.resize(0); //the meshes are cleanend by their owner gameobjects
	LOG("Cleaning Module Render. Done");
	return true;
}

void ModuleRender::Render(ComponentCamera* camera) const
{
	assert(camera != nullptr);
	for (std::list<GameObject*>::const_iterator it = renderizables.begin(); it != renderizables.end(); ++it) //render meshes //TODO: Check what happens with shared meshes
	{
		if (*it == nullptr) 
		{
			LOG("Missing mesh. Couldn't render.");
		}
		else
		{
			bool render = true;
			if (!(*it)->enabled || (frustumCulling && App->scene->sceneCamera != nullptr && !App->scene->sceneCamera->frustum.Intersects(*(*it)->aaBBGlobal) && !App->scene->sceneCamera->frustum.Contains(*(*it)->aaBBGlobal)))
			{
				render = false;
			}
			if(render)
			{
				for (std::list<Component*>::const_iterator it2 = (*it)->components.cbegin(); it2 != (*it)->components.cend(); ++it2)
				{
					if ((*it2)->type == Component::ComponentTypes::MESH_COMPONENT)
					{
						((ComponentMesh*)(*it2))->Render(camera, (*it)->transform);
					}
				}
			}			
			glEnd();
	
		}
		
	}
	glBindVertexArray(0);
}

const std::list<GameObject*>* ModuleRender::getRenderizables() const
{
	return &renderizables;
}

void ModuleRender::insertRenderizable(GameObject * newMesh)
{
	assert(newMesh != nullptr);
	renderizables.push_back(newMesh);
}
