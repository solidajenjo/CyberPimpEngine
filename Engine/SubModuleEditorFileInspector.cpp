#include "SubModuleEditorFileInspector.h"
#include "Application.h"
#include <fstream>
#include <iostream>
#include <filesystem>
#include "imgui/imgui.h"
using namespace ImGui;

SubModuleEditorFileInspector::SubModuleEditorFileInspector(const std::string & editorModuleName) : SubModuleEditor(editorModuleName)
{
	runningPath = std::filesystem::absolute(path).string();
}

void SubModuleEditorFileInspector::Show()
{
	if (enabled)
	{	
		ImGui::Begin(editorModuleName.data(), &enabled);
		if (runningPath != std::filesystem::absolute(path).string())
		{
			if (Button("..")) {
				path = std::filesystem::absolute(path).parent_path().string();
			}
		}
		for (std::filesystem::path pathIt : std::filesystem::directory_iterator(path))
		{
			std::string file = std::filesystem::relative(pathIt).string();
			if (std::filesystem::is_directory(pathIt))
			{				
				if (Button(file.c_str()))
				{
					path = std::filesystem::absolute(pathIt).string();
				}
			}
			else {							
				Button(file.c_str());
			}			
			
		}
		ImGui::End();
	}
}

