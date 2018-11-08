#ifndef _SUB_MODULE_EDITOR_H
#define _SUB_MODULE_EDITOR_H

class SubModuleEditor
{
public:
	SubModuleEditor(char* editorModuleName) : editorModuleName(editorModuleName) {}; //TODO: No usar char*
	~SubModuleEditor() {};

	virtual void Show() = 0 ; //TODO: constante

	bool enabled = true;
	char* editorModuleName;
};

#endif;