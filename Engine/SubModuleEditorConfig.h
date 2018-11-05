#ifndef _SUB_MODULE_EDITOR_CONFIG_H
#define _SUB_MODULE_EDITOR_CONFIG_H

#include "SubModuleEditor.h"
#include <list>

class SubModuleEditorConfig : public SubModuleEditor
{
public:
	SubModuleEditorConfig(char* editorModuleName) : SubModuleEditor(editorModuleName), fpsList(40, 0.f), lastMs(0),
		frameCounter(0), nextSecond(1000) {};
	~SubModuleEditorConfig() {};

	void Show();
	void lineListPlotter(const std::list<float> &valList);
//members

	std::list<float> fpsList;
	int lastMs, frameCounter, nextSecond;
};

#endif