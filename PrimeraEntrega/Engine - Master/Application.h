#pragma once

#include<list>
#include "Globals.h"
#include "Module.h"

class ModuleRender;
class ModuleWindow;
class ModuleTextures;
class ModuleInput;
class ModuleRenderExercise;
class ModuleCamera;
class ModulePrograms;
class ModuleModelLoader;
class ModuleImGui;
class ModuleFileSystem;
class ModuleResources;

class Application
{
public:

	Application();
	~Application();

	bool Init();
	update_status Update();
	bool CleanUp();

	const char * getAppName() const;

	void setAppName(const char * name);

	int getFramerateCap() const;

	void setFramerateCap(int cap);

public:
	ModuleRender* renderer = nullptr;
	ModuleWindow* window = nullptr;
	ModuleTextures* textures = nullptr;
	ModuleInput* input = nullptr;
    ModuleRenderExercise* exercise = nullptr;
	ModuleCamera* camera = nullptr;
	ModulePrograms* program = nullptr;
	ModuleModelLoader* model = nullptr;
	ModuleImGui* imgui = nullptr;
	ModuleFileSystem* fs = nullptr;
	ModuleResources* resources = nullptr;

	std::string appName = "";

	int framerateCap = 60;

private:

	std::list<Module*> modules;

};

extern Application* App;
