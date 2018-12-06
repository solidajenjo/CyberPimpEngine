#ifndef _SCENE_IMPORTER_H
#define _SCENE_IMPORTER_H

#include <string>
#include <vector>
#include <map>


struct aiScene;
struct aiNode;

class GameObject;
class ComponentMesh;
class ComponentMaterial;

class SceneImporter
{
public:

	bool Import(const std::string &file) const;
	void Load(const std::string &file) const; //Loads the model & all his materials.

private:
	inline void writeToBuffer(std::vector<char> &buffer, unsigned &pointer, const unsigned size, const void* data) const;
	ComponentMesh* LoadMesh(const char path[1024], std::map<std::string, ComponentMaterial*> &materials) const;
};

#endif 