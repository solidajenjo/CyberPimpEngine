#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#include "Globals.h"
#include <list>
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
		id = generateID();
	}

	~GameObject()
	{
		RELEASE(transform);
		RELEASE(aaBB);

		for (std::list<Component*>::iterator it = components.begin(); it != components.end(); ++it)
		{
			RELEASE(*it);
		}
	}

	//members
	
	Transform* transform = nullptr;

	GameObject* parent = nullptr; //Released by scene module

	std::list<GameObject*> children; //Released by scene module (each one)
	std::list<Component*> components;

	AABB* aaBB = nullptr;
	AABB* aaBBGlobal = nullptr;
	bool enabled = true, selected = false;
	std::string name = "";

	unsigned id = 0u; //unique game object id

private:

	unsigned generateID()
	{
		static unsigned s_id = 0;
		return ++s_id;
	}

};


#endif