#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#include "Globals.h"
#include <vector>
#include <string>
#include "Transform.h"

class Component;
class Transform;

class GameObject
{
public:

	GameObject()
	{
		transform = new Transform(this);
	}

	~GameObject()
	{
		RELEASE(transform);
	}
//members
	
	Transform* transform = nullptr;

	GameObject* parent;
	std::vector<GameObject*> children;  //TODO: Clean
	std::vector<Component*> components; //TODO: Clean

	bool enabled = true;
	std::string name = "";
};

#endif