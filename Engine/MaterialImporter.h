#ifndef _MATERIAL_IMPORTER_H
#define _MATERIAL_IMPORTER_H

#include <string>

class GameObject;
class ComponentMaterial;

struct aiMaterial;

class MaterialImporter
{
public:

	std::string Import(const aiMaterial * material, const unsigned index); //stores as DDS in engine folders
	ComponentMaterial* Load(char path[4096]);
	void Save(char path[1024], ComponentMaterial* material);
};

#endif 