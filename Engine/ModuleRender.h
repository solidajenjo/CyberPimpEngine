#ifndef _MODULE_RENDER_H
#define _MODULE_RENDER_H

#include "Module.h"
#include "Globals.h"
#include <list>
#include <vector>

#define MAX_ALPHA_MESHES 512

class GameObject;
class ComponentCamera;

class ModuleRender : public Module
{
	friend class ModuleScene;

public:

	bool Init() override;
	update_status PreUpdate() override;
	update_status Update() override;
	update_status PostUpdate() override;
	bool CleanUp() override;

	void Render(const ComponentCamera* camera);
	
	void insertRenderizable(GameObject* go);
	void removeRenderizable(GameObject* go);

//members

	void* context; // Opaque SDL typedef void* openGL handler. 

	bool frustumCulling = true;

	bool fog = true;
	float fogColor[3] = { 1.f, 1.f, 1.f };
	float fogFalloff = 5000.f;
	float fogQuadratic = 1000.f;
	unsigned activeLights = 0u;


private:
	std::list<GameObject*> renderizables;  //The owner of the component should clean this
	std::vector<GameObject*> alphaRenderizables; //store meshes with alpha to render in order to get the blend working 
	unsigned alphaAmount = 0u;
};

#endif