#include "Entity.h"
#include "Transform.h"

Entity::Entity(const char * name) : entityName(name)
{
	transform = new Transform();
}

Entity::~Entity()
{
	delete transform;
}
