#ifndef _COMPONENT_MESH_H_
#define _COMPONENT_MESH_H_

#include <vector>
#include "MathGeoLib/include/Math/float3.h"

#include "Component.h"

class GameObject;

class ComponentMesh : public Component
{
public:

//members

	unsigned VAO = 0;
	unsigned VIndex = 0;
	unsigned nVertices = 0;
	unsigned nIndices = 0;
	unsigned texture = 0;
	std::vector<float3> vertices;

	GameObject* owner = nullptr;

	//TODO:Release openGL memory on destroy
};

#endif