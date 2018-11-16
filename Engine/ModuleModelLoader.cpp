#include "glew-2.1.0/include/GL/glew.h"
#include "ModuleModelLoader.h"
#include "Application.h"
#include "ModuleTextures.h"
#include "ModuleRender.h"
#include "ModuleCamera.h"
#include "ModuleScene.h"
#include "Assimp/include/assimp/cimport.h"
#include "Assimp/include/assimp/postprocess.h"
#include "Assimp/include/assimp/scene.h"
#include "Assimp/include/assimp/material.h"
#include "Assimp/include/assimp/mesh.h"
#include "MathGeoLib/include/Geometry/OBB.h"
#include "GameObject.h"
#include "Transform.h"
#include "ComponentMesh.h"
#include <stack>
#include <string>




void ModuleModelLoader::Load(const std::string &geometryPath)
{
	assert(geometryPath != "");
	
	LOG("Load model %s", geometryPath.c_str());	;
	const aiScene* scene = aiImportFile(geometryPath.c_str(), aiProcess_Triangulate);
	if (scene == nullptr) {
		LOG("Error importing model -> %s", aiGetErrorString());
	}
	else
	{
		unsigned tex;

		for (unsigned i = 0; i < scene->mNumMaterials; ++i)
		{
			tex = GenerateMaterialData(scene->mMaterials[i]);
		}
		if (tex == 0) 
		{
			LOG("Failed loading texures -> %s", aiGetErrorString()); 
		}
		allCorners.resize(0);
		GameObject* retGameObject = GenerateMeshData(scene->mRootNode, scene, tex);

		if (retGameObject != nullptr)
		{
			LOG("Model loaded.");			
			aiVector3D pos;
			aiVector3D scl;
			aiQuaternion rot;

			scene->mRootNode->mTransformation.Decompose(scl, rot, pos);
			retGameObject->transform->SetPosition(float3(pos.x, pos.y, pos.z));
			Quat q(rot.x, rot.y, rot.z, rot.w); //Quat to translate from black math voodoo to human understable
			retGameObject->transform->SetRotation(q.ToEulerXYZ());
			retGameObject->transform->SetScale(float3(scl.x, scl.y, scl.z));
			retGameObject->transform->RecalcModelMatrix();

			App->scene->insertGameObject(retGameObject); //scene handles all the gameobjects -> must clean them
			
		}
		else
		{
			LOG("Couldn't load model %s", geometryPath.c_str());
		}
	}
	aiReleaseImport(scene);	
}

