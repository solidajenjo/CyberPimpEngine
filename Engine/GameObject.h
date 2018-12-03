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

	GameObject(const char name[40]) 
	{
		transform = new Transform(this); //notice transform who owns it
		xg::Guid guid = xg::newGuid();
		std::string uuid = guid.str();
		sprintf_s(gameObjectUUID, uuid.c_str());		
		sprintf_s(this->name, name);
	}

	GameObject(char UUID[40], Transform* transform) : transform(transform) 
	{
		sprintf_s(gameObjectUUID,UUID);
	};

	~GameObject()
	{
		RELEASE(transform); //TODO: Release bug on exit

		for (std::list<Component*>::iterator it = components.begin(); it != components.end(); ++it)
		{
			RELEASE(*it);
		}
	}

	void InsertComponent(Component* newComponent);
	void InsertChild(GameObject* child);
	void SetInstanceOf(char instanceOrigin[40]);

	void Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer);
	bool UnSerialize(rapidjson::Value &value, bool isInstantiated);	
	
	GameObject* MakeInstanceOf() const;
	ComponentMesh* GetMeshInstanceOrigin(char meshUUID[40]) const;

	//members
	
	Transform* transform = nullptr;

	GameObject* parent = nullptr; //Released by scene module

	std::list<GameObject*> children; //Released by scene module (each one)
	std::list<Component*> components;

	AABB* aaBB = nullptr;
	AABB aaBBGlobal = AABB();
	bool enabled = true, selected = false;
	
	char name[40] = "";
	char gameObjectUUID[40] = ""; //unique game object id
	char meshRoot[80] = ""; //special Identifier for mesh hierarchies root
	char instanceOf[40] = ""; //identifier to search on assets hierarchy & clone
	char path[1024] = ""; //some type of gameobjects store a path to be loaded from disk

	Component::ComponentTypes containerType = Component::ComponentTypes::VOID_COMPONENT; // to declare special gameobjects holding one component only
};


#endif