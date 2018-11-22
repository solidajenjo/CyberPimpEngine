#ifndef _MATERIAL_IMPORTER_H
#define _MATERIAL_IMPORTER_H

#include <string>

struct aiMaterial;

class MaterialImporter
{
public:

	bool Import(const char* file, const char* path, std::string& output_file);	
	bool Import(const aiMaterial * material);
	bool Load(const char* exported_file);
};

#endif 