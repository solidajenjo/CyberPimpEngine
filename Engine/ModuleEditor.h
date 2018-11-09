#ifndef _MODULE_EDITOR_H
#define _MODULE_EDITOR_H

#include "Module.h"
#include <vector>

class SubModuleEditor;
class SubModuleEditorMenu;
class SubModuleEditorConsole;
class SubModuleEditorWorldInspector;
class SubModuleEditorViewPort;
class SubModuleEditorConfig;
class SubModuleEditorToolBar;
class ModuleTextures;

class ModuleEditor :
	public Module
{
public:

	bool Init() override;
	update_status PreUpdate() override;
	update_status Update() override;
	update_status PostUpdate() override;
	bool CleanUp() override;

//members

	std::vector<SubModuleEditor*> subModules;

	bool bDock = true;
	unsigned logo = 0;
	unsigned backgroundTex = 0;

	SubModuleEditorMenu* menu = nullptr;
	SubModuleEditorConsole* console = nullptr;
	SubModuleEditorWorldInspector* worldInspector = nullptr;
	SubModuleEditorViewPort* viewPort = nullptr;
	SubModuleEditorConfig* config = nullptr;
	SubModuleEditorToolBar* toolBar = nullptr;
	ModuleTextures* textures = nullptr; //independent of game textures - Handles editor textures
};

#endif