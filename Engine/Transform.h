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
	float x = 1, y = 1, z = 1;
}Scale;

class Transform :
	public EntityAttachable
{
public:
	Transform();
	~Transform();

	float* getModelMatrix() const;

//members

	Position position;
	Rotation rotation;
	Scale scale;
};

#endif