#ifndef __MODULE_H__
#define __MODULE_H__

#include "Globals.h"

class Application;
class Config;

class Module
{

private:
	bool enable;
	char name[30];

public:

	Module(const char* name, bool enabled = true) : enable(enabled)
	{
		strcpy_s(this->name, name);
	}

	virtual ~Module() {}

	virtual bool Init() 
	{
		return true; 
	}
	
	virtual bool Start() 
	{
		return true;
	}

	virtual update_status PreUpdate()
	{
		return UPDATE_CONTINUE;
	}

	virtual update_status Update()
	{
		return UPDATE_CONTINUE;
	}

	virtual update_status PostUpdate()
	{
		return UPDATE_CONTINUE;
	}

	virtual void SaveModule(Config* config) {}

	virtual void LoadModule(Config* config) {}

	virtual bool CleanUp() 
	{ 
		return true; 
	}

	virtual void EventResolve(const Event& event) {}

};

#endif
