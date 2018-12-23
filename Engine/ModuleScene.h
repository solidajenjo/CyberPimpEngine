#ifndef _MODULE_SCENE_H
#define _MODULE_SCENE_H

#include "Module.h"
#include <map>
#include <string>
#include <vector>
#include <MathGeoLib/include/Geometry/AABB.h>

class GameObject;
class ComponentCamera;

class ModuleScene :	public Module
{
	
	friend class ComponentMaterial;

public:
	
	enum class ImportedType
	{
		MODEL,
		MATERIAL,
		MAP
	};

	bool Init() override;
	update_status Update() override;
	bool CleanUp() override; //clean when a new model is loaded & on exit

	void InsertGameObject(GameObject* newGO); //Insert instantiated Game Object
	void ImportGameObject(GameObject* newGO, ImportedType type); //Insert imported game Object
	void DestroyGameObject(GameObject* destroyableGO);
	void ShowHierarchy(bool isWorld = true); //editor drawing moved here to mantain controled & private the gameobjects on the scene. This avoids wrong loads & destroys
	void DrawNode(GameObject* gObj, bool isWorld = true);

	bool IsRoot(const GameObject* go) const;
	void AttachToRoot(GameObject* go);
	void AttachToAssets(GameObject* go);
	void AttachToMaps(GameObject* go);
	void AttachToModels(GameObject* go);
	void AttachToMaterials(GameObject* go);

	void DeleteGameObject(GameObject* go, bool isAsset = false);
	bool MakeParent(const std::string &parentUUID, GameObject* son); //searchs for a gameobject by it's UUID and attaches son to it
	
	GameObject* FindInstanceOrigin(const std::string &instance);	

	void SetSkyBox(); //sets skybox on framebuffer

	void GetStaticGlobalAABB(AABB* globalAABB, std::vector<GameObject*> &staticGOs) const; //get the global enclosing BB of the instantiated static gameobjects & the static gameObjects by reference

	void Serialize();
	void LinkGameObjects();

	//members

	GameObject* selected = nullptr;
	ComponentCamera* sceneCamera = nullptr;

private:

	void FlattenHierarchyOnImport(GameObject* go);

	//members

	std::map<std::string, GameObject*> sceneGameObjects; //handles all scene game objects
	
	GameObject* root = nullptr; //scene root
	GameObject* directory = nullptr; //handles all imported files
	GameObject* mapFolder = nullptr; //cleaned on directory destroy
	GameObject* modelFolder = nullptr; //cleaned on directory destroy
	GameObject* materialFolder = nullptr; //cleaned on directory destroy
};

#endif