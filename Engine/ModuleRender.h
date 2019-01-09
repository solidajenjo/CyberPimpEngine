#ifndef _MODULE_RENDER_H
#define _MODULE_RENDER_H

#include "Module.h"
#include "Globals.h"
#include <list>

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

	void Render(const ComponentCamera* camera) const;
	
	void insertRenderizable(GameObject* go);
	void removeRenderizable(const GameObject* go);
//members

	void* context; // Opaque SDL typedef void* openGL handler. 

	bool frustumCulling = true;

	bool fog = true;
	float fogColor[3] = { 1.f, 1.f, 1.f };
	float fogFalloff = 5000.f;
	float fogQuadratic = 1000.f;

private:
	std::list<const GameObject*> renderizables;  //The owner of the component should clean this
};

#endif