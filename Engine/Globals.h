#pragma once
#include <windows.h>
#include <stdio.h>

#define LOG(format, ...) log(__FILE__, __LINE__, format, __VA_ARGS__);

void log(const char file[], int line, const char* format, ...);

enum update_status
{
	UPDATE_CONTINUE = 1,
	UPDATE_STOP,
	UPDATE_ERROR
};

// Configuration -----------
#define FULLSCREEN false
#define WINDOWED_FULLSCREEN true
#define RESIZABLE_WINDOW true
#define VSYNC true
#define TITLE "CyberPimp Engine"
#define OPENGL_VERSION "#version 330"