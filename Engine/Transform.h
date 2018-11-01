#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "EntityAttachable.h"

typedef struct 
{
	float x = 0, y = 0, z = 0;
}Position;

typedef struct 
{
	float x = 0, y = 0, z = 0;
}Rotation;

typedef struct 
{
	float x = 0, y = 0, z = 0;
}Scale;

class Transform :
	public EntityAttachable
{
public:
	Transform();
	~Transform();

//members

	Position positon;
	Rotation rotation;
	Scale scale;
};

#endif