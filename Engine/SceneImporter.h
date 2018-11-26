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

	struct Node
	{						
		unsigned nMeshes = 0;
		unsigned parent = 0;
		unsigned id = 0;		
		std::string name = "";
		Transform transform = Transform(nullptr);
		std::vector<std::vector<float>> vertices;
		std::vector<std::vector<float>> coords;
		std::vector<std::vector<unsigned>> indices;
	};

	bool Import(const std::string file) const;
	GameObject* Load(const std::string file) const;

private:
		void writeToBuffer(std::vector<char> &buffer, unsigned &pointer, const unsigned size, const void* data) const;
};

#endif 