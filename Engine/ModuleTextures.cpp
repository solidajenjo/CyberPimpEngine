#include "glew-2.1.0/include/GL/glew.h"
#include "Globals.h"
#include "Application.h"
#include "ModuleTextures.h"
#include "ModuleFileSystem.h"
#include "ComponentMap.h"
#include "DevIL/include/IL/il.h"
#include "DevIL/include/IL/ilut.h"
#include "DevIL/include/IL/ilu.h"
#include <algorithm>

using namespace std;

ModuleTextures::ModuleTextures()
{
	ilInit();
	iluInit();
	ilutInit();
}

bool ModuleTextures::Init()
{
	char fallbackImageWhite[4] = { GLubyte(255), GLubyte(255), GLubyte(255), GLubyte(255) };
	
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &whiteFallback);
	glBindTexture(GL_TEXTURE_2D, whiteFallback);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, fallbackImageWhite);

	char fallbackImageBlack[4] = { GLubyte(0), GLubyte(0), GLubyte(0), GLubyte(0) };

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &blackFallback);
	glBindTexture(GL_TEXTURE_2D, blackFallback);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, fallbackImageBlack);

	return (blackFallback != 0u && whiteFallback != 0u);
}

// Called before quitting
bool ModuleTextures::CleanUp() //can be called to reset stored textures
{
	LOG("Cleaning Module Textures");
	for (std::list<unsigned>::iterator it = textures.begin(); it != textures.end(); ++it)
		glDeleteTextures(1, &(*it)); //clean textures
	textures.resize(0);
	LOG("Cleaning Module Textures. Done");
	return true;
}

// Load new texture from file path
unsigned ModuleTextures::Load(const std::string& path, bool useMipMaps)
{
	LOG("Loading texture -> %s", path.c_str());
	ILuint imageId;
	ilGenImages(1, &imageId);
	ilBindImage(imageId);
	unsigned fileSize = App->fileSystem->Size(path);
	if (fileSize > 0)
	{
		char* data = new char[fileSize];
		if (App->fileSystem->Read(path, data, fileSize))
		{
			if (ilLoadL(IL_DDS, data, fileSize))
			{
				GLuint textureId = 0;
				glGenTextures(1, &textureId);

				glBindTexture(GL_TEXTURE_2D, textureId);

				ILinfo ImageInfo;
				iluGetImageInfo(&ImageInfo);
				if (ImageInfo.Origin == IL_ORIGIN_UPPER_LEFT)
				{
					iluFlipImage();
				}

				int channels = ilGetInteger(IL_IMAGE_CHANNELS);
				if (channels == 3)
				{
					ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE);
				}
				else if (channels == 4)
				{
					ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
				}

				ILubyte* imageData = ilGetData();
				int width = ilGetInteger(IL_IMAGE_WIDTH);
				int height = ilGetInteger(IL_IMAGE_HEIGHT);

				glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_FORMAT), width, height, 0, ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE, imageData);
				
				if (useMipMaps)
				{
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
					glGenerateMipmap(GL_TEXTURE_2D);
				}
				else
				{
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				}
				ilDeleteImages(1, &imageId);

				glBindTexture(GL_TEXTURE_2D, 0);
				textures.push_back(textureId);
				delete[] data;
				return textureId;
			}
		}
		delete[] data;
	}
	
	LOG("Error loading texture.");
	return 0;
}

void ModuleTextures::UnLoad(unsigned texNum)
{
	std::list<unsigned>::iterator it = std::find(textures.begin(), textures.end(), texNum);
	glDeleteTextures(1, &texNum);
	if (it != textures.end()) 
	{
		textures.erase(it);
	}	
}


