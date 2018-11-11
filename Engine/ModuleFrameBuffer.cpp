#include "Globals.h"
#include "Application.h"
#include "ModuleFrameBuffer.h"
#include "ModuleCamera.h"
#include "ModuleProgram.h"
#include "ModuleEditor.h"
#include "ModuleRender.h"
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
		// generate texture
		glGenTextures(1, &texColorBuffer);
	}
	return UPDATE_CONTINUE;
}

// Called every draw update
update_status ModuleFrameBuffer::Update()
{
	if (framebuffer != 0)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//render scene to frameBuffer
		
		App->program->UseProgram();
		App->renderer->Render();
		if (App->editor->gizmosEnabled)
		{
			math::float4x4 model = float4x4::identity;
			glUniformMatrix4fv(glGetUniformLocation(App->program->program,
				"model"), 1, GL_TRUE, &model[0][0]);
			float4x4 view = App->camera->frustum.ViewMatrix();
			glUniformMatrix4fv(glGetUniformLocation(App->program->program,
				"view"), 1, GL_TRUE, &view[0][0]);
			glUniformMatrix4fv(glGetUniformLocation(App->program->program,
				"proj"), 1, GL_TRUE, &App->camera->frustum.ProjectionMatrix()[0][0]);
			glUniform1i(glGetUniformLocation(App->program->program, "useColor"), 1);
			glUniform3f(glGetUniformLocation(App->program->program, "colorU"), .2f, .2f, .2f);
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

			glUniform3f(glGetUniformLocation(App->program->program, "colorU"), 1.f, 0.f, 0.f); // x Axis
			glBegin(GL_LINES);
			glVertex3f(0.f, 0.1f, 0.f);
			glVertex3f(d, 0.1f, 0.f);
			glEnd();

			glUniform3f(glGetUniformLocation(App->program->program, "colorU"), 0.f, 0.f, 1.f); // z Axis
			glBegin(GL_LINES);
			glVertex3f(0.f, 0.1f, d);
			glVertex3f(0.f, 0.1f, 0.f);
			glEnd();

			glUniform3f(glGetUniformLocation(App->program->program, "colorU"), 0.f, 1.f, 0.f); // z Axis
			glBegin(GL_LINES);
			glVertex3f(0.f, 0.1f, 0.f);
			glVertex3f(0.f, d, 0.f);
			glEnd();

		}
		App->program->StopUseProgram();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	else // if on pre was 0 and still 0 something went wrong. Abort
	{
		LOG("Unexpected framebuffer status in Module Framebuffer. Aborting.");
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
	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleFrameBuffer::CleanUp()
{
	LOG("Cleaning Module Framebuffer");
	glDeleteFramebuffers(1, &framebuffer);
	glDeleteTextures(1, &texColorBuffer);
	LOG("Cleaning Module Framebuffer. Done");
	return true;
}

bool ModuleFrameBuffer::RecalcFrameBufferTexture()
{
	bool ret = true;
	
	glViewport(0, 0, viewPortWidth, viewPortHeight);
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

	return ret;
}

