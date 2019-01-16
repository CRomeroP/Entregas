#ifndef __MODULE_CAMERAEDITOR_H__
#define __MODULE_CAMERAEDITOR_H__

#include "Module.h"
#include "Globals.h"
#include "MathGeoLib.h"

class ComponentCamera;
class GameObject;

class ModuleCameraEditor :public Module
{
public:
	ModuleCameraEditor();
	~ModuleCameraEditor();

	bool Init();
	bool Start();
	update_status Update();
	bool CleanUp();
	
	void LookAt(math::float4x4 & matrix, const math::float3 & eye, const math::float3 & target, const math::float3 & up);

	ComponentCamera* getCam();

private:
	void Movement();
	void Rotate(float x, float y);
	void Zoom(float zoom);
	GameObject* Select(float3* select_point) const;

public:
	float mov_speed = 1.5f;
	float rot_speed = 1.0f;
	float zoom_speed = 1.5f;

private:
	float3 target;
	bool look = false;
	ComponentCamera* cam = nullptr;
	LineSegment selector;
	float3 lhit;
};

#endif


