#pragma once
#include "EntityAttachable.h"

struct Position
{
	float x = 0, y = 0, z = 0;
};

struct Rotation
{
	float x = 0, y = 0, z = 0;
};

struct Scale
{
	float x = 0, y = 0, z = 0;
};

class Transform :
	public EntityAttachable
{
public:
	Transform();
	~Transform();

//Members
	Position postion;
	Rotation rotation;
	Scale scale;
};

