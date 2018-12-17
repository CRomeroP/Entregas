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

	bool vsync = false; 
	bool Depth_Test = false;
	bool Cull_Face = false;
	bool Lighting = false;
	bool Color_Material = false;
	bool Texture_2D = false;
	bool Wireframe = false;
	bool Normals = false;
	bool Faces = false;
	bool ShowBB = false;
	bool ShowQT = false;
	bool Frustum_Culling = true;
	bool ShowClickRay = false;

	unsigned int id = 0;
};
