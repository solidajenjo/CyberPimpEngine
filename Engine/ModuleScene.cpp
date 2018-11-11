#include "ModuleScene.h"
#include "GameObject.h"


bool ModuleScene::Init()
{
	LOG("Init Scene module");
	return true;
}

update_status ModuleScene::Update()
{
	return UPDATE_CONTINUE;
}

bool ModuleScene::CleanUp()
{
	LOG("Cleaning scene GameObjects.");
	for (std::vector<GameObject*>::iterator it = sceneGameObjects.begin(); it != sceneGameObjects.end(); ++it)
	{
		RELEASE(*it);
	}
	sceneGameObjects.resize(0);
	return true;
}
