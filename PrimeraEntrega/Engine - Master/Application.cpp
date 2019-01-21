#pragma once
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "ModuleTextures.h"
#include "ModuleInput.h"
#include "ModuleRenderExercise.h"
#include "ModulePrograms.h"
#include "ModuleCamera.h"
#include "ModuleModelLoader.h"
#include "ModuleImGui.h"
#include "ModuleFileSystem.h"
#include "ModuleResources.h"
#include "ModuleScene.h"

using namespace std;

Application::Application()
{

	random = new math::LCG();
	// Order matters: they will Init/start/update in this order
	modules.push_back(window = new ModuleWindow());
	modules.push_back(camera = new ModuleCamera());
	modules.push_back(input = new ModuleInput());
	modules.push_back(fs = new ModuleFileSystem());
	modules.push_back(level = new ModuleScene());
	modules.push_back(resources = new ModuleResources());
	modules.push_back(textures = new ModuleTextures());
	modules.push_back(renderer = new ModuleRender());
	modules.push_back(model = new ModuleModelLoader());
	modules.push_back(program = new ModulePrograms());
	modules.push_back(imgui = new ModuleImGui());
	
}

Application::~Application()
{
	for(list<Module*>::iterator it = modules.begin(); it != modules.end(); ++it)
    {
        delete *it;
    }
}

bool Application::Init()
{
	bool ret = true;

	for(list<Module*>::iterator it = modules.begin(); it != modules.end() && ret; ++it)
		ret = (*it)->Init();

	return ret;
}

update_status Application::Update()
{
	update_status ret = UPDATE_CONTINUE;

	for(list<Module*>::iterator it = modules.begin(); it != modules.end() && ret == UPDATE_CONTINUE; ++it)
		ret = (*it)->PreUpdate();

	for(list<Module*>::iterator it = modules.begin(); it != modules.end() && ret == UPDATE_CONTINUE; ++it)
		ret = (*it)->Update();

	for(list<Module*>::iterator it = modules.begin(); it != modules.end() && ret == UPDATE_CONTINUE; ++it)
		ret = (*it)->PostUpdate();

	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;

	for(list<Module*>::reverse_iterator it = modules.rbegin(); it != modules.rend() && ret; ++it)
		ret = (*it)->CleanUp();

	return ret;
}

const char* Application::getAppName() const
{
	return appName.c_str();
}

void Application::setAppName(const char * name)
{
	if (name != nullptr && name != appName)
	{
		appName = name;
		SDL_SetWindowTitle(window->window, name);
	}
}

int Application::getFramerateCap() const
{
	return framerateCap;
}

void Application::setFramerateCap(int cap)
{
	framerateCap = cap;
}

LCG & Application::Random()
{
	return *random;
}

const char* Application::GetAppName() const
{
	return appName.c_str();
}

// ---------------------------------------------
void Application::SetAppName(const char * name)
{
	if (name != nullptr && name != appName)
	{
		appName = name;
		window->SetTitle(name);
		// TODO: Filesystem should adjust its writing folder
	}
}

const char* Application::GetOrganizationName() const
{
	return organization.c_str();
}

void Application::SetOrganizationName(const char * name)
{
	if (name != nullptr && name != organization)
	{
		organization= name;
		// TODO: Filesystem should adjust its writing folder
	}
}

void Application::BroadcastEvent(const Event& event)
{
	for (list<Module*>::iterator it = modules.begin(); it != modules.end(); ++it)
		(*it)->EventResolve(event);
}