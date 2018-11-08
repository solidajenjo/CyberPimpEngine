#ifndef _SUB_MODULE_EDITOR_CONFIG_H
#define _SUB_MODULE_EDITOR_CONFIG_H

#include "SubModuleEditor.h"
#include <vector>

class SubModuleEditorConfig : public SubModuleEditor
{
public:
	SubModuleEditorConfig(char* editorModuleName) : SubModuleEditor(editorModuleName), f(0), time(50, 0), fps(50, 0),
		deltaTimeAcc(0), fpsAcc(0), maxFps(-10000.f), minFps(10000.f), maxDelta(-10000.f), minDelta(10000.f)
	{};
	~SubModuleEditorConfig() {};

	void Show();
	
//members

	unsigned f;
	std::vector<float> time;
	std::vector<float> fps;
	float deltaTimeAcc, fpsAcc, maxFps, minFps, maxDelta, minDelta;
};

#endif