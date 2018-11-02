#include "SubModuleEditorViewPort.h"
#include "Application.h"
#include "ModuleCamera.h"
#include "ModuleProgram.h"
#include "ModuleTextures.h"
#include "ModuleWindow.h"
#include "imgui.h"


SubModuleEditorViewPort::SubModuleEditorViewPort(char* editorModuleName) : SubModuleEditor(editorModuleName)
{	

}


SubModuleEditorViewPort::~SubModuleEditorViewPort()
{
	glDeleteFramebuffers(1, &framebuffer);
}

void SubModuleEditorViewPort::Show()
{
	if (enabled)
	{	
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

		//DRAW SOMETHING
		glBegin(GL_TRIANGLES);
		glVertex3f(-1, -1, 0);
		glVertex3f(1, -1, 0);
		glVertex3f(0, 1, 0);
		glEnd();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);	

		//TODO::Flip texture
		ImGui::Begin("viewport");
		{
			ImGui::Image((void*)(intptr_t)texColorBuffer, ImGui::GetWindowContentRegionMax());
			ImGui::End();			
		}
		
	}
}

void SubModuleEditorViewPort::Init()
{
	LOG("Init editor viewport.")
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	// generate texture
	glGenTextures(1, &texColorBuffer);
	glBindTexture(GL_TEXTURE_2D, texColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, App->window->screenWidth, App->window->screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// attach it to currently bound framebuffer object
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);
	

	glBindTexture(GL_TEXTURE_2D, 0);
}
