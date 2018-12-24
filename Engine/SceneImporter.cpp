#include "SceneImporter.h"
#include "Assimp/include/assimp/cimport.h"
#include "Assimp/include/assimp/postprocess.h"
#include "Assimp/include/assimp/scene.h"
#include "Assimp/include/assimp/material.h"
#include "Assimp/include/assimp/mesh.h"
#include "Application.h"
#include "MathGeoLib/include/Math/Quat.h"
#include "GameObject.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "ModuleScene.h"
#include "ModuleFileSystem.h"
#include "MaterialImporter.h"
#include <stack>


bool SceneImporter::Import(const std::string & file) const
{
	LOG("Import model %s", file.c_str());
	const aiScene* scene = aiImportFile(file.c_str(), aiProcess_Triangulate);

	if (scene == nullptr) {
		LOG("Error importing model -> %s", aiGetErrorString());
		return false;
	}

	aiNode* rootNode = scene->mRootNode;
	assert(rootNode != nullptr);

	std::stack<aiNode*> stackNode;
	std::stack<GameObject*> stackParent;

	unsigned nMaterials = scene->mNumMaterials;

	MaterialImporter mi;
	std::vector<std::string> materials;
	materials.resize(nMaterials);
	for (unsigned i = 0u; i < nMaterials; ++i)
	{
		GameObject* mat = new GameObject("Material", true);
		materials[i] = mi.Import(scene->mMaterials[i], mat);	
		RELEASE(mat->transform);
		App->scene->ImportGameObject(mat, ModuleScene::ImportedType::MATERIAL); //import material
	}

	GameObject* root = new GameObject("Root", true);
	for (unsigned i = 0; i < rootNode->mNumChildren; ++i) //skip rootnode
	{
		stackNode.push(rootNode->mChildren[i]);
		stackParent.push(root);
	}

	std::string meshPath = "Library/Meshes/";
	while (!stackNode.empty())
	{
		//process model
		aiNode* node = stackNode.top(); stackNode.pop();		
		GameObject* gameObjectNode = new GameObject(node->mName.C_Str(), true);
		GameObject* parent = stackParent.top(); stackParent.pop();
		sprintf_s (gameObjectNode->parentUUID, parent->gameObjectUUID);

		parent->InsertChild(gameObjectNode);

		for (unsigned i = 0; i < node->mNumChildren; ++i)
		{
			stackNode.push(node->mChildren[i]);	
			stackParent.push(gameObjectNode);
		}
		
		unsigned nMeshes = node->mNumMeshes;

		aiVector3D pos;
		aiVector3D scl;
		aiQuaternion rot;

		node->mTransformation.Decompose(scl, rot, pos);

		Quat q(rot.x, rot.y, rot.z, rot.w); //Quat to translate from black math voodoo to human understable
		gameObjectNode->transform->SetTransform(float3(pos.x, pos.y, pos.z), q.ToEulerXYZ(), float3(scl.x, scl.y, scl.z));
		
		if (nMeshes >= 1) //is a mesh
		{
			
			for (unsigned i = 0u; i < nMeshes; ++i)
			{
				std::vector<char> bytes;
				unsigned bytesPointer = 0u;
				xg::Guid guid = xg::newGuid();
				char meshUUID[40];
				sprintf_s(meshUUID, guid.str().c_str());

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
				writeToBuffer(bytes, bytesPointer, sizeof(char) * 1024, materials[mesh->mMaterialIndex].c_str());
				std::string meshName = meshPath + std::string(meshUUID) + ".msh";
				App->fileSystem->Write(meshName, &bytes[0], bytes.size());
				ComponentMesh* newMesh = LoadMesh(meshName.c_str());
				if (newMesh != nullptr)
				{
					gameObjectNode->InsertComponent(newMesh); //insert the mesh on the new node
				}
			}			
		}		
	}
	App->scene->ImportGameObject(root, ModuleScene::ImportedType::MODEL);	
	return true;
}


inline void SceneImporter::writeToBuffer(std::vector<char> &buffer, unsigned & pointer, const unsigned size, const void * data) const
{
	buffer.resize(buffer.size() + size);
	memcpy(&buffer[pointer], data, size);
	pointer += size;
}

