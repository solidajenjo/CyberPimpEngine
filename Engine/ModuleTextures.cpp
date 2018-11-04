#include "GL/glew.h"
#include "Globals.h"
#include "Application.h"
#include "ModuleTextures.h"
#include "il.h"
#include "ilut.h"
#include "ilu.h"


using namespace std;

ModuleTextures::ModuleTextures()
{
	ilInit();
	iluInit();
	ilutInit();
}

// Destructor
ModuleTextures::~ModuleTextures()
{
}

// Called before render is available
bool ModuleTextures::Init()
{
	bool ret = true;

	return ret;
}

// Called before quitting
bool ModuleTextures::CleanUp()
{
	return true;
}

// Load new texture from file path
unsigned ModuleTextures::Load(const char* path)
{
	ILuint imageId;
	ilGenImages(1, &imageId);
	ilBindImage(imageId);

	if (ilLoadImage(path))
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

		/*if (mipmaps)
		{*/
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glGenerateMipmap(GL_TEXTURE_2D);
		/*}
		else
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		}
		*/
		ilDeleteImages(1, &imageId);

		glBindTexture(GL_TEXTURE_2D, 0);

		return textureId;
	}

	return 0;
}


