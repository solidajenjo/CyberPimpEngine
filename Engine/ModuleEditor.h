#ifndef _MODULE_EDITOR_H
#define _MODULE_EDITOR_H

#include "Module.h"
#include <vector>

class SubModuleEditor;
class SubModuleEditorMenu;
class SubModuleEditorConsole;
class SubModuleEditorWorldInspector;
class SubModuleEditorViewPort;

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

	std::vector<SubModuleEditor*> subModules;

	SubModuleEditorMenu* menu;
	SubModuleEditorConsole* console;
	SubModuleEditorWorldInspector* worldInspector;
	SubModuleEditorViewPort* viewPort;

	bool bDock = false;
};

#endif