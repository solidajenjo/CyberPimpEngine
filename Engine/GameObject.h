#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#include "Globals.h"
#include <list>
#include <string>
#include "Transform.h"
#include "Component.h"
#include "MathGeoLib/include/Geometry/AABB.h"
#include "crossguid/include/crossguid/guid.hpp"
#include "rapidjson-1.1.0/include/rapidjson/prettywriter.h"
#include "rapidjson-1.1.0/include/rapidjson/document.h"

class ComponentMesh;

class GameObject
{
public:


	GameObject(const char name[1024], bool isContainer = false);

	GameObject(char UUID[40], Transform* transform);

	~GameObject();
	
	void InsertComponent(Component* newComponent);
	void InsertChild(GameObject* child);
	void SetInstanceOf(char instanceOrigin[40]);

	void Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer);
	bool UnSerialize(rapidjson::Value &value);	

	void PropagateStaticCheck();
	
	
	GameObject* MakeInstanceOf() const;
	GameObject* Clone(bool breakInstance = false) const;

	//members
	
	Transform* transform = nullptr;

	GameObject* parent = nullptr; //Released by scene module

	std::list<GameObject*> children; //Released by scene module (each one)
	std::list<Component*> components;

	AABB* aaBB = nullptr;
	AABB* aaBBGlobal = nullptr;
	bool enabled = true, selected = false, isRenderizable = false;
	
	char name[1024] = "";
	char gameObjectUUID[40] = ""; //unique game object id
	char parentUUID[40] = ""; //unique parent's game object id
	char instanceOf[40] = ""; //identifier to search on assets hierarchy & clone
	char path[1024] = ""; //some type of gameobjects store a path to be loaded from disk
	bool isInstantiated = false;
	bool isStatic = false;
	bool isContainer = false;
	bool active = true;
};


#endif