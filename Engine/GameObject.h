#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#include "Globals.h"
#include <vector>
#include <string>
#include "Transform.h"
#include "Component.h"


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
		for (std::vector<GameObject*>::iterator it = children.begin(); it != children.end(); ++it)
		{
			RELEASE(*it);
		}
		for (std::vector<Component*>::iterator it = components.begin(); it != components.end(); ++it)
		{
			RELEASE(*it);
		}
	}
//members
	
	Transform* transform = nullptr;

	GameObject* parent = nullptr;
	std::vector<GameObject*> children; 
	std::vector<Component*> components;

	bool enabled = true;
	std::string name = "";
};

#endif