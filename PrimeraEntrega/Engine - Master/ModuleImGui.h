#pragma once
#include "Globals.h"
#include "Module.h"

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
	void Draw();
};

