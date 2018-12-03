#include "ModuleFileSystem.h"
#include "SDL/include/SDL_filesystem.h"

ModuleFileSystem::ModuleFileSystem()
{
	char* p = SDL_GetPrefPath("DraconisEngine", "");
	prefPath = std::string(p);
	SDL_free(p);
}

bool ModuleFileSystem::Write(const std::string & path, void * data, unsigned size) const
{
	return false;
}

bool ModuleFileSystem::Exists(const std::string & path) const
{
	return false;
}

unsigned ModuleFileSystem::Size(const std::string & path) const
{
	return 0;
}

bool ModuleFileSystem::CreateDir(const std::string & path) const
{
	return false;
}

bool ModuleFileSystem::IsDir(const std::string & path) const
{
	return false;
}

bool ModuleFileSystem::Delete(const std::string & path) const
{
	return false;
}