GameObject* ModuleModelLoader::GenerateMeshData(aiNode* rootNode, const aiScene* scene, unsigned texture)
{
	assert(rootNode != nullptr);
	assert(scene != nullptr);
	GameObject* rootGO = new GameObject();
	std::stack<GameObject*> stackGO;
	stackGO.push(rootGO);
	std::stack<aiNode*> stackNode;
	stackNode.push(rootNode);
	while (!stackGO.empty())
	{
		//process model
		
		GameObject* newGO = stackGO.top(); stackGO.pop();
		aiNode* node = stackNode.top(); stackNode.pop();
		LOG("Model node -> %s", node->mName.C_Str());
		newGO->name = std::string(node->mName.C_Str());			
		for (unsigned i = 0; i < node->mNumChildren; ++i)
		{			
			GameObject* newChild = new GameObject();
			newChild->parent = newGO;
			newGO->children.push_back(newChild);
			stackGO.push(newChild);
			stackNode.push(node->mChildren[i]);
		}

		if (node->mNumMeshes == 1) //is a mesh -  otherwise is anything i don't care for now
		{
			unsigned meshPointer = *node->mMeshes;
			aiMesh* mesh = scene->mMeshes[meshPointer];

			std::vector<unsigned> indices;
			indices.resize(mesh->mNumFaces * 3);
			for (unsigned i = 0; i < mesh->mNumFaces; ++i)
			{
				aiFace face = mesh->mFaces[i];
				assert(scene->mMeshes[meshPointer]->mFaces[i].mNumIndices == 3);

				indices[i * 3] = face.mIndices[0];
				indices[(i * 3) + 1] = (face.mIndices[1]);
				indices[(i * 3) + 2] = (face.mIndices[2]);
			}

			newGO->aaBB = new AABB();
			newGO->aaBB->SetFrom((float3*)mesh->mVertices, mesh->mNumVertices);
			allCorners.push_back(float3(newGO->aaBB->MaxX(), newGO->aaBB->MaxY(), newGO->aaBB->MaxZ()));
			allCorners.push_back(float3(newGO->aaBB->MinX(), newGO->aaBB->MinY(), newGO->aaBB->MinZ()));

			GLuint vbo, vao, vio;
			glGenVertexArrays(1, &vao);
			glGenBuffers(1, &vbo);
			glGenBuffers(1, &vio);
			glBindVertexArray(vao);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * mesh->mNumVertices * 5, NULL, GL_STATIC_DRAW);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * mesh->mNumVertices * 3, mesh->mVertices);
			float offset = sizeof(GLfloat) * mesh->mNumVertices * 3;
			if (mesh->mNumUVComponents[0] == 0) //for now only channel 0 matters
			{
				LOG("No texture coords found. Skipping %s", node->mName.C_Str());
			}
			else
			{
				for (unsigned i = 0; i < mesh->mNumVertices; ++i)
				{
					float* bufferCoords = (float*)glMapBufferRange(GL_ARRAY_BUFFER, offset + sizeof(GLfloat) * 2 * i, sizeof(GLfloat) * 2, GL_MAP_WRITE_BIT);
					memcpy(bufferCoords, &mesh->mTextureCoords[0][i].x, sizeof(GLfloat) * 2);
					glUnmapBuffer(GL_ARRAY_BUFFER);
				}
				LOG("%s Texture coords OK.", node->mName.C_Str());
			}
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vio);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * mesh->mNumFaces * 3, &indices[0], GL_STATIC_DRAW);

			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(
				0,                  // attribute 0
				3,                  // number of componentes (3 floats)
				GL_FLOAT,           // data type
				GL_FALSE,           // should be normalized?
				0,                  // stride
				(void*)0            // array buffer offset
			);
			glVertexAttribPointer(
				1,
				2,
				GL_FLOAT,
				GL_FALSE,
				0,
				(void*)(sizeof(float) * 3 * mesh->mNumVertices)    // 3 float 6 vertices for jump positions
			);


			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);

			ComponentMesh* compMesh = new ComponentMesh();
			compMesh->nVertices = scene->mMeshes[meshPointer]->mNumVertices;
			compMesh->nIndices = scene->mMeshes[meshPointer]->mNumFaces * 3;
			compMesh->vertices.resize(scene->mMeshes[meshPointer]->mNumVertices);
			for (unsigned i = 0; i < scene->mMeshes[meshPointer]->mNumVertices; ++i)
				compMesh->vertices[i] = (float3(scene->mMeshes[meshPointer]->mVertices[i].x, scene->mMeshes[meshPointer]->mVertices[i].y, scene->mMeshes[meshPointer]->mVertices[i].z));

			compMesh->VAO = vao;
			compMesh->VIndex = vio;
			compMesh->texture = texture;
			compMesh->owner = newGO;

			aiVector3D pos;
			aiVector3D scl;
			aiQuaternion rot;

			node->mTransformation.Decompose(scl, rot, pos);

			Quat q(rot.x, rot.y, rot.z, rot.w); //Quat to translate from black math voodoo to human understable

			newGO->transform->SetPosition(float3(pos.x, pos.y, pos.z));
			newGO->transform->SetRotation(q.ToEulerXYZ());
			newGO->transform->SetScale(float3(scl.x, scl.y, scl.z));
			newGO->transform->RecalcModelMatrix();
			App->renderer->insertRenderizable(compMesh);

			newGO->components.push_back(compMesh);
		}
		else
		{
			LOG("%s is not a mesh. Skipping.", node->mName.C_Str());
		}

	}
	return rootGO;
}

unsigned int ModuleModelLoader::GenerateMaterialData(aiMaterial * materials)
{
	assert(materials != nullptr);
	aiString file;
	aiReturn ret = materials->GetTexture(aiTextureType_DIFFUSE, 0, &file, nullptr, nullptr);	
	if (file.C_Str()[0] == 0)
	{
		LOG("Texture data not found. Load it Manually.");
		return -1;
	}
	if (ret != AI_SUCCESS) {
		LOG("Failed retrieving texture %s.", file.C_Str());
		return -1;
	}
	return App->textures->Load(file.C_Str());
}

