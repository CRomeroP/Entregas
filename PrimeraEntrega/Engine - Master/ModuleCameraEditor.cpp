#include "ModuleCameraEditor.h"
#include "Globals.h"
#include "Application.h"
#include "ModuleCameraEditor.h"
#include "ModuleImGui.h"
#include "ModuleInput.h"
#include "ModuleWindow.h"
#include "ComponentCamera.h"
#include "ModuleRender.h"
#include "utils/debug_draw.hpp"
#include "ModuleScene.h"
#include "GameObject.h"
#include <vector>

#include "mmgr/mmgr.h"






ModuleCameraEditor::ModuleCameraEditor()
{
	cam = new ComponentCamera(nullptr);
	selector = LineSegment(float3::zero, float3::unitY);
	lhit = float3::zero;
}


ModuleCameraEditor::~ModuleCameraEditor()
{
	RELEASE(cam);
}

bool ModuleCameraEditor::Init(Config* config)
{
	cam->OnLoad(config);
	//App->render->active_camera = cam;
	return true;
}

bool ModuleCameraEditor::Start(Config* config)
{
	LOG("Setting Camera");
	bool ret = true;

	//Load(config);

	return false;
}

update_status ModuleCameraEditor::Update()
{
	Frustrum* frustum = &cam->frustum;
	return update_status();
}

bool ModuleCameraEditor::CleanUp()
{
	return false;
}

void ModuleCameraEditor::LookAt(math::float4x4 & matrix, const math::float3 & eye, const math::float3 & target, const math::float3 & up)
{
}

ComponentCamera * ModuleCameraEditor::getCam()
{
	return nullptr;
}

void ModuleCameraEditor::Move()
{
	Frustum* frustum = &dummy->frustum;

	float adjusted_speed = mov_speed;

	if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT) adjusted_speed *= 5.0f;
	if (App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT) adjusted_speed *= 0.5f;

	float3 right(frustum->WorldRight());
	float3 forward(frustum->front);

	float3 movement(float3::zero);

	float metric_proportion = App->hints->GetFloatValue(ModuleHints::METRIC_PROPORTION);

	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) movement += forward;
	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) movement -= forward;
	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) movement -= right;
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) movement += right;
	if (App->input->GetKey(SDL_SCANCODE_R) == KEY_REPEAT) movement += float3::unitY;
	if (App->input->GetKey(SDL_SCANCODE_F) == KEY_REPEAT) movement -= float3::unitY;

	if (movement.Equals(float3::zero) == false)
	{
		frustum->Translate(movement * (metric_proportion*adjusted_speed * dt));
		looking = false;
	}
}


