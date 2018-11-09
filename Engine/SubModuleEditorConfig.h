#ifndef _SUB_MODULE_EDITOR_CONFIG_H
#define _SUB_MODULE_EDITOR_CONFIG_H

#define SUB_SAMPLE_TIME 500.f
#define SUB_SAMPLE_MULTIPLIER 1000.f / SUB_SAMPLE_TIME

#include "SubModuleEditor.h"
#include <vector>

class SubModuleEditorConfig : public SubModuleEditor
{
public:
	SubModuleEditorConfig(std::string editorModuleName) : SubModuleEditor(editorModuleName) {};

	void Show();
	
//members

	unsigned f = 0, frames = 0;
	float timer = SUB_SAMPLE_TIME;
	std::vector<float> time = std::vector<float>(50, 0.f);
	std::vector<float> fps = std::vector<float>(50, 0.f);
	float deltaTimeAcc = 0, 
		fpsAcc = 0, 
		maxFps = -10000.f, 
		minFps = 10000.f, 
		maxDelta = -10000.f, 
		minDelta = 10000.f,
		fpsAvgDivider = 1.f,
		fpsAvg = 0.f;
};

#endif