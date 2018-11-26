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

	enum class Primitives
	{
		CUBE,
		SPHERE,
		TORUS,
		CYLINDER,
		PLANE
	};

	ComponentMesh() : Component("Mesh") {}
	ComponentMesh(Primitives primitive);
	ComponentMesh(const std::vector<float> &vertices, const std::vector<unsigned> &indices, const std::vector<float> &texCoords);

	~ComponentMesh() {
		if (VAO != 0)
			glDeleteVertexArrays(1, &VAO);
	}

	void EditorDraw() override;

	void Render(const ComponentCamera* camera) const;

	void SendToGPU();
	void ReleaseFromGPU();

//members

	unsigned VAO = 0;
	unsigned VIndex = 0;
	unsigned nVertices = 0;
	unsigned nCoords = 0;
	unsigned nIndices = 0;	
	std::vector<float3> meshVertices;
	std::vector<float3> meshNormals;
	std::vector<float> meshTexCoords;
	std::vector<unsigned> meshIndices;

	ComponentMaterial* material;
	bool showNormals = false;
	float normalLength = 2.f;
	
};

#endif