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

	alphaRenderizables.resize(MAX_ALPHA_MESHES);

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

void ModuleRender::Render(const ComponentCamera* camera) 
{
	BROFILER_CATEGORY("Render", Profiler::Color::Aquamarine);
	assert(camera != nullptr);
	std::set<GameObject*> lightIntersections;
	if (App->scene->sceneCamera != nullptr)
		App->spacePartitioning->aabbTreeLighting.GetIntersections(App->scene->sceneCamera->frustum, lightIntersections);
	else
		App->spacePartitioning->aabbTreeLighting.GetIntersections(App->camera->editorCamera.frustum, lightIntersections);

	//Separate lights by type
	std::vector<ComponentLight*> directionals; //TODO: Remove push_backs
	std::vector<ComponentLight*> points;
	std::vector<ComponentLight*> spots;
	for (GameObject* go :  lightIntersections)
	{
		ComponentLight* cL = (ComponentLight*)go->components.front();

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
			cL->pointSphere.pos = cL->owner->transform->getGlobalPosition() + cL->owner->transform->front * cL->spotDistance;
			spots.push_back(cL);
			break;
		}
	}

	alphaAmount = 0u;

	if (frustumCulling && App->scene->sceneCamera != nullptr)
	{
		std::set<GameObject*> intersections;
		
		App->spacePartitioning->kDTree.GetIntersections(App->scene->sceneCamera->frustum, intersections);
		App->spacePartitioning->aabbTree.GetIntersections(App->scene->sceneCamera->frustum, intersections);
		for (GameObject* go : intersections)
		{
			if (go->enabled && (App->scene->sceneCamera->frustum.Intersects(*go->aaBBGlobal) || App->scene->sceneCamera->frustum.Contains(*go->aaBBGlobal)))
			{
				for (Component* comp : go->components)
				{
					if (comp->type == Component::ComponentTypes::MESH_COMPONENT)
					{
						if (((ComponentMesh*)comp)->material->useAlpha)
						{
							alphaRenderizables[alphaAmount++] = go;
						}
						else
							((ComponentMesh*)comp)->Render(camera, go->transform, directionals, points, spots);
					}
				}
			}
		}
	}
	else
	{
		for (std::list<GameObject*>::iterator it = renderizables.begin(); it != renderizables.end(); ++it) //render meshes 
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
						if (((ComponentMesh*)(*it2))->material->useAlpha)
						{
							alphaRenderizables[alphaAmount++] = (*it);
						}
						else
							((ComponentMesh*)(*it2))->Render(camera, (*it)->transform, directionals, points, spots);
					}
				}
			}
			glEnd();
		}
	}

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	if (alphaAmount > 1u)
		std::sort(alphaRenderizables.begin(), alphaRenderizables.begin() + alphaAmount + 1, [&camera](const GameObject* go1, const GameObject* go2)
		{		
			if (go2 == nullptr || go1 == nullptr)
				return false;
			return go1->transform->getGlobalPosition().Distance(camera->frustum.pos) < go2->transform->getGlobalPosition().Distance(camera->frustum.pos);
		});

	for (unsigned i = 0u; i < alphaAmount; ++i)
	{
		for (Component* comp : alphaRenderizables[i]->components)
		{
			if (comp->type == Component::ComponentTypes::MESH_COMPONENT)
			{
				((ComponentMesh*)comp)->Render(camera, alphaRenderizables[i]->transform, directionals, points, spots);
			}
		}
	}
	glDisable(GL_BLEND);

}


void ModuleRender::insertRenderizable(GameObject * go)
{
	assert(go != nullptr);
	go->isRenderizable = true;
	renderizables.push_back(go);
}

void ModuleRender::removeRenderizable(GameObject * go)
{
	assert(go != nullptr);
	renderizables.remove(go);
}
