#include "Application.h"
#include "ModuleCamera.h"
#include "ModuleInput.h"
#include "SDL/include/SDL.h"


ModuleCamera::ModuleCamera()
{
}


ModuleCamera::~ModuleCamera()
{
}

bool ModuleCamera::Init()
{
	frustum.type = math::FrustumType::PerspectiveFrustum;

	frustum.pos = { 0.0f,5.0f,-10.0f };
	frustum.front = { 0.0f,0.0f,-1.0f };
	frustum.up = { 0.0f,1.0f,0.0f };

	frustum.nearPlaneDistance = 0.1f;
	frustum.farPlaneDistance = 1000.0f;

	frustum.verticalFov = 90.0f;
	frustum.horizontalFov = 2.f * Atan(Tan(frustum.verticalFov*0.5f)*16.0f/9.0f);

	cameraPos = math::float3(0.0f, 3.0f, 5.0f);
	cameraTarget = math::float3(0.0f, 0.0f, 0.0f);
	LookAt(view, frustum.pos, cameraTarget, frustum.up);
	cameraSpeed = 0.5f;
	return true;
}

update_status ModuleCamera::Update()
{
	
	float3 posAux = float3(0.0f, 0.0f, 0.0f);
	if (App->input->GetKey(NULL)[SDL_SCANCODE_LSHIFT]) {
		cameraSpeed *= 2;
	}
	if (App->input->GetKey(NULL)[SDL_SCANCODE_E]) {
		posAux.y += cameraSpeed;
	}
	if (App->input->GetKey(NULL)[SDL_SCANCODE_Q]) {
		posAux.y -= cameraSpeed;
	}
	if (App->input->GetKey(NULL)[SDL_SCANCODE_W]) {
		posAux += frustum.front*cameraSpeed;
	}
	if (App->input->GetKey(NULL)[SDL_SCANCODE_S]) {
		posAux -= frustum.front*cameraSpeed;
	}
	if (App->input->GetKey(NULL)[SDL_SCANCODE_A]) {
		posAux -= frustum.WorldRight()*cameraSpeed;
	}

	if (App->input->GetKey(NULL)[SDL_SCANCODE_D]) {
		posAux += frustum.WorldRight()*cameraSpeed;
	}

	cameraPos += posAux;
	cameraTarget += posAux;


	frustum.pos = cameraPos;

	Z = -frustum.front;
	Y = frustum.up;
	X = frustum.WorldRight();
	LookAt(view, frustum.pos, cameraTarget, frustum.up);
	cameraSpeed = 0.5f;
	return UPDATE_CONTINUE;
}

bool ModuleCamera::CleanUp()
{
	return true;
}

void ModuleCamera::LookAt(math::float4x4&  matrix, const math::float3& eye, const math::float3& target, const math::float3& up)
{
	math::float3 f(target - eye);
	f.Normalize();

	math::float3 s(f.Cross(up));
	s.Normalize();

	math::float3 u(s.Cross(f));

	// axis in rows to do inverse
	matrix[0][0] = s.x;
	matrix[0][1] = s.y;
	matrix[0][2] = s.z;

	matrix[1][0] = u.x;
	matrix[1][1] = u.y;
	matrix[1][2] = u.z;

	matrix[2][0] = -f.x;
	matrix[2][1] = -f.y;
	matrix[2][2] = -f.z;

	// mult inverted rotation matrix negate to make inverse
	matrix[0][3] = -s.Dot(eye);
	matrix[1][3] = -u.Dot(eye);
	matrix[2][3] = f.Dot(eye);

	matrix[3][0] = 0.0f;
	matrix[3][1] = 0.0f;
	matrix[3][2] = 0.0f;
	matrix[3][3] = 1.0f;
}

float * ModuleCamera::getViewMatrix()
{
	static float4x4 matrix;
	matrix = frustum.ViewMatrix();
	matrix.Transpose();

	return (float*)matrix.v;
}
