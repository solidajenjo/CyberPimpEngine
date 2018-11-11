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
class ModuleModelLoader;
class ModuleEditor;
class ModuleCamera;
class ModuleProgram;
class ModuleTime;
class ModuleScene;
class ModuleFrameBuffer;

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
	ModuleModelLoader* modelLoader = nullptr;
	ModuleEditor* editor = nullptr;
	ModuleCamera* camera = nullptr;
	ModuleProgram* program = nullptr;
	ModuleTime* appTime = nullptr;
	ModuleFrameBuffer* frameBuffer = nullptr;
	ModuleScene* scene = nullptr;

	bool exit = false;
	ImGuiTextBuffer* consoleBuffer = nullptr;

private:

	std::list<Module*> modules;

};

extern Application* App;

#endif
