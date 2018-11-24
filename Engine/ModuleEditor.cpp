#include "imgui/imgui.h"
#include "imgui/examples/imgui_impl_sdl.h"
#include "imgui/examples/imgui_impl_opengl3.h"
#include "glew-2.1.0/include/GL/glew.h"
#include "ModuleEditor.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "ModuleScene.h"
#include "ModuleFrameBuffer.h"
#include "Application.h"
#include "SubModuleEditorMenu.h"
#include "SubModuleEditorConsole.h"
#include "SubModuleEditorWorldInspector.h"
#include "SubModuleEditorInspector.h"
#include "SubModuleEditorViewPort.h"
#include "SubModuleEditorGameViewPort.h"
#include "SubModuleEditorConfig.h"
#include "SubModuleEditorToolBar.h"
#include "SubModuleEditorFileInspector.h"
#include "GameObject.h"
#include "ModuleDebugDraw.h"
#include "ModuleEditorCamera.h"
#include "debugdraw.h"

bool ModuleEditor::Init()
{

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer->context);
	ImGui_ImplOpenGL3_Init(OPENGL_VERSION);
	
	ImGui::StyleColorsDark();
	
	subModules.push_back(menu = new SubModuleEditorMenu("Menu"));
	subModules.push_back(console = new SubModuleEditorConsole("Console"));
	subModules.push_back(worldInspector = new SubModuleEditorWorldInspector("World Inspector"));
	subModules.push_back(inspector = new SubModuleEditorInspector("Inspector"));
	subModules.push_back(viewPort = new SubModuleEditorViewPort("Viewport"));
	subModules.push_back(gameViewPort = new SubModuleEditorGameViewPort("Game Viewport"));
	subModules.push_back(config = new SubModuleEditorConfig("Config"));
	subModules.push_back(files = new SubModuleEditorFileInspector("Files"));
	App->consoleBuffer = new ImGuiTextBuffer();

	toolBar = new SubModuleEditorToolBar("ToolBar");
	textures = new ModuleTextures(); 

	return true;
}

update_status ModuleEditor::PreUpdate()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();
	
	toolBar->Show();

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	ImGuiDockNodeFlags opt_flags = ImGuiDockNodeFlags_None | ImGuiDockNodeFlags_PassthruDockspace;
	window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, toolBar->toolBarHeight));
	ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, viewport->Size.y * .95f/* - toolBar->toolBarHeight*/));
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::SetNextWindowBgAlpha(0.f);	
	ImGui::Begin("DockSpace", &bDock, window_flags);	
		if (backgroundTex == 0) //post glew initialization needed
		{
			backgroundTex = textures->Load("editorBackground.png");
			logo = textures->Load("editorHeaderLogo.png");
			checkersTex = textures->Load("checkers.png");			
			noCamTex = textures->Load("noCam.png");		
			App->scene->SetSkyBox();
		}
		float imageXPos = (viewport->Size.x / 2) - (viewport->Size.y / 2);
		ImVec2 cornerPos = ImGui::GetCursorPos();
		ImGui::SetCursorPosX(imageXPos);
		ImGui::Image((void*)(intptr_t)backgroundTex, ImVec2(viewport->Size.y - toolBar->toolBarHeight, viewport->Size.y * .95f - toolBar->toolBarHeight), ImVec2(0, 1), ImVec2(1, 0));
		ImGui::SetCursorPos(cornerPos);
		ImGuiID dockspace_id = ImGui::GetID("DockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.f, 0.f), opt_flags);
	ImGui::End();
	ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(500, 500));	
	App->frameBuffer->PreUpdate();
	return UPDATE_CONTINUE;
}

update_status ModuleEditor::Update()
{
	for (std::vector<SubModuleEditor*>::iterator it = subModules.begin(); it != subModules.end(); ++it)
	{
		(*it)->Show();
	}	
	if (gizmosEnabled)
	{
		dd::xzSquareGrid(-200.f, 200.f, 0.f, 1.f, dd::colors::DarkGray);
		App->debugDraw->Draw(&App->camera->editorCamera, App->frameBuffer->framebuffer, App->frameBuffer->viewPortWidth, App->frameBuffer->viewPortHeight);
	}
	return UPDATE_CONTINUE;
}

update_status ModuleEditor::PostUpdate()
{
	ImGui::PopStyleVar();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	App->frameBuffer->PostUpdate();
	return UPDATE_CONTINUE;
}

bool ModuleEditor::CleanUp()
{
	LOG("Cleaning Module Editor");
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
	for (std::vector<SubModuleEditor*>::iterator it = subModules.begin(); it != subModules.end(); ++it)
	{
		if (*it != nullptr)
			delete (*it);
	}

	RELEASE (toolBar);
	RELEASE (textures);
	LOG("Cleaning Module Editor. Done");
	return true;
}
