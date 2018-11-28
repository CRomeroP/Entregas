#pragma once
#include "Module.h"
#include "Globals.h"
#include "Math/float4x4.h"

struct SDL_Texture;
struct SDL_Renderer;
struct SDL_Rect;

class ModuleRender : public Module
{
public:
	ModuleRender();
	~ModuleRender();

	bool Init();
	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();
	void WindowResized(unsigned width, unsigned height);

public:
	math::float4x4 model;
	void* context;

	unsigned int id = 0;
};
