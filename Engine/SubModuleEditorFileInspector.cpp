#include "SubModuleEditorFileInspector.h"
#include "Application.h"
#include <fstream>
#include <iostream>
#include <filesystem>
#include <chrono>
#include <ctime>
#include "imgui/imgui.h"
#include "SDL/include/SDL_rwops.h"
#include "rapidjson-1.1.0/include/rapidjson/stringbuffer.h"
#include "rapidjson-1.1.0/include/rapidjson/prettywriter.h"

using namespace ImGui; 
using namespace std::filesystem;
using namespace rapidjson;

SubModuleEditorFileInspector::SubModuleEditorFileInspector(const std::string & editorModuleName) : SubModuleEditor(editorModuleName)
{
	runningPath = std::filesystem::absolute(path).string();
	document = Document(kObjectType);
	SDL_RWops *rw = SDL_RWFromFile(fileSystemRegistry.c_str(), "r");	
	if (rw == nullptr)
	{				
		SDL_RWops *rw = SDL_RWFromFile(fileSystemRegistry.c_str(), "w");
		if (rw == nullptr)
		{
			LOG("Couldn't create file system registry. %s", SDL_GetError());			
		}
		
		SDL_RWclose(rw);
	}
	else 
	{
		unsigned docSize = SDL_RWsize(rw);		
		char* docContent = (char*)malloc(docSize);		
		if (SDL_RWread(rw, docContent, docSize, 1))
		{			
			if (document.Parse(docContent, docSize).HasParseError())
			{
				LOG("Error parsing file system registry -> %d", document.GetParseError());
			}
		}
		free(docContent);
		SDL_RWclose(rw);
	}	
}

void SubModuleEditorFileInspector::Show()
{
	if (enabled)
	{	
		ImGui::Begin(editorModuleName.data(), &enabled);
		
		if (runningPath != absolute(path).string())
		{
			if (Button("..")) {
				path = absolute(path).parent_path().string();
			}
		}
		for (std::filesystem::path pathIt : directory_iterator(path))
		{
			std::string file = relative(pathIt).string();
			PushID(file.c_str());
			if (std::filesystem::is_directory(pathIt))
			{	
				PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor(0.3f, 0.3f, 0.3f));
				PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor(0.3f, 0.3f, 0.3f));
				if (Button(file.c_str()))
				{
					path = absolute(pathIt).string();
				}
				PopStyleColor();
				PopStyleColor();
			}
			else {		
				if (document.FindMember(file.c_str()) == document.MemberEnd())
				{					
					char fileName[4096];
					sprintf(&fileName[0], "%s", file.c_str());
					rapidjson::Value fileNameV(&fileName[0], document.GetAllocator());
					document.AddMember(fileNameV, "Imported", document.GetAllocator());
					StringBuffer buffer;
					Writer<StringBuffer> writer(buffer);
					document.Accept(writer);
					document.Parse(buffer.GetString());
					LOG("Importing new detected file %s.", file.c_str());
					//update registry
					SDL_RWops *rw = SDL_RWFromFile(fileSystemRegistry.c_str(), "w");
					if (rw == nullptr)
					{
						LOG("Couldn't create file system registry. %s", SDL_GetError());
					}
					else
					{
						SDL_RWwrite(rw, buffer.GetString(), strlen(buffer.GetString()), 1);
						SDL_RWclose(rw);
					}
				}
				else {
					std::string status = std::string(document[file.c_str()].GetString());
					if (status == "Imported")
					{
						PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor(0.108f, 0.5f, 0.1f));
						PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor(0.108f, 0.5f, 0.1f));						
					}
					else
					{
						PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor(0.9f, 0.1f, 0.1f));
						PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor(0.9f, 0.1f, 0.1f));						
					}
					Button(file.c_str());
					if (BeginPopupContextItem("item context menu"))
					{
						if (Button("Re-import"))
						{
							LOG("Reimporting %s", file.c_str());
							CloseCurrentPopup();
						}						
						EndPopup();
					}
					PopStyleColor();
					PopStyleColor();
				}				
			}			
			PopID();
		}		
		ImGui::End();
	}
}

