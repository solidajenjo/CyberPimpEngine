#ifndef _LOG_CPP
#define _LOG_CPP
#include "Globals.h"
#include "Application.h"
#include "imgui/imgui.h"

void log(const char file[], int line, const char* format, ...)
{
	static char tmp_string[4096];
	static char tmp_console[4096];

	static va_list  ap;

	// Construct the string from variable arguments
	va_start(ap, format);
	vsprintf_s(tmp_string, 4096, format, ap);
	va_end(ap);

	if (App != nullptr && ImGui::GetCurrentContext() != nullptr && App->consoleBuffer != nullptr)
	{
		sprintf_s(tmp_console, 4096, "\nEngine:%s", tmp_string);
		App->consoleBuffer->appendf(tmp_console);
	}	
}
#endif