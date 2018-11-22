#include "Globals.h"
#include "Application.h"
#include "ModuleFrameBuffer.h"
#include "ModuleEditorCamera.h"
#include "ModuleProgram.h"
#include "ModuleEditor.h"
#include "ModuleRender.h"
#include "ModuleScene.h"
#include "ModuleDebugDraw.h"
#include "GameObject.h"
#include "ComponentCamera.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "debugdraw.h"
#include "MathGeoLib/include/MathGeoLib.h"
#include "Util/par_shapes.h"
#include "glew-2.1.0/include/GL/glew.h"


// Called before render is available
bool ModuleFrameBuffer::Init()
{
	bool ret = true;		

	return ret;
}

update_status ModuleFrameBuffer::PreUpdate()
{
	if (framebuffer != 0)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	else //post glew initialization required
	{
		glGenFramebuffers(1, &framebuffer);
		glGenRenderbuffers(1, &colorRenderbuffer);
		glGenRenderbuffers(1, &depthRenderbuffer);
		// generate textures
		glGenTextures(1, &texColorBuffer);
	}

	if (gameFramebuffer != 0)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, gameFramebuffer);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	else //post glew initialization required
	{
		glGenFramebuffers(1, &gameFramebuffer);
		glGenRenderbuffers(1, &gameColorRenderbuffer);
		glGenRenderbuffers(1, &gameDepthRenderbuffer);
		// generate textures
		glGenTextures(1, &gameTexColorBuffer);
	}
	return UPDATE_CONTINUE;
}

