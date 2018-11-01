#ifndef _ENTITY_H
#define _ENTITY_H

class Transform;

class Entity
{
	/*
		World instantiable object
	*/
public:
	Entity();
	~Entity();

//members

	Transform* transform;
};

#endif 
