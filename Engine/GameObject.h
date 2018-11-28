#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#include "Globals.h"
#include <list>
#include <string>
#include "Transform.h"
#include "Component.h"
#include "MathGeoLib/include/Geometry/AABB.h"
#include "crossguid/include/crossguid/guid.hpp"

class GameObject
{
public:

	GameObject(std::string name) : name(name)
	{
		transform = new Transform(this); //notice transform who owns it
		xg::Guid guid = xg::newGuid();
		gameObjectUUID = guid.str();
	}

	GameObject(std::string gameObjectUUID, Transform* transform) : gameObjectUUID(gameObjectUUID), transform(transform) {};

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

	//members
	
	Transform* transform = nullptr;

	GameObject* parent = nullptr; //Released by scene module

	std::list<GameObject*> children; //Released by scene module (each one)
	std::list<Component*> components;

	AABB* aaBB = nullptr;
	AABB aaBBGlobal = AABB();
	bool enabled = true, selected = false;
	std::string name = "";

	std::string gameObjectUUID = ""; //unique game object id

};


#endif