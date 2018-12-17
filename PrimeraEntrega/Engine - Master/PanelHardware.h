#ifndef __PANELHARDWARE_H__
#define __PANELHARDWARE_H__

#include "Panel.h"

class PanelHardware : public Panel
{
public:
	PanelHardware(const char* name);
	~PanelHardware();

	void Draw();
};

#endif // !__PANELHARDWAREINFO_H__