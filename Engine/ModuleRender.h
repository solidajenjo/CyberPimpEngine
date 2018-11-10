#ifndef _MODULE_RENDER_H
#define _MODULE_RENDER_H

#include "Module.h"
#include "Globals.h"
#include <vector>

class ComponentMesh;

class ModuleRender : public Module
{
public:

	bool Init() override;
	update_status PreUpdate() override;
	update_status Update() override;
	update_status PostUpdate() override;
	bool CleanUp() override;

	void Render() const;
//members

	void* context; // Opaque SDL typedef void* openGL handler. 
	std::vector<ComponentMesh*> renderizables;  //The owner of the component should clean this
};

#endif