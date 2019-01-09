#ifndef _FAKE_GAMEOBJECT_H_
#define _FAKE_GAMEOBJECT_H_

#include "GameObject.h"
#include "Component.h"

class FakeGameObject : public GameObject //no clean needed because is only a referencerer
{
public:

	FakeGameObject() : GameObject("") {};

	Component* component = nullptr; // Fake game object to hold a component
	GameObject* original = nullptr;
	
};


#endif