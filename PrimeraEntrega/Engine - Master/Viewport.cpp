#include "Globals.h"
#include "Viewport.h"

#include "Application.h"


#include "ModuleRender.h"
#include "ModuleInput.h"
//#include "ModuleDebugDraw.h"
#include "ModuleImGui.h"
#include "ModuleCameraEditor.h"
#include "ModuleHints.h"
#include "ModulePrograms.h"

#include "GameObject.h"

#include "ComponentCamera.h"

#include "Config.h"
#include "utils/debug_draw.hpp"

#include "ImGui.h"
#include "GL/glew.h"

Viewport::Viewport()
{
}

Viewport::~Viewport()
{
	RemoveFrameBuffer(fbuffer);
	RemoveFrameBuffer(msaa_fbuffer);
	RemoveFrameBuffer(post_fbuffer);
}

void Viewport::Draw(ComponentCamera* camera)
{
	int posx = App->imgui->GetPosX(ModuleImGui::TabPanelLeft) + App->imgui->GetWidth(ModuleImGui::TabPanelLeft);
	int posy = 21;
	int width = App->imgui->GetPosX(ModuleImGui::TabPanelRight) - posx;
	int height = App->imgui->GetPosY(ModuleImGui::TabPanelBottom) - 21;

	ImGui::SetNextWindowPos(ImVec2(float(posx), float(posy)));
	ImGui::SetNextWindowSize(ImVec2(float(width), float(height)));

	if (ImGui::Begin("Viewport", &active, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoFocusOnAppearing))
	{
		if (ImGui::BeginChild("SceneCanvas", ImVec2(0, 0), true, ImGuiWindowFlags_NoMove))
		{
			DrawQuickBar(camera);

			focused = ImGui::IsWindowFocused();

			float width = ImGui::GetWindowContentRegionWidth();
			float height = ImGui::GetContentRegionAvail().y;

			camera->SetAspectRatio(float(width) / float(height));
			GenerateFBOs(unsigned(width), unsigned(height));

			float metric_proportion = App->hints->GetFloatValue(ModuleHints::METRIC_PROPORTION);
			if (draw_plane == true)
			{
				dd::xzSquareGrid(-50.0f*metric_proportion, 50.0f*metric_proportion, 0.0f, metric_proportion, dd::colors::LightGray, 0, true);
			}

			if (draw_axis == true)
			{
				dd::axisTriad(math::float4x4::identity, metric_proportion*0.1f, metric_proportion, 0, false);
			}

			//if (debug_draw == true)
			//{
			//	App->DebugDraw();
			//}

			bool msaa = App->hints->GetBoolValue(ModuleHints::ENABLE_MSAA);

			App->renderer->Draw(camera, msaa ? msaa_fbuffer.id : fbuffer.id, fb_width, fb_height);
			//App->debug_draw->Draw(camera, msaa ? msaa_fbuffer.id : fbuffer.id, fb_width, fb_height);

			App->renderer->Postprocess(msaa ? msaa_fbuffer.tex : fbuffer.tex, post_fbuffer.id, fb_width, fb_height);

			ImVec2 screenPos = ImGui::GetCursorScreenPos();

			ImGui::GetWindowDrawList()->AddImage(
				(void*)post_fbuffer.tex,
				ImVec2(screenPos),
				ImVec2(screenPos.x + fb_width, screenPos.y + fb_height),
				ImVec2(0, 1), ImVec2(1, 0));

			DrawGuizmo(camera);

		}
		ImGui::EndChild();
	}
	ImGui::End();
}

void Viewport::Save(Config* config) const
{
	config->AddBool("Draw Plane", draw_plane);
	config->AddBool("Draw Axis", draw_axis);
	config->AddBool("Debug Draw", debug_draw);
}

void Viewport::Load(Config* config)
{
	draw_plane = config->GetBool("Draw Plane", true);
	draw_axis = config->GetBool("Draw Axis", true);
	debug_draw = config->GetBool("Debug Draw", true);
}

