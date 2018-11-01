#ifndef _MODULE_EDITOR_H
#define _MODULE_EDITOR_H

#include "Module.h"

class SubModuleEditorMenu;

class ModuleEditor :
	public Module
{
public:
	ModuleEditor();
	~ModuleEditor();
	
	bool Init();

	update_status PreUpdate();

	update_status Update();

	update_status PostUpdate();

	bool CleanUp();

//members

	SubModuleEditorMenu* menu;

	bool p_open = true;
};

#endif