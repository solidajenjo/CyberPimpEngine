#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleWindow.h"
#include "ModuleProgram.h"
#include "ModuleFrameBuffer.h"
#include "ModuleSpacePartitioning.h"
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
#include "ComponentLight.h"


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

void ModuleRender::Render(const ComponentCamera* camera) const
{
	BROFILER_CATEGORY("Render", Profiler::Color::Aquamarine);
	assert(camera != nullptr);
	//Temporary - Collect all lights
	std::vector<ComponentLight*> directionals;
	std::vector<ComponentLight*> points;
	std::vector<ComponentLight*> spots;
	for (std::map<std::string, GameObject*>::const_iterator it = App->scene->sceneGameObjects.begin(); it != App->scene->sceneGameObjects.end(); ++it)
	{
		for (Component* comp : (*it).second->components)
		{
			if (comp->type == Component::ComponentTypes::LIGHT_COMPONENT)
			{
				ComponentLight* cL = (ComponentLight*)comp;
				switch (cL->lightType)
				{
				case ComponentLight::LightTypes::DIRECTIONAL:
					directionals.push_back(cL);
					break;
				case ComponentLight::LightTypes::POINT:
					cL->pointSphere.pos = cL->owner->transform->getGlobalPosition();
					points.push_back(cL);
					break;
				case ComponentLight::LightTypes::SPOT:
					spots.push_back(cL);
					break;
				}
			}
		}
	}
	if (frustumCulling && App->scene->sceneCamera != nullptr)
	{
		std::set<GameObject*> intersections;
		App->spacePartitioning->kDTree.GetIntersections(App->scene->sceneCamera->frustum, intersections);
		App->spacePartitioning->quadTree.GetIntersections(App->scene->sceneCamera->frustum, intersections);
		for (GameObject* go : intersections)
		{
			if (go->enabled && (App->scene->sceneCamera->frustum.Intersects(*go->aaBBGlobal) || App->scene->sceneCamera->frustum.Contains(*go->aaBBGlobal)))
			{
				for (Component* comp : go->components)
				{
					if (comp->type == Component::ComponentTypes::MESH_COMPONENT)
					{
						((ComponentMesh*)comp)->Render(camera, go->transform, directionals, points, spots);
					}
				}
			}
		}
	}
	else
	{
		for (std::list<const GameObject*>::const_iterator it = renderizables.begin(); it != renderizables.end(); ++it) //render meshes 
		{
			assert((*it) != nullptr);
			bool render = true;
			if (!(*it)->enabled || (frustumCulling && App->scene->sceneCamera != nullptr && !App->scene->sceneCamera->frustum.Intersects(*(*it)->aaBBGlobal) && !App->scene->sceneCamera->frustum.Contains(*(*it)->aaBBGlobal)))
			{
				render = false;
			}
			if (render)
			{
				for (std::list<Component*>::const_iterator it2 = (*it)->components.cbegin(); it2 != (*it)->components.cend(); ++it2)
				{
					if ((*it2)->type == Component::ComponentTypes::MESH_COMPONENT)
					{
						((ComponentMesh*)(*it2))->Render(camera, (*it)->transform, directionals, points, spots);
					}
				}
			}
			glEnd();
		}
	}
	glBindVertexArray(0);
}


void ModuleRender::insertRenderizable(GameObject * go)
{
	assert(go != nullptr);
	go->isRenderizable = true;
	renderizables.push_back(go);
}

void ModuleRender::removeRenderizable(const GameObject * go)
{
	assert(go != nullptr);
	renderizables.remove(go);
}
