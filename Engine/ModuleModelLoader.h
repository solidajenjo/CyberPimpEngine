#pragma once
#include "Module.h"
#include <vector>
#include <string>

struct aiMesh;
struct aiMaterial;

class EntityMesh;

class ModuleModelLoader :
	public Module
{
public:

	bool CleanUp() override;

	void Load(std::string geometryPath /*mesh hierarchy reference*/); //TODO: return Mesh references
	unsigned GenerateMeshData(aiMesh* meshes, unsigned &vio);
	unsigned GenerateMaterialData(aiMaterial* materials);

//members

	std::vector<unsigned> meshes; //default ctor init
};

