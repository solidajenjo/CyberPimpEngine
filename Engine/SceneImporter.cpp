#include "SceneImporter.h"
#include "Assimp/include/assimp/cimport.h"
#include "Assimp/include/assimp/postprocess.h"
#include "Assimp/include/assimp/scene.h"
#include "Assimp/include/assimp/material.h"
#include "Assimp/include/assimp/mesh.h"
#include "Application.h"
#include "SDL/include/SDL_rwops.h"
#include "MathGeoLib/include/Math/Quat.h"
#include "GameObject.h"
#include "ComponentMesh.h"
#include "ModuleRender.h"
#include <stack>

bool SceneImporter::Import(const std::string file) const
{
	LOG("Load model %s", file.c_str());
	const aiScene* scene = aiImportFile(file.c_str(), aiProcess_Triangulate);

	if (scene == nullptr) {
		LOG("Error importing model -> %s", aiGetErrorString());
		return false;
	}

	aiNode* rootNode = scene->mRootNode;
	assert(rootNode != nullptr);

	std::vector<char> bytes;
	bytes.resize(sizeof(unsigned)); //reserve space to store num nodes
	std::stack<aiNode*> stackNode;
	std::stack<unsigned> stackParent;
	
	unsigned id = 0u;
	unsigned numNodes = 0u;
	unsigned bytesPointer = sizeof(unsigned); //reserve to store the amount of nodes
	for (unsigned i = 0; i < rootNode->mNumChildren; ++i) //skip rootnode
	{
		stackNode.push(rootNode->mChildren[i]);
		stackParent.push(id);
	}

	while (!stackNode.empty())
	{
		//process model
		aiNode* node = stackNode.top(); stackNode.pop();
		++numNodes;
		unsigned parent = stackParent.top(); stackParent.pop();
		++id;
		for (unsigned i = 0; i < node->mNumChildren; ++i)
		{
			stackNode.push(node->mChildren[i]);
			stackParent.push(id);
		}

		unsigned nameSize = node->mName.length + 1; //include the \0
		++nameSize;
		while (nameSize % 4 != 0) //align to multiple of 4 to keep coherence
			++nameSize;

		writeToBuffer(bytes, bytesPointer, sizeof(unsigned), &nameSize);				
		
		unsigned nMeshes = node->mNumMeshes;
		
		writeToBuffer(bytes, bytesPointer, sizeof(unsigned), &nMeshes);
		writeToBuffer(bytes, bytesPointer, sizeof(unsigned), &parent);
		writeToBuffer(bytes, bytesPointer, sizeof(unsigned), &id);

		writeToBuffer(bytes, bytesPointer, sizeof(char) * nameSize, node->mName.C_Str());

		aiVector3D pos;
		aiVector3D scl;
		aiQuaternion rot;

		node->mTransformation.Decompose(scl, rot, pos);

		Quat q(rot.x, rot.y, rot.z, rot.w); //Quat to translate from black math voodoo to human understable
		Transform transform(nullptr); //temporal transform to store node transform
		transform.SetTransform(float3(pos.x, pos.y, pos.z), q.ToEulerXYZ(), float3(scl.x, scl.y, scl.z));

		writeToBuffer(bytes, bytesPointer, sizeof(transform), &transform);

		if (nMeshes >= 1) //is a mesh
		{		
			for (unsigned i = 0u; i < nMeshes; ++i)
			{				
				aiMesh* mesh = scene->mMeshes[*node->mMeshes];
				unsigned nVertices = mesh->mNumVertices;
				unsigned nIndices = mesh->mNumFaces * 3;
				unsigned nCoords = mesh->mNumVertices * 2;
				if (mesh->mTextureCoords[0] == nullptr)
					nCoords = 0;
				writeToBuffer(bytes, bytesPointer, sizeof(unsigned), &nVertices);
				writeToBuffer(bytes, bytesPointer, sizeof(unsigned), &nIndices);
				writeToBuffer(bytes, bytesPointer, sizeof(unsigned), &nCoords);
				writeToBuffer(bytes, bytesPointer, sizeof(float) * 3 * nVertices, &scene->mMeshes[*node->mMeshes]->mVertices[0]);

				std::vector<unsigned> indices = std::vector<unsigned>(nIndices);

				unsigned nFaces = mesh->mNumFaces;				

				for (unsigned j = 0u; j < nFaces; ++j)
				{
					aiFace face = mesh->mFaces[j];
					assert(mesh->mFaces[j].mNumIndices == 3);

					indices[j * 3] = face.mIndices[0];
					indices[(j * 3) + 1] = face.mIndices[1];
					indices[(j * 3) + 2] = face.mIndices[2];
				}				
				writeToBuffer(bytes, bytesPointer, sizeof(unsigned) * indices.size(), &indices[0]);				
				std::vector<float> coords = std::vector<float>(nCoords);
				coords.resize(nCoords);
				for (unsigned j = 0; j < nVertices && nCoords > 0; ++j)
				{
					coords[j * 2] = mesh->mTextureCoords[0][j].x;
					coords[(j * 2) + 1] = mesh->mTextureCoords[0][j].y;
				}
				if (nCoords > 0)
					writeToBuffer(bytes, bytesPointer, sizeof(float) * coords.size(), &coords[0]);
			}
		}
	}

	bytesPointer = 0;
	writeToBuffer(bytes, bytesPointer, sizeof(unsigned), &numNodes); //store the amount of nodes of the model
	
	unsigned ext = file.find("FBX") - 1;
	unsigned nameBegin = ext;
	while (file[nameBegin] != '\\' && file[nameBegin] != '/' && nameBegin > 0)
	{
		--nameBegin;
	}
	std::string importedFileName = "Library/Meshes/" + file.substr(nameBegin, ext) + ".dmd";
	SDL_RWops *rw = SDL_RWFromFile(importedFileName.c_str(), "w");	
	if (rw == nullptr || SDL_RWwrite(rw, &bytes[0], sizeof(char), bytes.size()) != bytes.size())
	{
		LOG("Failed importing %s -> %s", file.c_str(), SDL_GetError());
		return false;
	}
	LOG("Succesfully imported %s", file.c_str());
	SDL_RWclose(rw);
	
	return true;
}

