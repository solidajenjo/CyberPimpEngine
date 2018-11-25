#include "SubModuleEditorFileInspector.h"
#include "Application.h"
#include <algorithm>
#include <filesystem>
#include "imgui/imgui.h"
#include "SDL/include/SDL_rwops.h"
#include "rapidjson-1.1.0/include/rapidjson/stringbuffer.h"
#include "rapidjson-1.1.0/include/rapidjson/prettywriter.h"
#include "SceneImporter.h"

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
	textureFormats.push_back("TGA");
	textureFormats.push_back("DDS");
	textureFormats.push_back("PNG");
	textureFormats.push_back("JPG");
	textureFormats.push_back("TIFF");
}

void SubModuleEditorFileInspector::Show()
{
	if (enabled)
	{			
		ImGui::Begin(editorModuleName.c_str(), &enabled);
		Text(absolute(path).string().c_str());
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
					LOG("Trying to import new detected file %s.", file.c_str());
					//try to import	
					unsigned i = file.length() - 1;
					std::string extension;
					bool imported = false;
					while (file[i--] != '.' && i > 0);
					for (unsigned j = i + 2; j < file.length(); ++j)
					{
						extension.push_back(toupper(file[j]));
					}
					if (extension == "FBX") //is a mesh
					{
						std::string upperPath = absolute(pathIt).string();
						for (unsigned i = 0u; i < upperPath.length(); ++i)
						{
							upperPath[i] = toupper(upperPath[i]);
						}
						SceneImporter si;
						imported = si.Import(upperPath);
					}
					else if (std::find(textureFormats.begin(), textureFormats.end(), extension) != textureFormats.end()) //is a texture format
					{

					}
					//register result					
					char fileName[4096];
					sprintf(&fileName[0], "%s", file.c_str());
					rapidjson::Value fileNameV(&fileName[0], document.GetAllocator());
					if (imported)
						document.AddMember(fileNameV, "Imported", document.GetAllocator());
					else
						document.AddMember(fileNameV, "Not Imported", document.GetAllocator());
					StringBuffer buffer;
					Writer<StringBuffer> writer(buffer);
					document.Accept(writer);
					document.Parse(buffer.GetString());					
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
					
					Button(pathIt.filename().string().c_str());
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
			ImGui::PopID();
		}			
		ImGui::End();
	}
}

