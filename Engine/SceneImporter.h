#ifndef _SCENE_IMPORTER_H
#define _SCENE_IMPORTER_H

#include <string>
#include <vector>
#include "Transform.h"

struct aiScene;
struct aiNode;

class GameObject;

class SceneImporter
{
public:

	bool Import(const std::string file) const;
	GameObject* Load(const std::string file) const;

private:
	inline void writeToBuffer(std::vector<char> &buffer, unsigned &pointer, const unsigned size, const void* data) const;
	inline void cleanTempData(GameObject** model);
};

#endif 