// Called every draw update
update_status ModuleFrameBuffer::Update()
{
	assert(framebuffer != 0);
	if (framebuffer != 0) //render editor
	{
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		glViewport(0, 0, viewPortWidth, viewPortHeight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		
		//render scene to frameBuffer
		ComponentMesh torus = ComponentMesh(par_shapes_create_torus(12, 4, .5f));
		torus.Render(&App->camera->editorCamera, App->program->program, 0.5f, 0.5f, 0.5f);
		App->program->UseProgram();
		App->renderer->Render(&App->camera->editorCamera);
		assert(skyBox != nullptr);
		if (App->camera->editorCamera.useSkyBox && skyBox != nullptr) //draw skybox
		{
			math::float4x4 model = float4x4::identity;
			model = model.FromTRS(App->camera->editorCamera.camPos, float4x4::identity.RotateX(-math::pi /2), float3(100.f, 100.f, 100.f));
			glUniformMatrix4fv(glGetUniformLocation(App->program->program,
				"model"), 1, GL_TRUE, &model[0][0]);
			float4x4 view = App->camera->editorCamera.frustum.ViewMatrix();
			glUniformMatrix4fv(glGetUniformLocation(App->program->program,
				"view"), 1, GL_TRUE, &view[0][0]);
			glUniformMatrix4fv(glGetUniformLocation(App->program->program,
				"proj"), 1, GL_TRUE, &App->camera->editorCamera.frustum.ProjectionMatrix()[0][0]);
			glUniform1i(glGetUniformLocation(App->program->program, "useColor"), 0);
			glBindTexture(GL_TEXTURE_2D, skyBox->material->texture);
			glUniform4f(glGetUniformLocation(App->program->program, "colorU"), skyBox->material->color.x, skyBox->material->color.y, skyBox->material->color.z, 1.0f); // material color
			glUniform1i(glGetUniformLocation(App->program->program, "texture0"), 0);
			glBindVertexArray(skyBox->VAO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyBox->VIndex);
			glDrawElements(GL_TRIANGLES, skyBox->nIndices, GL_UNSIGNED_INT, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}
		dd::xzSquareGrid(-200.f, 200.f, 0.f, 1.f, dd::colors::DarkGray);
		if (App->editor->gizmosEnabled)  //TODO: Consider relocate this in a more apropiated place
		{
			App->debugDraw->Draw(&App->camera->editorCamera, framebuffer, viewPortWidth, viewPortHeight);
			
			if (App->scene->selected != nullptr)
			{
				dd::aabb(App->scene->selected->aaBBGlobal.minPoint, App->scene->selected->aaBBGlobal.maxPoint, dd::colors::Coral);
			}

			if (App->scene->sceneCamera != nullptr)
			{
				float4x4 idn = float4x4::identity;
				glUniform1i(glGetUniformLocation(App->program->program, "useColor"), 1);
				glUniformMatrix4fv(glGetUniformLocation(App->program->program,
					"model"), 1, GL_TRUE, &idn[0][0]);
				glBegin(GL_LINES);
				glUniform4f(glGetUniformLocation(App->program->program, "colorU"), .4f, 1.f, .4f, 1.f);
				for (unsigned i = 0; i < App->scene->sceneCamera->frustum.NumEdges(); ++i)
				{
					float3 a = App->scene->sceneCamera->frustum.Edge(i).a;
					float3 b = App->scene->sceneCamera->frustum.Edge(i).b;
					glVertex3f(a.x, a.y, a.z);
					glVertex3f(b.x, b.y, b.z);

				}
				glEnd();
			}
		}
	}
	assert(gameFramebuffer != 0);
	if (gameFramebuffer != 0 && App->scene->sceneCamera != nullptr)  //render game
	{
		App->program->UseProgram();
		glBindFramebuffer(GL_FRAMEBUFFER, gameFramebuffer);
		glViewport(0, 0, gameViewPortWidth, gameViewPortHeight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		assert(skyBox != nullptr);
		if (App->scene->sceneCamera->useSkyBox && skyBox != nullptr) //draw skybox
		{
			math::float4x4 model = float4x4::identity;
			model = model.FromTRS(App->scene->sceneCamera->owner->transform->getGlobalPosition(), float4x4::identity.RotateX(-math::pi / 2), float3(100.f, 100.f, 100.f));
			glUniformMatrix4fv(glGetUniformLocation(App->program->program,
				"model"), 1, GL_TRUE, &model[0][0]);
			float4x4 view = App->scene->sceneCamera->frustum.ViewMatrix();
			glUniformMatrix4fv(glGetUniformLocation(App->program->program,
				"view"), 1, GL_TRUE, &view[0][0]);
			glUniformMatrix4fv(glGetUniformLocation(App->program->program,
				"proj"), 1, GL_TRUE, &App->scene->sceneCamera->frustum.ProjectionMatrix()[0][0]);
			glUniform1i(glGetUniformLocation(App->program->program, "useColor"), 0);
			glBindTexture(GL_TEXTURE_2D, skyBox->material->texture);
			glUniform4f(glGetUniformLocation(App->program->program, "colorU"), skyBox->material->color.x, skyBox->material->color.y, skyBox->material->color.z, 1.0f); // material color
			glUniform1i(glGetUniformLocation(App->program->program, "texture0"), 0);
			glBindVertexArray(skyBox->VAO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyBox->VIndex);
			glDrawElements(GL_TRIANGLES, skyBox->nIndices, GL_UNSIGNED_INT, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}
		App->scene->sceneCamera->Update();
		App->renderer->Render(App->scene->sceneCamera);
	}
	App->program->StopUseProgram();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
		
	return UPDATE_CONTINUE;
}

update_status ModuleFrameBuffer::PostUpdate()
{
	if (framebuffer != 0)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	if (gameFramebuffer != 0)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, gameFramebuffer);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleFrameBuffer::CleanUp()
{
	LOG("Cleaning Module Framebuffer");
	glDeleteFramebuffers(1, &framebuffer);
	glDeleteTextures(1, &texColorBuffer);
	glDeleteFramebuffers(1, &gameFramebuffer);
	glDeleteTextures(1, &gameTexColorBuffer);
	glDeleteTextures(1, &skyBox->material->texture);
	RELEASE(skyBox);
	LOG("Cleaning Module Framebuffer. Done");
	return true;
}

bool ModuleFrameBuffer::RecalcFrameBufferTexture()
{
	bool ret = true;
		
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer); 

	glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, viewPortWidth, viewPortHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorRenderbuffer);

	glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, viewPortWidth, viewPortHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer);

	glBindTexture(GL_TEXTURE_2D, texColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, viewPortWidth, viewPortHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		LOG("Failed to make complete framebuffer object %x", status);
		ret = false;
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, gameFramebuffer);

	glBindRenderbuffer(GL_RENDERBUFFER, gameColorRenderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, gameViewPortWidth, gameViewPortHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorRenderbuffer);

	glBindRenderbuffer(GL_RENDERBUFFER, gameDepthRenderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, gameViewPortWidth, gameViewPortHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, gameDepthRenderbuffer);

	glBindTexture(GL_TEXTURE_2D, gameTexColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, gameViewPortWidth, gameViewPortHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gameTexColorBuffer, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		LOG("Failed to make complete framebuffer object %x", status);
		ret = false;
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return ret;
}

