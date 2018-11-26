#ifndef _SUB_MODULE_FILE_INSPECTOR_H
#define _SUB_MODULE_FILE_INSPECTOR_H

#include "SubModuleEditor.h"
#include "rapidjson-1.1.0/include/rapidjson/document.h"
#include <string>
#include <list>

class SubModuleEditorFileInspector : public SubModuleEditor
{
public:
	SubModuleEditorFileInspector(const std::string& editorModuleName);

	void Show();

	std::string runningPath = "";
	std::string path = "Library";
	std::string fileSystemRegistry = "Library/System/FileSystemRegistry.json";

	std::list<std::string> textureFormats;

	rapidjson::Document document;
};

#endif