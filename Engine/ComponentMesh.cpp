#include "Component.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "ComponentCamera.h"
#include "Application.h"
#include "ModuleEditor.h"
#include "ModuleProgram.h"
#include "ModuleScene.h"
#include "imgui/imgui.h"
#include "MathGeoLib/include/Math/float4x4.h"
#include "crossguid/include/crossguid/guid.hpp"
#include "MathGeoLib/include/Math/MathConstants.h"
#include "SceneImporter.h"
#include "glew-2.1.0/include/GL/glew.h"
#include "debugdraw.h"
#include "GameObject.h"
#pragma warning(push)
#pragma warning(disable : 4996)  
#pragma warning(disable : 4244)  
#pragma warning(disable : 4305)  
#pragma warning(disable : 4838) 
#ifndef PAR_SHAPES_IMPLEMENTATION
#define PAR_SHAPES_IMPLEMENTATION
#include "Util/par_shapes.h"
#pragma warning(pop)
#endif


std::map<std::string, ComponentMesh*>ComponentMesh::meshesLoaded;

ComponentMesh::ComponentMesh(Primitives primitive) : Component(ComponentTypes::MESH_COMPONENT)
{	
	FromPrimitive(primitive);
}

void ComponentMesh::FromPrimitive(Primitives primitive)
{
	par_shapes_mesh_s* mesh;
	primitiveType = primitive;
	switch (primitive)
	{
		case Primitives::CUBE:
			mesh = par_shapes_create_cube();
			break;
		case Primitives::SPHERE:
			mesh = par_shapes_create_parametric_sphere(20, 20);
			break;
		case Primitives::TORUS:
			mesh = par_shapes_create_torus(20, 20, 0.5f);
			break;
		case Primitives::CYLINDER:
			mesh = par_shapes_create_cylinder(12, 12);
			break;
		case Primitives::PLANE:
			mesh = par_shapes_create_plane(2, 2);
			break;
	}
	
	nVertices = mesh->npoints;
	nCoords = mesh-> npoints * 2;
	nIndices = mesh->ntriangles * 3;	
	nNormals = mesh->npoints;
	meshVertices.resize(nVertices);
	for (unsigned i = 0u; i < nVertices; ++i)
	{
		meshVertices[i].x = mesh->points[i * 3];
		meshVertices[i].y = mesh->points[(i * 3) + 1];
		meshVertices[i].z = mesh->points[(i * 3) + 2];
	}

	if (mesh->normals != nullptr)
	{
		meshNormals.resize(nVertices);
		for (unsigned i = 0u; i < nVertices; ++i)
		{
			meshNormals[i].x = mesh->normals[i * 3];
			meshNormals[i].y = mesh->normals[(i * 3) + 1];
			meshNormals[i].z = mesh->normals[(i * 3) + 2];
		}
	}

	if (mesh->tcoords != nullptr)
	{
		meshTexCoords.resize(nCoords);
		memcpy(&meshTexCoords[0], mesh->tcoords, sizeof(float) * nCoords);
	}
	else
		nCoords = 0;

	meshIndices.resize(nIndices);
	for (unsigned i = 0u; i < nIndices; ++i)
	{
		meshIndices[i] = mesh->triangles[i];
	}

	material = new ComponentMaterial(1.f, 1.f, 1.f, 1.f);
	delete mesh;
}

ComponentMesh::ComponentMesh(const std::vector<float>& vertices, const std::vector<unsigned>& indices, const std::vector<float>& texCoords) : Component(ComponentTypes::MESH_COMPONENT)
{
	nVertices = vertices.size() / 3.f;
	meshVertices.resize(nVertices);
	memcpy(&meshVertices[0], &vertices[0], sizeof(float) * nVertices * 3);
	nCoords = texCoords.size();
	if (nCoords > 0)
	{
		meshTexCoords.resize(nCoords);
		memcpy(&meshTexCoords[0], &texCoords[0], sizeof(float) * nCoords);
	}
	nIndices = indices.size();
	if (nIndices > 0)
	{
		meshIndices.resize(nIndices);
		memcpy(&meshIndices[0], &indices[0], sizeof(unsigned) * nIndices);
	}
	material = new ComponentMaterial(1.f, 1.f, 1.f, 1.f);
}

ComponentMesh::~ComponentMesh()
{
	if (VAO != 0)
		glDeleteVertexArrays(1, &VAO);
}

void ComponentMesh::EditorDraw()
{
	//show info from his material
	material->EditorDraw();
}

