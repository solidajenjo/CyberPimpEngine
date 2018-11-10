#include "glew-2.1.0/include/GL/glew.h"
#include "ModuleModelLoader.h"
#include "Application.h"
#include "ModuleTextures.h"
#include "ModuleRender.h"
#include "Assimp/include/assimp/cimport.h"
#include "Assimp/include/assimp/postprocess.h"
#include "Assimp/include/assimp/scene.h"
#include "Assimp/include/assimp/material.h"
#include "Assimp/include/assimp/mesh.h"
#include "GameObject.h"
#include "Transform.h"
#include "ComponentMesh.h"
#include <string>


void ModuleModelLoader::Load(std::string geometryPath)//TODO: Test with other assets
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
			return;
		}

		GameObject* retGameObject = GenerateMeshData(scene->mRootNode, scene, tex);
		if (retGameObject != nullptr)
		{
			LOG("Model loaded.");
		}
		else
		{
			LOG("Couldn't load model %s", geometryPath.c_str());
		}
	}
	aiReleaseImport(scene);	
}

GameObject* ModuleModelLoader::GenerateMeshData(aiNode* node, const aiScene* scene, unsigned texture)
{
	assert(node != nullptr);
	assert(scene != nullptr);
	LOG("Model node -> %s", node->mName.C_Str());
	GameObject* newGO = new GameObject();
	newGO->name = std::string(node->mName.C_Str());
	for (unsigned i = 0; i < node->mNumChildren; ++i)
	{
		newGO->children.push_back(GenerateMeshData(node->mChildren[i], scene, texture));
	}

	if (node->mNumMeshes == 1) //is a leaf of the tree
	{
		unsigned meshPointer = *node->mMeshes;
		aiMesh* mesh = scene->mMeshes[meshPointer];

		std::vector<unsigned> indices;
		for (unsigned i = 0; i < mesh->mNumFaces; ++i)
		{
			aiFace face = mesh->mFaces[i];
			assert(scene->mMeshes[meshPointer]->mFaces[i].mNumIndices == 3);

			indices.push_back(face.mIndices[0]);
			indices.push_back(face.mIndices[1]);
			indices.push_back(face.mIndices[2]);
		}

		GLuint vbo, vao, vio;
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
		glGenBuffers(1, &vio);

		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * mesh->mNumVertices * 5, NULL, GL_STATIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * mesh->mNumVertices * 3, mesh->mVertices);

		float offset = sizeof(GLfloat) * mesh->mNumVertices * 3;
		for (unsigned i = 0; i < mesh->mNumVertices; ++i)
		{
			float* bufferCoords = (float*)glMapBufferRange(GL_ARRAY_BUFFER, offset + sizeof(GLfloat) * 2 * i, sizeof(GLfloat) * 2, GL_MAP_WRITE_BIT);
			memcpy(bufferCoords, &mesh->mTextureCoords[0][i].x, sizeof(GLfloat) * 2);
			glUnmapBuffer(GL_ARRAY_BUFFER);
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
		for (unsigned i = 0; i < scene->mMeshes[meshPointer]->mNumVertices; ++i)
			compMesh->vertices.push_back(float3(scene->mMeshes[meshPointer]->mVertices[i].x, scene->mMeshes[meshPointer]->mVertices[i].y, scene->mMeshes[meshPointer]->mVertices[i].z));

		compMesh->VAO = vao;
		compMesh->VIndex = vio;
		compMesh->texture = texture;
		compMesh->owner = newGO;

		//TODO:Get & Set mesh transform
		
		App->renderer->renderizables.push_back(compMesh);

		newGO->components.push_back(compMesh);
		
	}
	return newGO;
}

unsigned int ModuleModelLoader::GenerateMaterialData(aiMaterial * materials)
{
	assert(materials != nullptr);

	aiString file;
	aiReturn ret = materials->GetTexture(aiTextureType_DIFFUSE, 0, &file, nullptr, nullptr);	
	if (ret != AI_SUCCESS) {
		LOG("Failed retrieving texture name.");
		return -1;
	}
	return App->textures->Load(file.C_Str());
}
