#ifndef _ENTITY_MESH_H
#define _ENTITY_MESH_H

#include "Entity.h"
#include <vector>

class Transform;

typedef struct
{
	unsigned VAO;
	unsigned VIO;
	unsigned nIndices;
	unsigned texture;
}Mesh;

class EntityMesh : public Entity
{

public:
	EntityMesh(const char* name) : Entity(name) {};
	~EntityMesh() {};

	bool Render() const;

//members

	int t0 = -1;
	Mesh mesh;
	
};

#endif 
