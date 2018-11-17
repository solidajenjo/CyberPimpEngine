#ifndef _COMPONENT_MESH_H_
#define _COMPONENT_MESH_H_

#include <vector>
#include "MathGeoLib/include/Math/float3.h"
#include "glew-2.1.0/include/GL/glew.h"

#include "Component.h"

class GameObject;
class ComponentMaterial;

class ComponentMesh : public Component
{
public:

	ComponentMesh() : Component("Mesh") {}
	~ComponentMesh() {
		if (VAO != 0)
			glDeleteVertexArrays(1, &VAO);
	}

	void EditorDraw() override;

//members

	unsigned VAO = 0;
	unsigned VIndex = 0;
	unsigned nVertices = 0;
	unsigned nIndices = 0;	
	std::vector<float3> vertices;

	ComponentMaterial* material;
	GameObject* owner = nullptr;

};

#endif