#ifndef _ENTITY_H
#define _ENTITY_H

class Transform;

class Entity
{
	/*
		World instantiable object
	*/
public:
	Entity(char* name) : name(name) {};
	~Entity() {};

	bool Render();
//members
	char* name;
	Transform* transform;
};

#endif 
