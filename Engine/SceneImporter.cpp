#define CLEAN_ON_FAILED(numNodes) {\
									LOG("Couldn't load %s -> %s", file.c_str(), SDL_GetError()); \
									SDL_RWclose(rw); \
									for (unsigned k = 0u; k < numNodes; ++k) RELEASE(model[k]); \
									delete[] model; \
									return nullptr; }

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
#include "ComponentMaterial.h"
#include "ModuleScene.h"
#include "ModuleRender.h"
#include "crossguid/include/crossguid/guid.hpp"
#include <stack>

bool SceneImporter::Import(const std::string file) const
{
	LOG("Import model %s", file.c_str());
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
		char nodeUUID[40];
		xg::Guid guid = xg::newGuid();
		sprintf_s(nodeUUID, guid.str().c_str());
		writeToBuffer(bytes, bytesPointer, sizeof(nodeUUID), &nodeUUID[0]);  //unique identifier to reference it from other gameobjects
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
				xg::Guid guid = xg::newGuid();
				char meshUUID[40];
				sprintf_s(meshUUID, guid.str().c_str());
				writeToBuffer(bytes, bytesPointer, sizeof(meshUUID), &meshUUID[0]);
				aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
				unsigned nVertices = mesh->mNumVertices;

				unsigned nIndices = 0;
				if (mesh->HasFaces())
					nIndices = mesh->mNumFaces * 3;
				
				unsigned nCoords = 0;
				if (mesh->HasTextureCoords(0))
					nCoords = mesh->mNumVertices * 2;
				
				unsigned nNormals = 0;
				if (mesh->HasNormals())
					nNormals = mesh->mNumVertices;
				
				writeToBuffer(bytes, bytesPointer, sizeof(unsigned), &nVertices);
				writeToBuffer(bytes, bytesPointer, sizeof(unsigned), &nIndices);
				writeToBuffer(bytes, bytesPointer, sizeof(unsigned), &nCoords);
				writeToBuffer(bytes, bytesPointer, sizeof(unsigned), &nNormals);

				writeToBuffer(bytes, bytesPointer, sizeof(float) * 3 * nVertices, &mesh->mVertices[0]);

				if (nIndices > 0)
				{
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
				}

				if (nCoords > 0)
				{
					std::vector<float> coords = std::vector<float>(nCoords);
					coords.resize(nCoords);
					for (unsigned j = 0; j < nVertices && nCoords > 0; ++j)
					{
						coords[j * 2] = mesh->mTextureCoords[0][j].x;
						coords[(j * 2) + 1] = mesh->mTextureCoords[0][j].y;
					}
					writeToBuffer(bytes, bytesPointer, sizeof(float) * coords.size(), &coords[0]);
				}

				if (nNormals > 0)
				{
					writeToBuffer(bytes, bytesPointer, sizeof(float) * 3 * nNormals, &mesh->mNormals[0]);
				}
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
	std::string importedFileName = "Library\\Meshes" + file.substr(nameBegin, ext) + ".dmd";
	SDL_RWops *rw = SDL_RWFromFile(importedFileName.c_str(), "w");	
	if (rw == nullptr || SDL_RWwrite(rw, &bytes[0], sizeof(char), bytes.size()) != bytes.size())
	{
		LOG("Failed importing %s -> %s", file.c_str(), SDL_GetError());
		return false;
	}
	LOG("Succesfully imported %s", file.c_str());
	SDL_RWclose(rw);
	Load(file.substr(nameBegin, ext)+".dmd");
	return true;
}

GameObject* SceneImporter::Load(const std::string file) const
{
	std::string path = "Library\\Meshes" + file;
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
	GameObject** model = new GameObject*[numNodes + 1];

	for (unsigned i = 0u; i < numNodes + 1; ++i)
		model[i] = new GameObject("Root"); //only the real root will keep this name

	for (unsigned i = 0u; i < numNodes; ++i)
	{		
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
		unsigned nMeshes = nodeHeader[1];
		unsigned parent = nodeHeader[2];
		unsigned id = nodeHeader[3];
		sprintf_s(model[id]->name, buffer);
		
		char nodeUUID[40];
		if (SDL_RWread(rw, nodeUUID, sizeof(nodeUUID), 1) != 1)
			CLEAN_ON_FAILED(numNodes + 1);
		sprintf_s(model[id]->gameObjectUUID, nodeUUID);

		if (parent <= numNodes - 1)
		{
			model[id]->parent = model[parent];
			model[parent]->InsertChild(model[id]);
		}
		if (SDL_RWread(rw, model[id]->transform, sizeof(Transform), 1) != 1)
			CLEAN_ON_FAILED(numNodes + 1);

		model[id]->transform->owner = model[id];
		model[id]->transform->RecalcModelMatrix();

		for (unsigned j = 0u; j < nMeshes; ++j) 
		{
			char meshUUID[40];
			if (SDL_RWread(rw, &meshUUID, sizeof(meshUUID), 1) != 1)
				CLEAN_ON_FAILED(numNodes + 1);

			unsigned nElements[4]; //nVertices - nIndices - nCoords - nNormals
			
			if (SDL_RWread(rw, &nElements, sizeof(unsigned), 4) != 4)
				CLEAN_ON_FAILED(numNodes + 1);

			ComponentMesh* newMesh = new ComponentMesh();
			sprintf_s(newMesh->meshUUID, meshUUID);
			newMesh->meshVertices.resize(nElements[0] * 3);
			newMesh->nVertices = nElements[0];
			newMesh->meshIndices.resize(nElements[1]);
			newMesh->nIndices = nElements[1];
			newMesh->meshTexCoords.resize(nElements[2]);
			newMesh->nCoords = nElements[2];
			newMesh->meshNormals.resize(nElements[3]);
			newMesh->nNormals = nElements[3];

			if (nElements[0] > 0 && SDL_RWread(rw, &newMesh->meshVertices[0], sizeof(float), nElements[0] * 3) != nElements[0] * 3)
				CLEAN_ON_FAILED(numNodes + 1);
			if (nElements[1] > 0 && SDL_RWread(rw, &newMesh->meshIndices[0], sizeof(unsigned), nElements[1]) != nElements[1])
				CLEAN_ON_FAILED(numNodes + 1);
			if (nElements[2] > 0 && SDL_RWread(rw, &newMesh->meshTexCoords[0], sizeof(float), nElements[2]) != nElements[2])
				CLEAN_ON_FAILED(numNodes + 1);
			if (nElements[3] > 0 && SDL_RWread(rw, &newMesh->meshNormals[0], sizeof(float), nElements[3] * 3) != nElements[3] * 3)
				CLEAN_ON_FAILED(numNodes + 1);

			model[id]->InsertComponent(newMesh);
			newMesh->material = new ComponentMaterial(1.f, 1.f, 1.f, 1.f);
		}	
	}
	SDL_RWclose(rw);
	GameObject* root = model[0];
	root->transform->PropagateTransform();
	sprintf_s(root->meshRoot, file.c_str());
	App->scene->ImportGameObject(root);
	delete[] model;
	LOG("Loaded.");
	return root;
}

inline void SceneImporter::writeToBuffer(std::vector<char> &buffer, unsigned & pointer, const unsigned size, const void * data) const
{
	buffer.resize(buffer.size() + size);
	memcpy(&buffer[pointer], data, size);
	pointer += size;
}

