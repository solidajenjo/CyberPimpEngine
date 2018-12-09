#ifndef _MATERIAL_IMPORTER_H
#define _MATERIAL_IMPORTER_H

#include <string>
#include <vector>

class ComponentMaterial;
class GameObject;

struct aiMaterial;

class MaterialImporter
{
public:

	std::string Import(const aiMaterial * material, const unsigned index, GameObject* &materialImported) const; //stores texture as DDS in engine folders return path of the material & a gameobject with the material by reference
	ComponentMaterial* Load(const char path[4096]) const;
	void Save(const char path[1024], const ComponentMaterial* material) const;
};

#endif 