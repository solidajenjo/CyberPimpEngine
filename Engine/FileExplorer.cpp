#include "Application.h"
#include "imgui/imgui.h"
#include "ModuleFileSystem.h"
#include "FileExplorer.h"

void FileExplorer::Reset()
{
	path = App->fileSystem->userDataMountPoint;
	rawPath = "";
	while (!pathStack.empty())
		pathStack.pop();
}

bool FileExplorer::Open()
{
	if (ImGui::BeginPopupModal(title))
	{		
		if (ImGui::Button("Accept", ImVec2(100,20))) 
		{
			ImGui::CloseCurrentPopup();
			ImGui::EndPopup();
			path = path + "/" + std::string(filename);
			rawPath = rawPath + "/" + std::string(filename);
			return true;
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel",ImVec2(100, 20)))
		{
			ImGui::CloseCurrentPopup();
			ImGui::EndPopup();
			return false;
		}
		ImGui::Text((path + "/").c_str());
		ImGui::InputText("File Name", &filename[0], sizeof(filename));				
		std::vector<std::string> files;
		std::vector<std::string> dirs;
		ImGui::Separator();
		ImGui::BeginChild("Files");
		App->fileSystem->GetContentList(path, files, dirs);
		ImVec2 buttonSize = ImGui::GetWindowContentRegionMax();
		buttonSize.y = 20;
		ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(.0f, 0.5f));
		if (pathStack.size() > 0)
		{
			if (ImGui::Button(".."))
			{
				path = pathStack.top();
				pathStack.pop();
			}
		}
		for each (std::string s in dirs)
		{			
			if (ImGui::Button(s.c_str(), buttonSize))
			{
				pathStack.push(path);
				path = path + "/" + s;
				rawPath = rawPath + "/" + s;
			}
		}
		for each (std::string s in files)
		{
			ImGui::Text(s.c_str());
			if (ImGui::IsItemClicked())
				sprintf_s(filename, s.c_str());
		}
		ImGui::PopStyleVar();
		ImGui::EndChild();
		ImGui::EndPopup();
		
	}
	return false;
}
