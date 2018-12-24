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
	ComponentMesh* LoadMesh(const char path[1024]) const;
	void SaveMesh(const ComponentMesh* mesh, const char path[1024]) const;
private:
	inline void writeToBuffer(std::vector<char> &buffer, unsigned &pointer, const unsigned size, const void* data) const;

};

#endif 