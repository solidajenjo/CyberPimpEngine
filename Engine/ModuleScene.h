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
	update_status Update() override;
	bool CleanUp() override; //clean when a new model is loaded & on exit

	void InsertGameObject(GameObject* newGO); //Insert instantiated Game Object
	void ImportGameObject(GameObject* newGO); //Insert imported game Object
	void DestroyGameObject(GameObject* destroyableGO);
	void ShowHierarchy(bool isWorld = true); //editor drawing moved here to mantain controled & private the gameobjects on the scene. This avoids wrong loads & destroys
	void DrawNode(GameObject* gObj, bool isWorld = true);

	const std::vector<GameObject*>* getSceneGameObjects() const; //read only getter
	const std::vector<GameObject*>* getImportedGameObjects() const; //read only getter
	bool IsRoot(const GameObject* go) const;

	bool MakeParentInstantiated(char parentUUID[40], GameObject* son); //searchs for an instantiated gameobject by it's UUID and attaches son to it
	bool MakeParentImported(char parentUUID[40], GameObject* son); //searchs for an imported gameobject by it's UUID and attaches son to it
	GameObject* FindInstanceOrigin(char instance[40]);	

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