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

private:
	std::list<const GameObject*> renderizables;  //The owner of the component should clean this
};

#endif