void ComponentMesh::Render(const ComponentCamera * camera, Transform* transform) const
{
	glUseProgram(material->program);	
	math::float4x4 model = float4x4::identity;
	glUniformMatrix4fv(glGetUniformLocation(material->program,
		"model"), 1, GL_TRUE, transform->GetModelMatrix());
	float4x4 view = camera->frustum.ViewMatrix(); //transform from 3x4 to 4x4
	glUniformMatrix4fv(glGetUniformLocation(material->program,
		"view"), 1, GL_TRUE, &view[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(material->program,
		"proj"), 1, GL_TRUE, &camera->frustum.ProjectionMatrix()[0][0]);
	if (material->texture > 0)
	{
		glUniform1i(glGetUniformLocation(material->program, "useTex"), 1);
		glBindTexture(GL_TEXTURE_2D, material->texture);
	}
	else
		glUniform1i(glGetUniformLocation(material->program, "useTex"), 0);

	glUniform4f(glGetUniformLocation(material->program, "object_color"), material->color.x, material->color.y, material->color.z, 1.0f); 
	float3 lightPos = float3(0.f, 0.f, 10.f);
	
	glUniform3fv(glGetUniformLocation(material->program, "light_pos"), 1, &lightPos[0]);
	glUniform1f(glGetUniformLocation(material->program, "ambient"), 0.9f);
	glUniform1f(glGetUniformLocation(material->program, "shininess"), 15.f);
	glUniform1f(glGetUniformLocation(material->program, "k_ambient"), material->ambient);
	glUniform1f(glGetUniformLocation(material->program, "k_diffuse"), material->diffuse);
	glUniform1f(glGetUniformLocation(material->program, "k_specular"), material->specular);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VIndex);
	glDrawElements(GL_TRIANGLES, nIndices, GL_UNSIGNED_INT, 0);
}

void ComponentMesh::Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	writer.StartObject();
	writer.String("type"); writer.Int((int)type);
	writer.String("meshPath"); writer.String(meshPath);
	writer.String("primitive"); writer.Int((int)primitiveType);
	writer.String("material");
	material->Serialize(writer);
	writer.EndObject();
}

void ComponentMesh::UnSerialize(rapidjson::Value & value)
{
	sprintf_s(meshPath, value["meshPath"].GetString());
	primitiveType = (Primitives)value["primitive"].GetInt();
	if (primitiveType != Primitives::VOID_PRIMITIVE)
	{
		FromPrimitive(primitiveType);
		material->UnSerialize(value["material"]);
	}
	else
	{
		char materialPath[1024];
		sprintf_s(materialPath, value["materialPath"].GetString());
		if (strlen(materialPath) > 0)
		{
			char instanceOf[40];
			sprintf_s(instanceOf, value["material"]["instanceOf"].GetString());
			if (strlen(instanceOf) > 0) {
				GameObject* materialInstance = App->scene->FindInstanceOrigin(instanceOf);
				if (material != nullptr)
				{
					material = (ComponentMaterial*) materialInstance->components.front();
				}
				else
				{
					material->UnSerialize(value["material"]);
				}
			}
		}
		else
		{
			material->UnSerialize(value["material"]);
		}
	}
}

bool ComponentMesh::Release()
{
	if (clients > 1) //still have clients -> no destroy
	{
		--clients;
		return false;
	}
	else
	{
		meshesLoaded.erase(meshPath); //unload completely
		return true;
	}
}

void ComponentMesh::SendToGPU()
{
	if (VAO > 0) //only once at gpu -- TODO: Keep a suscriber counter to know how many gameobjects remain expecting this to be rendered
		return;
	GLuint vbo, vao, vio;
	unsigned totalSize = (sizeof(float3) * 2 * nVertices) + (sizeof(float) * 2 * nVertices);
	unsigned offsetTexCoords = nVertices * sizeof(float3);
	unsigned offsetNormals = offsetTexCoords + (nVertices * sizeof(float) * 2);
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &vio);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);	
	glBufferData(GL_ARRAY_BUFFER, totalSize, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, offsetTexCoords, &meshVertices[0]);
	if (meshTexCoords.size() > 0)
		glBufferSubData(GL_ARRAY_BUFFER, offsetTexCoords, sizeof(float) * meshTexCoords.size(), &meshTexCoords[0]);
	
	if (meshNormals.size() > 0)
		glBufferSubData(GL_ARRAY_BUFFER, offsetNormals, sizeof(float3) * meshNormals.size(), &meshNormals[0]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vio);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned) * nIndices, &meshIndices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);		
	glVertexAttribPointer(0, 3,	GL_FLOAT, GL_FALSE,	0, (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)offsetTexCoords);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE,	0, (void*)offsetNormals);


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	VAO = vao;
	VIndex = vio;
}

void ComponentMesh::ReleaseFromGPU()
{
	if (VAO != 0)
	{
		glDeleteVertexArrays(1, &VAO);
		VAO = 0;
	}
}

ComponentMesh* ComponentMesh::GetMesh(std::string path)
{
	std::map<std::string, ComponentMesh*>::iterator it = meshesLoaded.find(path);
	if (it == meshesLoaded.end())
	{
		SceneImporter si;
		ComponentMesh* mesh = si.LoadMesh(path.c_str());
		if (mesh != nullptr)
		{
			meshesLoaded[path] = mesh;
			++mesh->clients;
			return mesh;
		}
		else
			return nullptr;
	}		
	++(*it).second->clients;
	return (*it).second;
}
