#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "ModuleTextures.h"
#include "ModuleInput.h"
#include "ModuleEditor.h"
#include "ModuleEditorCamera.h"
#include "ModuleProgram.h"
#include "ModuleFrameBuffer.h"
#include "ModuleTime.h"
#include "ModuleScene.h"
#include "ModuleDebugDraw.h"
#include "ModuleFileSystem.h"
#include "Module.h"
#include "Brofiler/ProfilerCore/Brofiler.h"
using namespace std;

Application::Application()
{
	// Order matters: they will Init/start/update in this order
	
	modules.push_back(window = new ModuleWindow());		
	modules.push_back(camera = new ModuleEditorCamera());			
	modules.push_back(textures = new ModuleTextures());
	modules.push_back(input = new ModuleInput());
	modules.push_back(appTime = new ModuleTime());
	modules.push_back(frameBuffer = new ModuleFrameBuffer());				
	modules.push_back(gameFrameBuffer = new ModuleFrameBuffer());				
	modules.push_back(editor = new ModuleEditor());
	modules.push_back(renderer = new ModuleRender());	
	modules.push_back(scene = new ModuleScene());
	modules.push_back(program = new ModuleProgram());	
	modules.push_back(debugDraw = new ModuleDebugDraw());
	modules.push_back(fileSystem = new ModuleFileSystem());
}

Application::~Application()
{
	for(list<Module*>::iterator it = modules.begin(); it != modules.end(); ++it)
    {
        delete *it;
    }
}

bool Application::Init()
{
	bool ret = true;

	for(list<Module*>::iterator it = modules.begin(); it != modules.end() && ret; ++it)
		ret = (*it)->Init();
	
	return ret;
}

update_status Application::Update()
{
	BROFILER_FRAME("Main Loop");
	update_status ret = UPDATE_CONTINUE;

	for(list<Module*>::iterator it = modules.begin(); it != modules.end() && ret == UPDATE_CONTINUE; ++it)
		ret = (*it)->PreUpdate();

	for(list<Module*>::iterator it = modules.begin(); it != modules.end() && ret == UPDATE_CONTINUE; ++it)
		ret = (*it)->Update();

	for(list<Module*>::iterator it = modules.begin(); it != modules.end() && ret == UPDATE_CONTINUE; ++it)
		ret = (*it)->PostUpdate();

	if (ret == UPDATE_ERROR || ret == UPDATE_STOP)
	{
		RELEASE(consoleBuffer); //Deactivate imgui console buffer
		consoleBuffer = nullptr; //Notice deactivation to log
	}
	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;

	for (list<Module*>::reverse_iterator it = modules.rbegin(); it != modules.rend() && ret; ++it)
	{
		ret = (*it)->CleanUp();
	}
	RELEASE(consoleBuffer);
	return ret;
}

