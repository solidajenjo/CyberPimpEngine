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
	for (std::vector<GameObject*>::reverse_iterator it = sceneGameObjects.rbegin(); it != sceneGameObjects.rend(); ++it) //must be cleaned backwards because dependencies
	{
		RELEASE(*it);
	}
	sceneGameObjects.resize(0);
	LOG("Cleaning scene GameObjects. Done");
	return true;
}
