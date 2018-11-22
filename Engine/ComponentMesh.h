#ifndef _COMPONENT_MESH_H_
#define _COMPONENT_MESH_H_

#include <vector>
#include "MathGeoLib/include/Math/float3.h"
#include "glew-2.1.0/include/GL/glew.h"
#include "Component.h"

class GameObject;
class ComponentMaterial;
class ComponentCamera;

struct par_shapes_mesh_s;

class ComponentMesh : public Component
{
public:

	ComponentMesh() : Component("Mesh") {}
	ComponentMesh(par_shapes_mesh_s* mesh);

	~ComponentMesh() {
		if (VAO != 0)
			glDeleteVertexArrays(1, &VAO);
	}

	void EditorDraw() override;

	void Render(const ComponentCamera* camera, const unsigned program, float r, float g, float b) const;

//members

	unsigned VAO = 0;
	unsigned VIndex = 0;
	unsigned nVertices = 0;
	unsigned nIndices = 0;	
	std::vector<float3> vertices;
	std::vector<unsigned> indices;

	ComponentMaterial* material;
	GameObject* owner = nullptr;

};

#endif