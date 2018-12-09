#ifndef _APPLICATION_H
#define _APPLICATION_H

#include<list>
#include "Globals.h"
#include "Module.h"

struct ImGuiTextBuffer;

class ModuleRender;
class ModuleWindow;
class ModuleTextures;
class ModuleInput;
class ModuleEditor;
class ModuleEditorCamera;
class ModuleProgram;
class ModuleTime;
class ModuleScene;
class ModuleFrameBuffer;
class ModuleDebugDraw;
class ModuleFileSystem;
class ModuleSpacePartitioning;

class Application
{
public:

	Application();
	~Application();

	bool Init();
	update_status Update();
	bool CleanUp();

public:
	ModuleRender* renderer = nullptr;
	ModuleWindow* window = nullptr;
	ModuleTextures* textures = nullptr;
	ModuleInput* input = nullptr;
	ModuleEditor* editor = nullptr;
	ModuleEditorCamera* camera = nullptr;
	ModuleProgram* program = nullptr;
	ModuleTime* appTime = nullptr;
	ModuleFrameBuffer* frameBuffer = nullptr;
	ModuleFrameBuffer* gameFrameBuffer = nullptr;
	ModuleScene* scene = nullptr;
	ModuleDebugDraw* debugDraw = nullptr;
	ModuleFileSystem* fileSystem = nullptr;
	ModuleSpacePartitioning* spacePartitioning = nullptr;

	bool exit = false;
	ImGuiTextBuffer* consoleBuffer = nullptr;

private:

	std::list<Module*> modules;

};

extern Application* App;

#endif
