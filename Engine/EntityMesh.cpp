#include "Entity.h"
#include "GL/glew.h"
#include "MathGeoLib.h"
#include "Application.h"
#include "ModuleProgram.h"
#include "ModuleCamera.h"
#include "EntityMesh.h"
#include "Application.h"
#include "ModuleModelLoader.h"
#include "ModuleTextures.h"
#include "Transform.h"

bool EntityMesh::Render() const
{	
	//transform->rotation.y += 0.02f;
	//transform->rotation.x += 0.02f;
	//transform->rotation.z += 0.02f;
	glUseProgram(App->program->program);
	math::float4x4 model = float4x4::identity;
	glUniformMatrix4fv(glGetUniformLocation(App->program->program,
		"model"), 1, GL_TRUE, transform->getModelMatrix());
	glUniformMatrix4fv(glGetUniformLocation(App->program->program,
		"view"), 1, GL_TRUE, &App->camera->view[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(App->program->program,
		"proj"), 1, GL_TRUE, &App->camera->proj[0][0]);
	glActiveTexture(GL_TEXTURE0);
	glUniform1f(glGetUniformLocation(App->program->program, "uvMultiplier"), 1);
	glBindTexture(GL_TEXTURE_2D, mesh.texture);
	glUniform1i(glGetUniformLocation(App->program->program, "texture0"), 0); 
	glBindVertexArray(mesh.VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.VIO);
	glDrawElements(GL_TRIANGLES, mesh.nIndices, GL_UNSIGNED_INT, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	return true;
}
