#ifndef __MODULECAMERA_H__
#define __MODULECAMERA_H__
#include "Globals.h"
#include "Module.h"
#include "MathGeoLib/include/MathGeoLib.h"

class ModuleCamera :
	public Module
{
public:
	ModuleCamera();
	~ModuleCamera();

	bool			Init();
	update_status	Update();
	bool			CleanUp();

	void LookAt(math::float4x4 & matrix, const math::float3 & eye, const math::float3 & target, const math::float3 & up);

	float * getViewMatrix();

public:
	Frustum frustum;
	math::float3 X = math::float3(1.0f, 0.0f, 0.0f);
	math::float3 Y = math::float3(0.0f, 1.0f, 0.0f);
	math::float3 Z = math::float3(0.0f, 0.0f, 1.0f);
	math::float3 cameraPos;
	math::float3 cameraTarget;
	math::float4x4 view;
	float cameraSpeed;

};
#endif
