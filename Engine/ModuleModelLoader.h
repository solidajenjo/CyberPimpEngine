#ifndef _MODULE_MODEL_LOADER_H
#define _MODULE_MODEL_LOADER_H

#include "Module.h"
#include <vector>
#include <string>
#include "MathGeoLib/include/Math/float3.h"
#include "MathGeoLib/include/Geometry/AABB.h"

struct aiScene;
struct aiNode;
struct aiMaterial;

class ComponentMesh;
class ComponentMaterial;
class GameObject;

class ModuleModelLoader :
	public Module
{
public:

	void Load(const std::string& geometryPath);

private:

	GameObject* GenerateMeshData(aiNode* rootNode, const aiScene* scene, const std::vector<ComponentMaterial*>& materials);
	void GenerateMaterialData(const aiMaterial* material, std::vector<ComponentMaterial*>& materialContainer);

//memebers
	std::vector<float3> allCorners; //store all bounding boxes corners to form global bounding box
};

#endif