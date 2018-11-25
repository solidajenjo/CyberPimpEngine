#ifndef _MODULE_RENDER_H
#define _MODULE_RENDER_H

#include "Module.h"
#include "Globals.h"
#include <list>

class ComponentMesh;
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

	void Render(ComponentCamera* camera) const;

	const std::list<ComponentMesh*>* getRenderizables() const; //read only getter
	void insertRenderizable(ComponentMesh* newMesh);
//members

	void* context; // Opaque SDL typedef void* openGL handler. 

	bool frustumCulling = true;

private:
	std::list<ComponentMesh*> renderizables;  //The owner of the component should clean this
};

#endif