#ifndef __PANEL_H__
#define __PANEL_H__

#include "Globals.h"
#include "Module.h"
#include "ImGui\imgui.h"
#include <string>
#include <list>

struct assetsElement
{
	~assetsElement()
	{}

	enum elementType
	{
		FOLDER = 0,
		FILE
	}type;

	std::string name = "";
	bool selected = false;
	bool renaming = false;
};

class Panel
{
public:
	Panel(const char* name) : name(name) {}
	virtual ~Panel() {}

	virtual void Draw() {}
	void toggleActive() { active = !active; }

	bool isActive() const { return active; }
	const char* getName() const { return name.c_str(); }

protected:

	std::string name = "";
	bool active = false;

};

#endif // !__PANEL_H__