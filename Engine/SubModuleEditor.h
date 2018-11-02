#ifndef _SUB_MODULE_EDITOR_H
#define _SUB_MODULE_EDITOR_H

class SubModuleEditor
{
public:
	SubModuleEditor(char* editorModuleName) : editorModuleName(editorModuleName) {};
	~SubModuleEditor() {};

	virtual void Show() = 0 ;

	bool enabled = true;
	char* editorModuleName;
};

#endif;