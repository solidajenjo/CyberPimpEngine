#include "ModuleFileSystem.h"
#include "SDL/include/SDL_filesystem.h"

bool ModuleFileSystem::Init()
{
	char* p = SDL_GetPrefPath("DraconisEngine", "");
	prefPath = std::string(p);
	SDL_free(p);
	PHYSFS_init(prefPath.c_str());
	PHYSFS_setWriteDir(".");
	PHYSFS_mount(".", "", 0);
	return true;
}

bool ModuleFileSystem::CleanUp()
{
	PHYSFS_deinit();
	return true;
}

bool ModuleFileSystem::Write(const std::string & path, void * data, unsigned size) const
{
	PHYSFS_File* file = PHYSFS_openWrite(path.c_str());
	if (file == nullptr)
	{
		LOG("Error writing in %s -> %s", path.c_str(), PHYSFS_getLastError());
		return false;
	}
	unsigned k = PHYSFS_writeBytes(file, data, size);
	if (k == size)
	{
		PHYSFS_close(file);
		return true;
	}
	LOG("Error writing in %s -> %s", path.c_str(), PHYSFS_getLastError());
	PHYSFS_close(file);
	return false;
}

bool ModuleFileSystem::Read(const std::string& path, void* data, unsigned size) const
{
	PHYSFS_File* file = PHYSFS_openRead(path.c_str());
	if (file == nullptr)
	{
		LOG("Error reading %s -> %s", path.c_str(), PHYSFS_getLastError());
		return false;
	}

	if (PHYSFS_readBytes(file, data, size) == size)
	{
		PHYSFS_close(file);
		return true;
	}
	PHYSFS_close(file);
	return false;
}

bool ModuleFileSystem::Exists(const std::string & path) const
{
	return false;
}

unsigned ModuleFileSystem::Size(const std::string & path) const
{
	PHYSFS_File* file = PHYSFS_openRead(path.c_str());
	if (file == nullptr)
	{
		LOG("Error reading %s -> %s", path.c_str(), PHYSFS_getLastError());
		return 0;
	}
	return PHYSFS_fileLength(file);	
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

