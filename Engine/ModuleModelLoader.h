#ifndef _MODULE_MODEL_LOADER_H
#define _MODULE_MODEL_LOADER_H

#include "Module.h"
#include <vector>
#include <string>

struct aiScene;
struct aiNode;
struct aiMaterial;

class ComponentMesh;
class GameObject;

class ModuleModelLoader :
	public Module
{
public:

	void Load(std::string geometryPath); //TODO: store root gameobject on scene module

private:

	GameObject* GenerateMeshData(aiNode* node, const aiScene* scene, unsigned texture);
	unsigned GenerateMaterialData(aiMaterial* materials);

};

#endif