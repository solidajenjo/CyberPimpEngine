#ifndef _MODULE_SCENE_H
#define _MODULE_SCENE_H

#include "Module.h"
#include <vector>

class GameObject;

class ModuleScene :
	public Module
{
public:
	
	bool Init() override;
	update_status Update() override;
	bool CleanUp() override; //clean when a new model is loaded & on exit

	void insertGameObject(GameObject* newGO);
	void destroyGameObject(GameObject* newGO);
	void showHierarchy(); //editor drawing moved here to mantain controled & private the gameobjects on the scene. This avoids wrong loads & destroys
	void drawNode(GameObject* gObj);

	const std::vector<GameObject*>* getSceneGameObjects() const; //read only getter

private:

	void flattenHierarchy(GameObject* go);

	//members

	std::vector<GameObject*> sceneGameObjects; //handles all game objects
	std::vector<GameObject*> sceneGameObjectsHierarchy; // handles hierarchy stored game objects, no need to release needed because the data is the same as sceneGameObjects

	GameObject* selected;
};

#endif