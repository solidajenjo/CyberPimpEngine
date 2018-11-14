#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#include "Globals.h"
#include <vector>
#include <string>
#include "Transform.h"
#include "Component.h"
#include "MathGeoLib/include/Geometry/AABB.h"

class GameObject
{
public:

	GameObject()
	{
		transform = new Transform(this); //notice transform who owns it
	}

	~GameObject()
	{
		RELEASE(transform);
		RELEASE(aaBB);

		for (std::vector<Component*>::iterator it = components.begin(); it != components.end(); ++it)
		{
			RELEASE(*it);
		}
	}
//members
	
	Transform* transform = nullptr;

	GameObject* parent = nullptr; //Released by scene module

	std::vector<GameObject*> children; //Released by scene module (each one)
	std::vector<Component*> components;

	AABB* aaBB = nullptr;
	bool enabled = true, selected = false;
	std::string name = "";
};

#endif