void Viewport::GenerateFBO(Framebuffer& buffer, unsigned w, unsigned h, bool depth, bool msaa, bool hdr)
{
	RemoveFrameBuffer(buffer);

	assert(w != 0 && h != 0);

	glGenFramebuffers(1, &buffer.id);
	glBindFramebuffer(GL_FRAMEBUFFER, buffer.id);
	glGenTextures(1, &buffer.tex);

	if (msaa)
	{
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, buffer.tex);
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, hdr ? GL_RGBA16F : GL_RGBA, w, h, GL_TRUE);

		glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, buffer.tex, 0);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D, buffer.tex);

		glTexImage2D(GL_TEXTURE_2D, 0, hdr ? GL_RGBA16F : GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, buffer.tex, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	if (depth)
	{
		glGenRenderbuffers(1, &buffer.depth);
		glBindRenderbuffer(GL_RENDERBUFFER, buffer.depth);

		if (msaa)
		{
			glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, w, h);
		}
		else
		{
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, w, h);
		}

		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, buffer.depth);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}

	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Viewport::GenerateFBOs(unsigned w, unsigned h)
{
	if (w != fb_width || h != fb_height)
	{
		GenerateFBO(fbuffer, w, h, true, false, true);
		GenerateFBO(msaa_fbuffer, w, h, true, true, true);
		GenerateFBO(post_fbuffer, w, h, false, false, false);

		fb_width = w;
		fb_height = h;
	}
}

