#include "GL/glew.h"
#include "EntityMesh.h"
#include "ModuleModelLoader.h"
#include "Application.h"
#include "ModuleTextures.h"
#include "cimport.h"
#include "postprocess.h"
#include "scene.h"
#include "material.h"
#include "mesh.h"
#include "SDL.h"#include <string>


ModuleModelLoader::ModuleModelLoader()
{
}


ModuleModelLoader::~ModuleModelLoader()
{
	//clean on destroy
	for (unsigned i = 0; i < meshes.size(); ++i)
		glDeleteVertexArrays(1, &meshes[i]);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}

std::vector<EntityMesh*> ModuleModelLoader::Load(char * geometryPath)
{
	LOG("Load model %s", geometryPath);
	std::vector<EntityMesh*> retMeshes;
	const aiScene* scene = aiImportFile(geometryPath, aiProcess_Triangulate);
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
			LOG("Failed loading texures.");
			return retMeshes;
		}
		for (unsigned i = 0; i < scene->mNumMeshes; ++i)
		{		
			//aiNode rootNode = scene->mRootNode->mChildren[i];
			
			EntityMesh* newMesh = new EntityMesh("");
			newMesh->mesh.nIndices = scene->mMeshes[i]->mNumFaces * 3;
			unsigned vio;
			newMesh->mesh.VAO = GenerateMeshData(scene->mMeshes[i], vio);
			newMesh->mesh.VIO = vio;
			newMesh->mesh.texture = tex;
			meshes.push_back(newMesh->mesh.VAO);
			retMeshes.push_back(newMesh);
		}
	}
	aiReleaseImport(scene);
	return retMeshes;
}

unsigned int ModuleModelLoader::GenerateMeshData(aiMesh * mesh, unsigned &vio)
{
	std::vector<unsigned int> indices;
	for (unsigned i = 0; i < mesh->mNumFaces; ++i)
	{
		aiFace face = mesh->mFaces[i];
		indices.push_back(face.mIndices[0]);
		indices.push_back(face.mIndices[1]);
		indices.push_back(face.mIndices[2]);
	}

	GLuint vbo, vao;
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
		void* bufferCoords = glMapBufferRange(GL_ARRAY_BUFFER, offset + sizeof(GLfloat) * 2 * i, sizeof(GLfloat) * 2, GL_MAP_WRITE_BIT);
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

	return vao;
}

unsigned int ModuleModelLoader::GenerateMaterialData(aiMaterial * materials)
{
	aiString file;
	aiReturn ret = materials->GetTexture(aiTextureType_DIFFUSE, 0, &file, nullptr, nullptr);	
	if (ret != AI_SUCCESS) {
		LOG("Failed retrieving texture name.");
		return -1;
	}
	return App->textures->Load(file.C_Str());
}
