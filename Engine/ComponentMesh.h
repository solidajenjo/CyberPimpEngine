#ifndef _COMPONENT_MESH_H_
#define _COMPONENT_MESH_H_

#include <vector>
#include <string>
#include <map>
#include "MathGeoLib/include/Math/float3.h"
#include "Component.h"

class ComponentMaterial;
class ComponentCamera;
class Transform;

struct par_shapes_mesh_s;

class ComponentMesh : public Component
{
public:

	enum class Primitives
	{
		VOID_PRIMITIVE = -1,
		CUBE = 0,
		SPHERE,
		TORUS,
		CYLINDER,
		PLANE
	};

	ComponentMesh() : Component(ComponentTypes::MESH_COMPONENT) {};
	ComponentMesh(Primitives primitive);
	ComponentMesh(const std::vector<float> &vertices, const std::vector<unsigned> &indices, const std::vector<float> &texCoords);

	~ComponentMesh();

	void FromPrimitive(Primitives primitive);
	void EditorDraw() override;

	void Render(const ComponentCamera* camera, Transform* transform) const;

	void Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer) override;
	void UnSerialize(rapidjson::Value &value) override;
	bool Release() override;
	void SendToGPU();
	void ReleaseFromGPU();

//members

	unsigned VAO = 0;
	unsigned VIndex = 0;
	unsigned nVertices = 0;
	unsigned nCoords = 0;
	unsigned nIndices = 0;	
	unsigned nNormals = 0;	
	std::vector<float3> meshVertices;
	std::vector<float3> meshNormals;
	std::vector<float> meshTexCoords;
	std::vector<unsigned> meshIndices;

	ComponentMaterial* material;
	char meshPath[1024] = "";
	Primitives primitiveType = Primitives::VOID_PRIMITIVE;

	//Resource management
	static ComponentMesh* GetMesh(std::string path); //check if a mesh is loaded and returns it, otherwise tries to load and return
	static std::map<std::string, ComponentMesh*> meshesLoaded;
};

#endif