GameObject* SceneImporter::Load(const std::string file) const
{
	std::string path = "Library/Meshes/" + file;
	SDL_RWops *rw = SDL_RWFromFile(path.c_str(), "r");
	if (rw == nullptr)
	{
		LOG("Couldn't load %s -> %s", file.c_str(), SDL_GetError());
		return nullptr;
	}
	unsigned numNodes;
	if (SDL_RWread(rw, &numNodes, sizeof(unsigned), 1) != 1) 
	{
		SDL_RWclose(rw);
		return nullptr;
	}

	LOG("Loading %s -> Num nodes %d", file.c_str(), numNodes);
	std::vector<Node*> model;
	model.resize(numNodes + 1); //0 is root
	
	for (unsigned i = 0u; i < numNodes; ++i)
	{
		Node* node = new Node();
		unsigned nodeHeader[4]; //nameSize - nMeshes - parent - id
		if (SDL_RWread(rw, &nodeHeader, sizeof(unsigned), 4) != 4)
		{
			LOG("Couldn't load %s -> %s", file.c_str(), SDL_GetError());
			SDL_RWclose(rw);
			return nullptr;
		}

		char buffer[4096];

		if (SDL_RWread(rw, &buffer, sizeof(char), nodeHeader[0]) != nodeHeader[0])
		{
			LOG("Couldn't load %s -> %s", file.c_str(), SDL_GetError());
			SDL_RWclose(rw);
			return nullptr;
		}
		node->name = std::string(buffer);
		node->nMeshes = nodeHeader[1];
		node->parent = nodeHeader[2];
		node->id = nodeHeader[3];

		if (SDL_RWread(rw, &node->transform, sizeof(Transform), 1) != 1)
		{
			LOG("Couldn't load %s -> %s", file.c_str(), SDL_GetError());
			SDL_RWclose(rw);
			return nullptr;
		}

		LOG("Node %s Parent: %d Id: %d Meshes: %d", node->name.c_str(), node->parent, node->id, node->nMeshes);
		node->vertices.resize(node->nMeshes);
		node->indices.resize(node->nMeshes);
		node->coords.resize(node->nMeshes);

		for (unsigned j = 0u; j < node->nMeshes; ++j) 
		{
			unsigned nElements[3]; //nVertices - nIndices - nCoords
			
			if (SDL_RWread(rw, &nElements, sizeof(unsigned), 3) != 3)
			{
				LOG("Couldn't load %s -> %s", file.c_str(), SDL_GetError());
				SDL_RWclose(rw);
				return nullptr;
			}
			
			node->vertices[j].resize(nElements[0] * 3);
			node->indices[j].resize(nElements[1]);
			node->coords[j].resize(nElements[2]);
			
			if (nElements[0] > 0 && SDL_RWread(rw, &node->vertices[j][0], sizeof(float), nElements[0] * 3) != nElements[0] * 3)
			{
				LOG("Couldn't load %s -> %s", file.c_str(), SDL_GetError());
				SDL_RWclose(rw);
				return nullptr;
			}
			if (nElements[1] > 0 && SDL_RWread(rw, &node->indices[j][0], sizeof(unsigned), nElements[1]) != nElements[1])
			{
				LOG("Couldn't load %s -> %s", file.c_str(), SDL_GetError());
				SDL_RWclose(rw);
				return nullptr;
			}
			if (nElements[2] > 0 && SDL_RWread(rw, &node->coords[j][0], sizeof(float), nElements[2]) != nElements[2])
			{
				LOG("Couldn't load %s -> %s", file.c_str(), SDL_GetError());
				SDL_RWclose(rw);
				return nullptr;
			}
		}
		model[nodeHeader[3]] = node;
	}
	SDL_RWclose(rw);

	std::vector<GameObject*> gameObjects(numNodes + 1, new GameObject("")); //build gameObjects

	for (unsigned i = 1u; i < model.size(); ++i)
	{
		Node* node = model[i];
		GameObject* newGO = new GameObject(node->name);
		newGO->transform = new Transform(node->transform);
		newGO->transform->owner = newGO;
		newGO->transform->RecalcModelMatrix();
		newGO->parent = gameObjects[node->parent];
		for (unsigned j = 0u; j < node->nMeshes; ++j)
		{
			ComponentMesh* newMesh = new ComponentMesh(node->vertices[j], node->indices[j], node->coords[j]);
			newGO->InsertComponent(newMesh);
			newGO->aaBB.SetNegativeInfinity();
			newGO->aaBB.SetFrom(&newMesh->meshVertices[0], newMesh->nVertices);
			newMesh->SendToGPU();
			App->renderer->insertRenderizable(newMesh);
			gameObjects[node->parent]->InsertChild(newGO);
			newGO->transform->PropagateTransform();
			gameObjects[node->id] = newGO;
		}

		//RELEASE(node); TODO:CLEAN HERE!!
	}		
	gameObjects[0]->name = "Root";
	return gameObjects[0];
}

void SceneImporter::writeToBuffer(std::vector<char> &buffer, unsigned & pointer, const unsigned size, const void * data) const
{
	buffer.resize(buffer.size() + size);
	memcpy(&buffer[pointer], data, size);
	pointer += size;
}
