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

class GameObject
{
public:

	GameObject(std::string name) : name(name)
	{
		transform = new Transform(this); //notice transform who owns it
		xg::Guid guid = xg::newGuid();
		std::string uuid = guid.str();
		sprintf_s(gameObjectUUID, uuid.c_str());		
	}

	GameObject(char UUID[40], Transform* transform) : transform(transform) 
	{
		sprintf_s(gameObjectUUID,UUID);
	};

	~GameObject()
	{
		RELEASE(transform);

		for (std::list<Component*>::iterator it = components.begin(); it != components.end(); ++it)
		{
			RELEASE(*it);
		}
	}

	void InsertComponent(Component* newComponent);
	void InsertChild(GameObject* child);

	void Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer);

	void Clone(const GameObject& clonedGO);

	//members
	
	Transform* transform = nullptr;

	GameObject* parent = nullptr; //Released by scene module

	std::list<GameObject*> children; //Released by scene module (each one)
	std::list<Component*> components;

	AABB* aaBB = nullptr;
	AABB aaBBGlobal = AABB();
	bool enabled = true, selected = false;
	std::string name = "";

	char gameObjectUUID[40] = ""; //unique game object id

};


#endif