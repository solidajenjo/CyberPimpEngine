#ifndef _GLOBALS_H
#define _GLOBALS_H

#include <windows.h>
#include <stdio.h>

#define LOG(format, ...) log(__FILE__, __LINE__, format, __VA_ARGS__)
#define RELEASE(p) { \
	if (p != nullptr) \
	{ \
		delete p; \
		p = nullptr; \
	} \
}


void log(const char file[], int line, const char* format, ...);

enum update_status
{
	UPDATE_CONTINUE = 1,
	UPDATE_STOP,
	UPDATE_ERROR
};

// Configuration -----------
#define FULLSCREEN false
#define RESIZABLE_WINDOW true
#define TITLE "Draconis Engine"
#define OPENGL_VERSION "#version 330"

#endif