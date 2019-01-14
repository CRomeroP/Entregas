#pragma once
#include "Globals.h"
#include "Module.h"
#include <vector>
#include <list>
#include <SDL.h>

#define FPS_LOG_SIZE 100
#define MEMORY_LOG_SIZE 100

class Panel;
class PanelConfiguration;
class PanelConsole;
class PanelHardware;
class PanelGOTree;

class ModuleImGui :
	public Module
{
public:
	ModuleImGui();
	~ModuleImGui();

	bool Init();

	bool Start();
	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();
	void consoleDraw();
	void hardwareDraw();
	void configDraw();
	void addMemory(float memory);
	void Draw();

public:
	bool active = true;

	std::list<Panel*> panels;

	PanelConsole* console = nullptr;
	PanelConfiguration* configuration = nullptr;
	PanelHardware* hardware = nullptr;
	PanelGOTree* GOTree = nullptr;

private:
	std::vector<float> fps_log;
	std::vector<float> game_fps_log;
	std::vector<float> ms_log;
	std::vector<float> game_ms_log;
	std::vector<float> memory_log;

	bool fullscreen = false;
	bool resizable = false;
	bool borderless = false;
	bool full_desktop = false;
	bool init = true;

	float brightness = 0.0f; 
	
	SDL_DisplayMode mode;

	int prevWidth = 0;
	int prevHeight = 0;
};

