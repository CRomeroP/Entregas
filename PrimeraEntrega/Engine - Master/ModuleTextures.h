#pragma once
#include<list>
#include "Module.h"
#include "Globals.h"
#include "GL/glew.h"

#include "DevIL\include\IL\il.h"
#include "DevIL\include\IL\ilut.h"

struct SDL_Texture;

class ModuleTextures : public Module
{
public:
	ModuleTextures();
	~ModuleTextures();

	bool Init();
	bool CleanUp();

	GLuint loadImage(const char* theFileName);

private:
	ILuint imageID;				// Create an image ID as a ULuint

	GLuint textureID;			// Create a texture ID as a GLuint

	ILboolean success;			// Create a flag to keep track of success/failure

	ILenum error;				// Create a flag to keep track of the IL error state
};