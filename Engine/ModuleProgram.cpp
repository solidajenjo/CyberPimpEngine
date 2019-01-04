#include "ModuleProgram.h"
#include "glew-2.1.0/include/GL/glew.h"
#include <vector>


bool ModuleProgram::Init()
{
	bool ret = true;
	ret = ret && Compile(ModuleProgram::Shaders::DEFAULT, "default.vs", "default.fs");
	ret = ret && Compile(ModuleProgram::Shaders::FORWARD_RENDERING, "ForwardRenderingShader.vs", "ForwardRenderingShader.fs");
	ret = ret && Compile(ModuleProgram::Shaders::NORMAL_INSPECTOR, "NormalInspectorShader.vs", "NormalInspectorShader.fs");
	return ret;
}

bool ModuleProgram::Compile(Shaders shaderType, std::string vsName, std::string fsName)
{
	unsigned program;
	LOG("Shader Program Creation");
	program = glCreateProgram();
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	char* vSource = readFile(vsName.c_str());
	glShaderSource(vs, 1, &vSource, 0);
	glCompileShader(vs);
	free(vSource);
	GLint isCompiled = 0;
	glGetShaderiv(vs, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(vs, GL_INFO_LOG_LENGTH, &maxLength);
		GLchar* infoLog = (GLchar*) malloc(sizeof(char) * (maxLength + 1));
		glGetShaderInfoLog(vs, maxLength, &maxLength, &infoLog[0]);
		glDeleteShader(vs);
		LOG("Vertex shader compilation error: %s", infoLog);
		free(infoLog);
		return false;
	}
	

	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	char* fSource = readFile(fsName.c_str());
	glShaderSource(fs, 1, &fSource, 0);
	glCompileShader(fs);
	free(fSource);
	isCompiled = 0;
	glGetShaderiv(fs, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(fs, GL_INFO_LOG_LENGTH, &maxLength);
		GLchar* infoLog = (GLchar*)malloc(sizeof(char) * (maxLength + 1));
		glGetShaderInfoLog(fs, maxLength, &maxLength, &infoLog[0]);
		glDeleteShader(fs);
		LOG("Fragment shader compilation error: %s", infoLog);
		free(infoLog);
		return false;
	}

	glAttachShader(program, vs);
	glAttachShader(program, fs);

	glLinkProgram(program);

	GLint isLinked = 0;
	glGetShaderiv(fs, GL_COMPILE_STATUS, &isLinked);
	if (isLinked == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
		GLchar* infoLog = (GLchar*)malloc(sizeof(char) * (maxLength + 1));
		glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);
		glDeleteProgram(program);
		glDeleteShader(vs);
		glDeleteShader(fs);
		LOG("Program Linking error: %s", infoLog);
		free(infoLog);
		return false;
	}


	glDeleteShader(vs);
	glDeleteShader(fs);
	LOG("Shader Program Created");

	switch (shaderType)
	{
	case Shaders::DEFAULT:
		default = new unsigned;
		*default = program;
		break;
	case Shaders::FORWARD_RENDERING:
		forwardRenderingProgram = new unsigned;
		*forwardRenderingProgram = program;
		break;
	case Shaders::NORMAL_INSPECTOR:
		normalInspectorProgram = new unsigned;
		*normalInspectorProgram = program;
	}
	return true;
}

void ModuleProgram::StopUseProgram() const
{
	glUseProgram(0);
}

bool ModuleProgram::CleanUp()
{
	LOG("Cleaning Module Program");
	glDeleteProgram(*default);
	glDeleteProgram(*forwardRenderingProgram);
	RELEASE(default);
	RELEASE(forwardRenderingProgram);
	LOG("Cleaning Module Program. Done");
	return true;
}

char* ModuleProgram::readFile(const std::string& name)
{
	char* data = nullptr;
	FILE* file = nullptr;
	fopen_s(&file, name.c_str(), "rb");
	if (file)
	{
		fseek(file, 0, SEEK_END);
		int size = ftell(file);
		rewind(file);
		data = (char*)malloc(size + 1);
		fread(data, 1, size, file);
		data[size] = 0;
		fclose(file);
	}
	return data;
}
