#ifndef _PANEL_GOTREE_H_
#define _PANEL_GOTREE_H_

#include "Panel.h"
#include "ImGui\imgui.h" 

class PanelGOTree :
	public Panel
{
public:
	PanelGOTree(const char* name);
	~PanelGOTree();

	void Draw();

public:
	unsigned selected = 0;

};

#endif /* _PANEL_GOTREE_H_ */
