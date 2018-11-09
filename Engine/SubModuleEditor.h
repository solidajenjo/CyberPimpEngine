#ifndef _SUB_MODULE_EDITOR_H
#define _SUB_MODULE_EDITOR_H
#include <string>

class SubModuleEditor
{
public:
	SubModuleEditor(std::string editorModuleName) : editorModuleName(editorModuleName) {};
	virtual ~SubModuleEditor() {};

	virtual void Show() = 0 ; //No const posible -> ImGui::Begin modifies enabled;

	bool enabled = true;
	std::string editorModuleName;
};

#endif;