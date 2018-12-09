#ifndef _MODULE_FILESYSTEM_H
#define _MODULE_FILESYSTEM_H

#include "Module.h"
#include "physfs-3.0.1/src/physfs.h"
#include <string>

class ModuleFileSystem :
	public Module
{
public:

	bool Init() override;
	bool CleanUp() override;

	bool Write(const std::string &path, const void* data, unsigned size) const; //writes data to path
	bool Read(const std::string &path, void* data, unsigned size) const; //reads from path and allocates in data. NOTE: The caller should be responsible to clean it

	bool Exists(const std::string &path) const;

	unsigned Size(const std::string &path) const;

	bool CreateDir(const std::string &path) const;

	bool IsDir(const std::string &path) const;

	bool Delete(const std::string &path) const;
	
private:

	std::string prefPath;
};

#endif
