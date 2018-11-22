#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "ComponentCamera.h"
#include "Application.h"
#include "ModuleEditor.h"
#include "imgui/imgui.h"
#include "MathGeoLib/include/Math/float4x4.h"
#include "MathGeoLib/include/Math/MathConstants.h"

#ifndef PAR_SHAPES_IMPLEMENTATION
#define PAR_SHAPES_IMPLEMENTATION
#include "Util/par_shapes.h"
#endif

ComponentMesh::ComponentMesh(par_shapes_mesh_s * mesh) : Component("Mesh")
{
	GLuint vbo, vao, vio;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &vio);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * mesh->npoints * 5, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * mesh->npoints * 3, mesh->points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat) * mesh->npoints * 3, sizeof(GLfloat) * mesh->npoints * 2, mesh->points);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vio);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * mesh->ntriangles * 3, &mesh->triangles, GL_STATIC_DRAW);

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
		(void*)(sizeof(float) * 3 * mesh->npoints)    // 3 float 6 vertices for jump positions
	);


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	VAO = vao;
	VIndex = vio;
	nVertices = mesh->npoints;
	nIndices = mesh->ntriangles * 3;
}

void ComponentMesh::EditorDraw()
{
	//show info from his material
	material->EditorDraw();
}

void ComponentMesh::Render(const ComponentCamera * camera, const unsigned program, float r, float g, float b) const
{
	math::float4x4 model = float4x4::identity;
	model = model.FromTRS(float3(0,0,0), float4x4::identity.RotateX(-math::pi / 2), float3(1.f, 1.f, 1.f)); //TODO USE TRANSFORM
	glUniformMatrix4fv(glGetUniformLocation(program,
		"model"), 1, GL_TRUE, &model[0][0]);
	float4x4 view = camera->frustum.ViewMatrix();
	glUniformMatrix4fv(glGetUniformLocation(program,
		"view"), 1, GL_TRUE, &view[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(program,
		"proj"), 1, GL_TRUE, &camera->frustum.ProjectionMatrix()[0][0]);
	glUniform1i(glGetUniformLocation(program, "useColor"), 1);
	//glBindTexture(GL_TEXTURE_2D, skyBox->material->texture); //TODO:USE TEXTURE
	//glUniform4f(glGetUniformLocation(program, "colorU"), r, g, b, 1.0f); // TODO:Use material color
	//glUniform1i(glGetUniformLocation(program, "texture0"), 0);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VIndex);
	glDrawElements(GL_TRIANGLES, nIndices, GL_UNSIGNED_INT, 0);
}
