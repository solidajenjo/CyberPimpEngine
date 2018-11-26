#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "ComponentCamera.h"
#include "Application.h"
#include "ModuleEditor.h"
#include "ModuleProgram.h"
#include "imgui/imgui.h"
#include "MathGeoLib/include/Math/float4x4.h"
#include "MathGeoLib/include/Math/MathConstants.h"
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

ComponentMesh::ComponentMesh(Primitives primitive) : Component("Mesh")
{
	par_shapes_mesh_s* mesh;
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

ComponentMesh::ComponentMesh(const std::vector<float>& vertices, const std::vector<unsigned>& indices, const std::vector<float>& texCoords) : Component("Mesh")
{
	
	nVertices = vertices.size();
	meshVertices.resize(nVertices);
	memcpy(&meshVertices[0], &vertices[0], sizeof(float) * nVertices);
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

void ComponentMesh::EditorDraw()
{
	//show info from his material
	material->EditorDraw();
	ImGui::Checkbox("Draw Normals", &showNormals);
	ImGui::SliderFloat("Normal length", &normalLength, 1.0f, 5.0f);
}

void ComponentMesh::Render(const ComponentCamera * camera) const
{
	glUseProgram(material->program);
	owner->transform->PropagateTransform();
	math::float4x4 model = float4x4::identity;
	glUniformMatrix4fv(glGetUniformLocation(material->program,
		"model"), 1, GL_TRUE, owner->transform->GetModelMatrix());
	float4x4 view = camera->frustum.ViewMatrix(); //transform from 3x4 to 4x4
	glUniformMatrix4fv(glGetUniformLocation(material->program,
		"view"), 1, GL_TRUE, &view[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(material->program,
		"proj"), 1, GL_TRUE, &camera->frustum.ProjectionMatrix()[0][0]);
		//glBindTexture(GL_TEXTURE_2D, skyBox->material->texture); //TODO:USE TEXTURE
	glUniform4f(glGetUniformLocation(material->program, "object_color"), material->color.x, material->color.y, material->color.z, 1.0f); // TODO:Use material color
	float3 lightPos = float3(0.f, 0.f, 10.f);
	
	glUniform3fv(glGetUniformLocation(material->program, "light_pos"), 1, &lightPos[0]);
	glUniform1f(glGetUniformLocation(material->program, "ambient"), 0.6f);
	glUniform1f(glGetUniformLocation(material->program, "shininess"), 65.f);
	glUniform1f(glGetUniformLocation(material->program, "k_ambient"), material->ambient);
	glUniform1f(glGetUniformLocation(material->program, "k_diffuse"), material->diffuse);
	glUniform1f(glGetUniformLocation(material->program, "k_specular"), material->specular);


	//glUniform1i(glGetUniformLocation(program, "texture0"), 0);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VIndex);
	glDrawElements(GL_TRIANGLES, nIndices, GL_UNSIGNED_INT, 0);
	
	if (showNormals)
	{
		glUseProgram(App->program->default);
		glUniformMatrix4fv(glGetUniformLocation(App->program->default,
			"model"), 1, GL_TRUE, owner->transform->GetModelMatrix());

		glUniformMatrix4fv(glGetUniformLocation(App->program->default,
			"view"), 1, GL_TRUE, &view[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(App->program->default,
			"proj"), 1, GL_TRUE, &camera->frustum.ProjectionMatrix()[0][0]);

		for (unsigned i = 0u; i < nVertices; ++i)
		{
			dd::vertexNormal(meshVertices[i], meshNormals[i], normalLength);
		}
		glBindVertexArray(0);
	}
}

void ComponentMesh::SendToGPU()
{
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
	glBufferSubData(GL_ARRAY_BUFFER, offsetTexCoords, sizeof(float) * 2 * nVertices, &meshTexCoords[0]);
	glBufferSubData(GL_ARRAY_BUFFER, offsetNormals, sizeof(float3) * nVertices, &meshNormals[0]);
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
		glDeleteVertexArrays(1, &VAO);
}
