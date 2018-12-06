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
#include "ModuleRender.h"
#include "ModuleTextures.h"
#include "ModuleFileSystem.h"
#include "crossguid/include/crossguid/guid.hpp"
#include "rapidjson-1.1.0/include/rapidjson/prettywriter.h"
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
	std::stack<std::string> stackParent;

	unsigned nMaterials = scene->mNumMaterials;

	MaterialImporter mi;
	std::vector<std::string> materials;
	materials.resize(nMaterials);
	for (unsigned i = 0u; i < nMaterials; ++i)
	{
		materials[i] = mi.Import(scene->mMaterials[i], i);	
	}

	GameObject* root = new GameObject("Root");
	for (unsigned i = 0; i < rootNode->mNumChildren; ++i) //skip rootnode
	{
		stackNode.push(rootNode->mChildren[i]);
		stackParent.push(root->gameObjectUUID);
	}

	rapidjson::StringBuffer sb;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);
	writer.StartArray();
	root->Serialize(writer);
	std::string meshPath = "Library/Meshes/";
	while (!stackNode.empty())
	{
		//process model
		aiNode* node = stackNode.top(); stackNode.pop();		
		GameObject* gameObjectNode = new GameObject(node->mName.C_Str());
		sprintf_s (gameObjectNode->parentUUID, stackParent.top().c_str());
		stackParent.pop();

		for (unsigned i = 0; i < node->mNumChildren; ++i)
		{
			stackNode.push(node->mChildren[i]);	
			stackParent.push(gameObjectNode->gameObjectUUID);
		}
		
		unsigned nMeshes = node->mNumMeshes;

		aiVector3D pos;
		aiVector3D scl;
		aiQuaternion rot;

		node->mTransformation.Decompose(scl, rot, pos);

		Quat q(rot.x, rot.y, rot.z, rot.w); //Quat to translate from black math voodoo to human understable
		gameObjectNode->transform->SetTransform(float3(pos.x, pos.y, pos.z), q.ToEulerXYZ(), float3(scl.x, scl.y, scl.z));
		writer.StartObject();
		writer.String("gameobject");
		gameObjectNode->Serialize(writer);
		writer.String("meshes");
		writer.StartArray();
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
				writer.String(meshName.c_str());
			}			
		}
		writer.EndArray();
		writer.EndObject();
	}

	unsigned extBegin = file.find_last_of('.');
	unsigned filenameBegin;
	filenameBegin = file.find_last_of('/');
	if (filenameBegin == std::string::npos)
		filenameBegin = file.find_last_of('\\') + 1;
	std::string fName = file.substr(filenameBegin, extBegin - filenameBegin);
	std::string importedFileName = "Library/Meshes/" + fName + ".dmd";

	unsigned dataSize = strlen(sb.GetString());
	char* serialData = new char[dataSize];
	memcpy(serialData, sb.GetString(), dataSize);
	App->fileSystem->Write(importedFileName.c_str(), serialData, dataSize);
	RELEASE(serialData);


	return true;
}

GameObject * SceneImporter::Load(const std::string & file, std::vector<GameObject*>& gameObjectMaterials) const
{
	return nullptr;
}
inline void SceneImporter::writeToBuffer(std::vector<char> &buffer, unsigned & pointer, const unsigned size, const void * data) const
{
	buffer.resize(buffer.size() + size);
	memcpy(&buffer[pointer], data, size);
	pointer += size;
}

