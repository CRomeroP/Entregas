#ifndef __ModuleRenderExercise_h__
#define __ModuleRenderExercise_h__

#include "Module.h"
#include "MathGeoLib.h"
#include "Math/float4x4.h"
#include "Math/float3.h"

class ModuleRenderExercise : public Module
{
public:
    ModuleRenderExercise();
    ~ModuleRenderExercise();

	bool            Init    ();
	update_status   Update  ();
	bool            CleanUp ();

private:
    unsigned vbo        = 0;

	math::float4x4 model;

};

#endif /* __ModuleRenderExercise_h__ */
