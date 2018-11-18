#include "Globals.h"
#include "Application.h"
#include "ModuleFrameBuffer.h"
#include "ModuleEditorCamera.h"
#include "ModuleProgram.h"
#include "ModuleEditor.h"
#include "ModuleRender.h"
#include "ModuleScene.h"
#include "GameObject.h"
#include "MathGeoLib/include/MathGeoLib.h"
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
	if (framebuffer != 0)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		glViewport(0, 0, viewPortWidth, viewPortHeight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//render scene to frameBuffer
		
		App->program->UseProgram();
		App->renderer->Render(&App->camera->editorCamera);
		if (App->editor->gizmosEnabled)  //TODO: Consider relocate this in a more apropiated place
		{
			math::float4x4 model = float4x4::identity;
			glUniformMatrix4fv(glGetUniformLocation(App->program->program,
				"model"), 1, GL_TRUE, &model[0][0]);
			float4x4 view = App->camera->editorCamera.frustum.ViewMatrix();
			glUniformMatrix4fv(glGetUniformLocation(App->program->program,
				"view"), 1, GL_TRUE, &view[0][0]);
			glUniformMatrix4fv(glGetUniformLocation(App->program->program,
				"proj"), 1, GL_TRUE, &App->camera->editorCamera.frustum.ProjectionMatrix()[0][0]);
			glUniform1i(glGetUniformLocation(App->program->program, "useColor"), 1);
			glUniform4f(glGetUniformLocation(App->program->program, "colorU"), .2f, .2f, .2f, 1.f);
			glLineWidth(1.0f);
			float d = 200.0f;
			glBegin(GL_LINES);
			for (float i = -d; i <= d; i += 1.0f)
			{
				glVertex3f(i, 0.0f, -d);
				glVertex3f(i, 0.0f, d);
				glVertex3f(-d, 0.0f, i);
				glVertex3f(d, 0.0f, i);
			}
			glEnd();

			glUniform4f(glGetUniformLocation(App->program->program, "colorU"), 1.f, 0.f, 0.f, 1.f); // x Axis
			glBegin(GL_LINES);
			glVertex3f(0.f, 0.1f, 0.f);
			glVertex3f(d, 0.1f, 0.f);
			glEnd();

			glUniform4f(glGetUniformLocation(App->program->program, "colorU"), 0.f, 0.f, 1.f, 1.f); // z Axis
			glBegin(GL_LINES);
			glVertex3f(0.f, 0.1f, d);
			glVertex3f(0.f, 0.1f, 0.f);
			glEnd();

			glUniform4f(glGetUniformLocation(App->program->program, "colorU"), 0.f, 1.f, 0.f, 1.f); // z Axis
			glBegin(GL_LINES);
			glVertex3f(0.f, 0.1f, 0.f);
			glVertex3f(0.f, d, 0.f);
			glEnd();
			
			if (App->scene->selected != nullptr)
			{					
				glUniform1i(glGetUniformLocation(App->program->program, "useColor"), 1);
				glUniformMatrix4fv(glGetUniformLocation(App->program->program,
					"model"), 1, GL_TRUE, App->scene->selected->transform->GetModelMatrix());
				if (App->scene->selected->aaBB != nullptr)
				{
					glUniform4f(glGetUniformLocation(App->program->program, "colorU"), .2f, 1.f, 2.f, 1.f);
					glLineWidth(2);
					glBegin(GL_LINES);
					for (unsigned i = 0; i < App->scene->selected->aaBB->NumEdges(); ++i)
					{
						float3 a = App->scene->selected->aaBB->Edge(i).a;
						float3 b = App->scene->selected->aaBB->Edge(i).b;
						glVertex3f(a.x, a.y, a.z);
						glVertex3f(b.x, b.y, b.z);
					}
					glEnd();
				}
				float4x4 idMat = float4x4::identity;
				glUniformMatrix4fv(glGetUniformLocation(App->program->program,
					"model"), 1, GL_TRUE, &idMat[0][0]);
				float3 position = App->scene->selected->transform->modelMatrixGlobal.Col3(3);
				float3 front = position + App->scene->selected->transform->front;
				float3 up = position + App->scene->selected->transform->up;
				float3 right = position + App->scene->selected->transform->right;
					
				glClear(GL_DEPTH_BUFFER_BIT);
				glUniform4f(glGetUniformLocation(App->program->program, "colorU"), 0.f, 0.f, 1.f, 1.f); // front vector
				glBegin(GL_LINES);
				glVertex3f(position.x, position.y, position.z);
				glVertex3f(front.x, front.y, front.z);
					
				glEnd();

				glUniform4f(glGetUniformLocation(App->program->program, "colorU"), 0.f, 1.f, 0.f, 1.f); // up vector
				glBegin(GL_LINES);
				glVertex3f(position.x, position.y, position.z);
				glVertex3f(up.x, up.y, up.z);
					
				glEnd();
					
				glUniform4f(glGetUniformLocation(App->program->program, "colorU"), 1.f, 0.f, 0.f, 1.f); // right vector
				glBegin(GL_LINES);
				glVertex3f(position.x, position.y, position.z);
				glVertex3f(right.x, right.y, right.z);
					
				glEnd();

				position = App->scene->selected->transform->modelMatrixGlobal.Col3(3);
				front = position + App->scene->selected->transform->front;
				up = position + App->scene->selected->transform->up;
				right = position + App->scene->selected->transform->right;

				glClear(GL_DEPTH_BUFFER_BIT);
				glUniform4f(glGetUniformLocation(App->program->program, "colorU"), 0.f, 0.f, 1.f, 1.f); // front vector
				glBegin(GL_LINES);
				glVertex3f(position.x, position.y, position.z);
				glVertex3f(front.x, front.y, front.z);

				glEnd();

				glUniform4f(glGetUniformLocation(App->program->program, "colorU"), 0.f, 1.f, 0.f, 1.f); // up vector
				glBegin(GL_LINES);
				glVertex3f(position.x, position.y, position.z);
				glVertex3f(up.x, up.y, up.z);

				glEnd();

				glUniform4f(glGetUniformLocation(App->program->program, "colorU"), 1.f, 0.f, 0.f, 1.f); // right vector
				glBegin(GL_LINES);
				glVertex3f(position.x, position.y, position.z);
				glVertex3f(right.x, right.y, right.z);

				glEnd();
				
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
		if (App->scene->sceneCamera != nullptr)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, gameFramebuffer);
			glViewport(0, 0, gameViewPortWidth, gameViewPortHeight);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			App->scene->sceneCamera->Update();
			App->renderer->Render(App->scene->sceneCamera);
		}
		App->program->StopUseProgram();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	else // if on pre was 0 and still 0 something went wrong. Abort
	{
		LOG("Wrong framebuffer status in Module Framebuffer. Aborting.");
		return UPDATE_ERROR;

	}
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