void Viewport::DrawQuickBar(ComponentCamera* camera)
{

	if (ImGui::BeginChild("ToolCanvas", ImVec2(435, 38), true, ImGuiWindowFlags_NoMove))
	{

		ImGui::SameLine(75.f);
		ImGui::Checkbox("Grid", &draw_plane);
		ImGui::SameLine();
		ImGui::Checkbox("Axis", &draw_axis);
		ImGui::SameLine();
		ImGui::Checkbox("Dbg Draw", &debug_draw);

		ImGui::SameLine();
		ImGui::ColorEdit3("Background", (float*)&camera->background, ImGuiColorEditFlags_NoInputs);
	}
	ImGui::EndChild();

	ImGui::SameLine();
	if (ImGui::BeginChild("ScaleCanvas", ImVec2(145, 38), true, ImGuiWindowFlags_NoMove))
	{
		float metric_proportion = App->hints->GetFloatValue(ModuleHints::METRIC_PROPORTION);
		float prev = metric_proportion;
		if (ImGui::DragFloat("Scale", &metric_proportion) && metric_proportion > 0.0f)
		{
			App->hints->SetFloatValue(ModuleHints::METRIC_PROPORTION, metric_proportion);
			ComponentCamera* camera = App->camera->GetCam();

			if (prev > 0.0f)
			{
				float adapt = (metric_proportion / prev);
				camera->frustum.pos *= adapt;
				camera->frustum.nearPlaneDistance *= adapt;
				camera->frustum.farPlaneDistance *= adapt;
			}
		}
	}
	ImGui::EndChild();

	ImGui::SameLine();
	if (ImGui::BeginChild("TextCanvas", ImVec2(350, 38), true, ImGuiWindowFlags_NoMove))
	{
		ImGui::SameLine();
		ImGui::Text("Avg %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}
	ImGui::EndChild();

}

void Viewport::DrawGuizmoProperties(GameObject* go)
{
	bool local = guizmo_mode == ImGuizmo::LOCAL && guizmo_op != ImGuizmo::SCALE;

	float4x4 model = local ? go->GetLocalTransform() : go->GetGlobalTransformation();
	model.Transpose();

	ImGui::RadioButton("Translate", (int*)&guizmo_op, (int)ImGuizmo::TRANSLATE);
	ImGui::SameLine();
	ImGui::RadioButton("Rotate", (int*)&guizmo_op, ImGuizmo::ROTATE);

	float matrixTranslation[3], matrixRotation[3], matrixScale[3];
	ImGuizmo::DecomposeMatrixToComponents((float*)&model, matrixTranslation, matrixRotation, matrixScale);
	bool transform_changed = ImGui::DragFloat3("Tr", matrixTranslation, 3);
	transform_changed = transform_changed || ImGui::DragFloat3("Rt", matrixRotation, 3);

	if (transform_changed)
	{
		ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, (float*)&model);
		model.Transpose();

		if (local || go->GetParent() == nullptr)
		{
			go->SetLocalTransform(model);
		}
		else
		{
			float4x4 parent = go->GetParent()->GetGlobalTransformation();
			parent.InverseOrthonormal();
			go->SetLocalTransform(parent*model);
		}
	}

	if (guizmo_op != ImGuizmo::SCALE)
	{
		ImGui::RadioButton("Local", (int*)&guizmo_mode, (int)ImGuizmo::LOCAL);
		ImGui::SameLine();
		ImGui::RadioButton("World", (int*)&guizmo_mode, (int)ImGuizmo::WORLD);
	}

	ImGui::PushID("snap");
	ImGui::Checkbox("", &guizmo_useSnap);
	ImGui::PopID();
	ImGui::SameLine();

	switch (guizmo_op)
	{
	case ImGuizmo::TRANSLATE:
		ImGui::InputFloat3("Snap", &guizmo_snap.x);
		break;
	case ImGuizmo::ROTATE:
		ImGui::InputFloat("Angle Snap", &guizmo_snap.x);
		break;
	}

}


void Viewport::DrawGuizmo(ComponentCamera* camera)
{
	if (App->imgui->selection_type == ModuleImGui::SelectionGameObject && App->imgui->selected.go != nullptr)
	{
		DrawGuizmo(camera, App->imgui->selected.go);
	}
}

void Viewport::DrawGuizmo(ComponentCamera* camera, GameObject* go)
{
	float4x4 view = camera->GetOpenGLViewMatrix();
	float4x4 proj = camera->GetOpenGLProjectionMatrix();

	ImGuizmo::BeginFrame();
	ImGuizmo::Enable(true);

	float4x4 model = go->GetGlobalTransformation();
	model.Transpose();

	float4x4 delta;

	ImGuiIO& io = ImGui::GetIO();
	ImGuizmo::SetRect(float(ImGui::GetCursorScreenPos().x), float(ImGui::GetCursorScreenPos().y), float(fb_width), float(fb_height));
	ImGuizmo::SetDrawlist();
	ImGuizmo::Manipulate((const float*)&view, (const float*)&proj, guizmo_op, guizmo_mode, (float*)&model, (float*)&delta, guizmo_useSnap ? &guizmo_snap.x : NULL);

	if (ImGuizmo::IsUsing() && !delta.IsIdentity())
	{
		model.Transpose();
		if (go->GetParent() == nullptr)
		{
			go->SetLocalTransform(model);
		}
		else
		{
			float4x4 parent = go->GetParent()->GetGlobalTransformation();
			parent.InverseOrthonormal();
			go->SetLocalTransform(parent*model);
		}
	}

	float3 points[8];
	go->global_bbox.GetCornerPoints(points);
	std::swap(points[2], points[5]);
	std::swap(points[3], points[4]);
	std::swap(points[4], points[5]);
	std::swap(points[6], points[7]);
	dd::box(points, dd::colors::Yellow);
}


float Viewport::DistanceFromAtt(float constant, float linear, float quadric, float epsilon)
{
	// Solve (1.0/constant+distance*linear+distance*distance*qua) <= 0.1; for solving distance for 0.1 attenuation
	// 1.0 <= 0.1*(c+d*l+d*d*q); 
	// 0 = (-1.0+0.1*c)+(0.1*l)*d+(0.1*q)*d*d;
	// a = 0.1*q;
	// b = 0.1*l;
	// d = (-b+-sqrt(b*b-4*a*c))/2.0f*a;

	float a = epsilon * quadric;
	float b = epsilon * linear;
	float c = epsilon * constant - 1.0f;

	float distance = 100000.0f;

	if (a == 0.0f)
	{
		if (b != 0.0f)
		{
			distance = -c / b;
		}
	}
	else
	{
		float sq = sqrt(b*b - 4.0f*a*c);
		float den = 1 / (2.0f*a);
		float d0 = (-b + sq)*den;
		float d1 = (-b - sq)*den;

		distance = max(d0, d1);
	}

	return distance;
}

void Viewport::RemoveFrameBuffer(Framebuffer& buffer)
{
	if (buffer.id != 0)
	{
		glDeleteFramebuffers(1, &buffer.id);
		buffer.id = 0;
	}

	if (buffer.depth != 0)
	{
		glDeleteRenderbuffers(1, &buffer.depth);
		buffer.depth = 0;
	}

	if (buffer.tex != 0)
	{
		glDeleteTextures(1, &buffer.tex);
		buffer.tex = 0;
	}

}
