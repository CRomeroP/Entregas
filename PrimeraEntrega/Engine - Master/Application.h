#pragma once

#include<list>
#include "Globals.h"
#include "Module.h"
#include "Event.h"
#include "MathGeoLib/include/Algorithm/Random/LCG.h"

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
class ModuleScene;

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

	LCG& Random();

	void BroadcastEvent(const Event& event);

	const char* GetAppName() const;
	void SetAppName(const char* name);
	const char* GetOrganizationName() const;
	void SetOrganizationName(const char* name);

public:

	LCG* random = nullptr;

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
	ModuleScene* level = nullptr;

	std::string appName = "";
	std::string organization;

	int framerateCap = 60;

private:

	std::list<Module*> modules;

};

extern Application* App;