ComponentMesh * SceneImporter::LoadMesh(const char path[1024]) const
{
	unsigned size = App->fileSystem->Size(std::string(path));
	if (size > 0)
	{
		char* buffer = new char[size];
		if (App->fileSystem->Read(std::string(path), buffer, size))
		{
			//read mesh header
			unsigned nVertices, nIndices, nCoords, nNormals;
			memcpy(&nVertices, &buffer[0], sizeof(unsigned));
			memcpy(&nIndices, &buffer[sizeof(unsigned) * 1], sizeof(unsigned));
			memcpy(&nCoords, &buffer[sizeof(unsigned) * 2], sizeof(unsigned));
			memcpy(&nNormals, &buffer[sizeof(unsigned) * 3], sizeof(unsigned));

			unsigned verticesOffset = 4 * sizeof(unsigned);
			unsigned verticesSize = nVertices * sizeof(float) * 3;
			unsigned indicesOffset = verticesOffset + verticesSize;
			unsigned indicesSize = nIndices * sizeof(unsigned);
			unsigned coordsOffset = indicesOffset + indicesSize;
			unsigned coordsSize = nCoords * sizeof(float);
			unsigned normalsOffset = coordsOffset + coordsSize;
			unsigned normalsSize = nNormals * sizeof(float) * 3;
			unsigned materialsOffset = normalsOffset + normalsSize;

			//create Mesh component
			ComponentMesh* newMesh = new ComponentMesh();			
			newMesh->nVertices = nVertices;
			newMesh->nIndices = nIndices;
			newMesh->nCoords = nCoords;
			newMesh->nNormals = nNormals;
			newMesh->meshVertices.resize(nVertices);
			memcpy(&newMesh->meshVertices[0], &buffer[verticesOffset], verticesSize);
			if (nIndices > 0)
			{
				newMesh->meshIndices.resize(nIndices);
				memcpy(&newMesh->meshIndices[0], &buffer[indicesOffset], indicesSize);
			}
			if (nCoords > 0)
			{
				newMesh->meshTexCoords.resize(nCoords);
				memcpy(&newMesh->meshTexCoords[0], &buffer[coordsOffset], coordsSize);
			}
			if (nNormals > 0)
			{
				newMesh->meshNormals.resize(nNormals);
				memcpy(&newMesh->meshNormals[0], &buffer[normalsOffset], normalsSize);
			}

			char materialPath[1024];
			memcpy(&materialPath[0], &buffer[materialsOffset], sizeof(char) * 1024);

			std::string matPath(materialPath);

			newMesh->material = ComponentMaterial::GetMaterial(matPath);			
			sprintf_s(newMesh->meshPath, path);
			RELEASE(buffer);			
			return newMesh;
		}
		RELEASE(buffer);
	}
	return nullptr;
}

void SceneImporter::SaveMesh(const ComponentMesh* mesh, const char path[1024]) const
{
	std::vector<char> bytes;
	unsigned bytesPointer = 0u;

	writeToBuffer(bytes, bytesPointer, sizeof(unsigned), &mesh->nVertices);
	writeToBuffer(bytes, bytesPointer, sizeof(unsigned), &mesh->nIndices);
	writeToBuffer(bytes, bytesPointer, sizeof(unsigned), &mesh->nCoords);
	writeToBuffer(bytes, bytesPointer, sizeof(unsigned), &mesh->nNormals);

	writeToBuffer(bytes, bytesPointer, sizeof(float) * 3 * mesh->nVertices, &mesh->meshVertices[0]);

	if (mesh->nIndices > 0)
	{
		writeToBuffer(bytes, bytesPointer, sizeof(unsigned) * mesh->meshIndices.size(), &mesh->meshIndices[0]);
	}

	if (mesh->nCoords > 0)
	{
		writeToBuffer(bytes, bytesPointer, sizeof(float) * mesh->meshTexCoords.size(), &mesh->meshTexCoords[0]);
	}

	if (mesh->nNormals > 0)
	{
		writeToBuffer(bytes, bytesPointer, sizeof(float) * 3 * mesh->nNormals, &mesh->meshNormals[0]);
	}
	writeToBuffer(bytes, bytesPointer, sizeof(char) * 1024, mesh->material->materialPath);

	App->fileSystem->Write(path, &bytes[0], bytes.size());

}

