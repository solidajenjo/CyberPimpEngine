#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleWindow.h"
#include "ModuleProgram.h"
#include "ModuleCamera.h"
#include "GameObject.h"
#include "Transform.h"
#include "sdl/include/SDL_video.h" 
#include "glew-2.1.0/include/GL/glew.h"
#include "ComponentMesh.h"


// Called before render is available
bool ModuleRender::Init()
{
	bool ret = true;
	LOG("Creating Renderer context");

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

	context = SDL_GL_CreateContext(App->window->window);

	glewInit();

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glEnable(GL_TEXTURE_2D);

	glClearDepth(1.0f);
	glClearColor(0.f, 0.f, 0.f, 1.f);
	return ret;
}

update_status ModuleRender::PreUpdate()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	
	return UPDATE_CONTINUE;
}

// Called every draw update
update_status ModuleRender::Update()
{
	return UPDATE_CONTINUE;
}

update_status ModuleRender::PostUpdate()
{

	SDL_GL_SwapWindow(App->window->window);

	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleRender::CleanUp()
{
	LOG("Cleaning Module Renderer");
	renderizables.resize(0); //the meshes are cleanend by their owner gameobjects
	LOG("Cleaning Module Render. Done");
	return true;
}

void ModuleRender::Render() const
{
	float4x4 view = App->camera->frustum.ViewMatrix();
	glUniformMatrix4fv(glGetUniformLocation(App->program->program,
		"view"), 1, GL_TRUE, &view[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(App->program->program,
		"proj"), 1, GL_TRUE, &App->camera->frustum.ProjectionMatrix()[0][0]);
	glUniform1i(glGetUniformLocation(App->program->program, "useColor"), 0);
	for (std::list<ComponentMesh*>::const_iterator it = renderizables.begin(); it != renderizables.end(); ++it) //render meshes
	{
		if (*it == nullptr) 
		{
			LOG("Missing mesh. Couldn't render.");
		}
		else
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, (*it)->texture);
			glUniform1i(glGetUniformLocation(App->program->program, "texture0"), 0);
			
			glUniformMatrix4fv(glGetUniformLocation(App->program->program,
				"model"), 1, GL_TRUE, (*it)->owner->transform->GetModelMatrix());
			//(*it)->owner->transform->rotation.z += 0.01f;
			glBindVertexArray((*it)->VAO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (*it)->VIndex);
			glDrawElements(GL_TRIANGLES, (*it)->nIndices, GL_UNSIGNED_INT, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}
		
	}
	
	glBindVertexArray(0);
}

const std::list<ComponentMesh*>* ModuleRender::getRenderizables() const
{
	return &renderizables;
}

void ModuleRender::insertRenderizable(ComponentMesh * newMesh)
{
	renderizables.push_back(newMesh);
}
