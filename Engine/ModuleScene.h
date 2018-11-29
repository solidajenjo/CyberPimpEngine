#ifndef _MODULE_SCENE_H
#define _MODULE_SCENE_H

#include "Module.h"
#include <vector>

class GameObject;
class ComponentCamera;

class ModuleScene :
	public Module
{
public:
	
	bool Init() override;
	bool CleanUp() override; //clean when a new model is loaded & on exit

	void InsertGameObject(GameObject* newGO);
	void ImportGameObject(GameObject* newGO);
	void DestroyGameObject(GameObject* destroyableGO);
	void ShowHierarchy(bool isWorld = true); //editor drawing moved here to mantain controled & private the gameobjects on the scene. This avoids wrong loads & destroys
	void DrawNode(GameObject* gObj, bool isWorld = true);

	const std::vector<GameObject*>* getSceneGameObjects() const; //read only getter
	const std::vector<GameObject*>* getImportedGameObjects() const; //read only getter
	bool IsRoot(const GameObject* go) const;
	void SetSkyBox(); //sets skybox on framebuffer

	void Serialize();
	//members

	GameObject* selected = nullptr;
	ComponentCamera* sceneCamera = nullptr;

private:

	void FlattenHierarchy(GameObject* go);
	void FlattenImported(GameObject* go);

	//members

	std::vector<GameObject*> sceneGameObjects; //handles all game objects instantiated
	std::vector<GameObject*> importedGameObjects; //handles all game objects imported
	GameObject* root = nullptr; //scene root
	GameObject* directory = nullptr; //handles all imported files
};

#endif