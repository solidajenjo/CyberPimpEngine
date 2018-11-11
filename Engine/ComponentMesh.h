#ifndef _COMPONENT_MESH_H_
#define _COMPONENT_MESH_H_

#include <vector>
#include "MathGeoLib/include/Math/float3.h"
#include "glew-2.1.0/include/GL/glew.h"

#include "Component.h"

class GameObject;

class ComponentMesh : public Component
{
public:

	~ComponentMesh() {
		if (VAO != 0)
			glDeleteVertexArrays(1, &VAO);
	}
//members

	unsigned VAO = 0;
	unsigned VIndex = 0;
	unsigned nVertices = 0;
	unsigned nIndices = 0;
	unsigned texture = 0;
	std::vector<float3> vertices;

	GameObject* owner = nullptr;

};

#endif