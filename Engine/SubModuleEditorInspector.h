#ifndef _SUB_MODULE_EDITOR_INSPECTOR_H
#define _SUB_MODULE_EDITOR_INSPECTOR_H

#include "SubModuleEditor.h"
#include <vector>
#include <string>

class SubModuleEditorInspector : public SubModuleEditor
{
public:

	SubModuleEditorInspector::SubModuleEditorInspector(const std::string & editorModuleName) : SubModuleEditor(editorModuleName) {}
	void Show();

private:

	int selectedNewComponent;
};

#endif