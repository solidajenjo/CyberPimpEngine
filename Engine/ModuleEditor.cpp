#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include "GL/glew.h"
#include "ModuleEditor.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "Application.h"
#include "SubModuleEditorMenu.h"
#include "SubModuleEditorConsole.h"
#include "SubModuleEditorWorldInspector.h"
#include "SubModuleEditorViewPort.h"
#include "SubModuleEditorConfig.h"
#include "SubModuleEditorCamera.h"

#include "SDL.h"


ModuleEditor::ModuleEditor()
{
}

ModuleEditor::~ModuleEditor()
{
}

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
	subModules.push_back(viewPort = new SubModuleEditorViewPort("Scene"));
	subModules.push_back(config = new SubModuleEditorConfig("Config"));
	subModules.push_back(camera = new SubModuleEditorCamera("Camera"));
	App->consoleBuffer = new ImGuiTextBuffer();

	return true;
}

update_status ModuleEditor::PreUpdate()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();
	
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	ImGuiDockNodeFlags opt_flags = ImGuiDockNodeFlags_None;
	window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowViewport(viewport->ID);

	ImGui::Begin("DockSpace Demo", &bDock, window_flags);
	ImGuiID dockspace_id = ImGui::GetID("MyDockspace");
	ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), opt_flags);
	ImGui::End();

	return UPDATE_CONTINUE;
}

update_status ModuleEditor::Update()
{
	for (std::vector<SubModuleEditor*>::iterator it = subModules.begin(); it != subModules.end(); ++it)
		(*it)->Show();
	return UPDATE_CONTINUE;
}

update_status ModuleEditor::PostUpdate()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	return UPDATE_CONTINUE;
}

bool ModuleEditor::CleanUp()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	delete menu;
	return true;
}
