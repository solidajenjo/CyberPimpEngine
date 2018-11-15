#include "glew-2.1.0/include/GL/glew.h"
#include "Globals.h"
#include "Application.h"
#include "ModuleTextures.h"
#include "DevIL/include/IL/il.h"
#include "DevIL/include/IL/ilut.h"
#include "DevIL/include/IL/ilu.h"


using namespace std;

ModuleTextures::ModuleTextures()
{
	ilInit();
	iluInit();
	ilutInit();
}


// Called before quitting
bool ModuleTextures::CleanUp() //can be called to reset stored textures
{
	LOG("Cleaning Module Textures");
	if (textures.size() > 0)
		glDeleteTextures(textures.size(), &textures[0]); //clean textures
	textures.resize(0);
	LOG("Cleaning Module Textures. Done");
	return true;
}

// Load new texture from file path
unsigned ModuleTextures::Load(const std::string& path)
{
	LOG("Loading texture -> %s", path.c_str());
	ILuint imageId;
	ilGenImages(1, &imageId);
	ilBindImage(imageId);

	if (ilLoadImage(path.c_str()))
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

		ILubyte* data = ilGetData();
		int width = ilGetInteger(IL_IMAGE_WIDTH);
		int height = ilGetInteger(IL_IMAGE_HEIGHT);

		glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_FORMAT), width, height, 0, ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE, data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glGenerateMipmap(GL_TEXTURE_2D);
	
		ilDeleteImages(1, &imageId);

		glBindTexture(GL_TEXTURE_2D, 0);
		textures.push_back(textureId);
		return textureId;
	}

	return 0;
}


