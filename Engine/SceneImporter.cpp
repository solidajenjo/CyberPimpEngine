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
#include "rapidjson-1.1.0/include/rapidjson/document.h"
#include "rapidjson-1.1.0/include/rapidjson/error/en.h"
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
	writer.EndArray();
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
	Load(importedFileName);
	return true;
}

void SceneImporter::Load(const std::string & file) const
{
	GameObject* go = nullptr;	
	std::map<std::string, ComponentMaterial*> materials;
	unsigned size = App->fileSystem->Size(file);
	if (size > 0)
	{
		char* buffer = new char[size];
		App->fileSystem->Read(file, buffer, size);
		rapidjson::Document document;
		if (document.Parse<rapidjson::kParseStopWhenDoneFlag>(buffer).HasParseError())
		{
			LOG("Error parsing model. Model file corrupted -> %s -> %d", rapidjson::GetParseError_En(document.GetParseError()), document.GetErrorOffset());
			return;
		}
		for (rapidjson::Value::ValueIterator it = document.GetArray().Begin(); it != document.GetArray().End(); ++it)
		{
			go = new GameObject("");
			if ((it)->HasMember("gameobject"))
			{
				go->UnSerialize((*it)["gameobject"], false);
			}
			else
			{
				go->UnSerialize((*it), false);
			}			
			App->scene->ImportGameObject(go);
			if ((*it).HasMember("meshes"))
			{
				for (rapidjson::Value::ValueIterator it2 = (*it)["meshes"].GetArray().Begin(); it2 != (*it)["meshes"].GetArray().End(); ++it2)
				{
					char path[1024];
					sprintf_s(path, (*it2).GetString());
					go->InsertComponent(LoadMesh(path, materials));
				}
			}
		}
	}
}
inline void SceneImporter::writeToBuffer(std::vector<char> &buffer, unsigned & pointer, const unsigned size, const void * data) const
{
	buffer.resize(buffer.size() + size);
	memcpy(&buffer[pointer], data, size);
	pointer += size;
}

ComponentMesh * SceneImporter::LoadMesh(const char path[1024], std::map<std::string, ComponentMaterial*> &materials) const
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

			std::map<std::string, ComponentMaterial*>::iterator it = materials.find(matPath);
			if (it == materials.end()) //Load from disk
			{
				MaterialImporter mi;
				GameObject* mat = mi.Load(materialPath);
				if (mat != nullptr)
				{
					App->scene->ImportGameObject(mat);
					materials[matPath] = (ComponentMaterial*) mat->components.front();
					newMesh->material = (ComponentMaterial*) mat->components.front();
				}
			}
			else //Use loaded
			{
				newMesh->material = materials[matPath];
			}
			RELEASE(buffer);
			return newMesh;
		}
		RELEASE(buffer);
	}
	return nullptr;
}

