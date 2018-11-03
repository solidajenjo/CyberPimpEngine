#pragma once
#include "Module.h"
#include <vector>

struct aiMesh;
struct aiMaterial;

class EntityMesh;

class ModuleModelLoader :
	public Module
{
public:
	ModuleModelLoader();
	~ModuleModelLoader();

	std::vector<EntityMesh*> Load(char * geometryPath); //return Mesh references
	unsigned GenerateMeshData(aiMesh* meshes, unsigned &vio);
	unsigned GenerateMaterialData(aiMaterial* materials);
	std::vector<unsigned> meshes;
};

