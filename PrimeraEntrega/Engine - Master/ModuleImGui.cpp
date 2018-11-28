#include "Globals.h"
#include "Application.h"
#include "ModuleImGui.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"

//#include "GL/glew.h"
#include "GL/gl3w.h"

#include "ImGui\imgui.h"
#include "ImGui\imgui_impl_sdl.h"
#include "ImGui\imgui_impl_opengl3.h"
#include <SDL.h>




ModuleImGui::ModuleImGui()
{
}


ModuleImGui::~ModuleImGui()
{
}

bool ModuleImGui::Init()
{	
	gl3wInit();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer->context);
	ImGui_ImplOpenGL3_Init();
	return true;

}

bool ModuleImGui::Start()
{
	ImGui::GetStyle().Colors[ImGuiCol_WindowBg] = { 0.1f,0.1f,0.1f,1.0f };
	ImGui::GetStyle().Colors[ImGuiCol_ScrollbarBg] = { 0.05f, 0.05f,0.05f,1.0f };
	ImGui::GetStyle().Colors[ImGuiCol_TitleBg] = { 0.05f, 0.05f,0.05f,1.0f };
	ImGui::GetStyle().Colors[ImGuiCol_TitleBgActive] = { 0.3f, 0.3f,0.3f,1.0f };

	ImGui::GetStyle().Colors[ImGuiCol_::ImGuiCol_Tab] = { 0.7f,0.3f,0.0f,1.0f };
	ImGui::GetStyle().Colors[ImGuiCol_::ImGuiCol_TabHovered] = { 0.8f,0.35f,0.0f,1.0f };
	ImGui::GetStyle().Colors[ImGuiCol_::ImGuiCol_TabUnfocused] = { 0.5f,0.2f,0.0f,1.0f };
	ImGui::GetStyle().Colors[ImGuiCol_::ImGuiCol_TabUnfocusedActive] = { 0.65f,0.25f,0.0f,1.0f };
	ImGui::GetStyle().Colors[ImGuiCol_::ImGuiCol_TabActive] = { 0.9f,0.45f,0.0f,1.0f };

	ImGui::GetStyle().Colors[ImGuiCol_::ImGuiCol_Header] = { 0.8f,0.37f,0.0f,1.0f };
	ImGui::GetStyle().Colors[ImGuiCol_::ImGuiCol_HeaderActive] = { 1.0f,0.6f,0.0f,1.0f };
	ImGui::GetStyle().Colors[ImGuiCol_::ImGuiCol_HeaderHovered] = { 0.95f,0.5f,0.0f,1.0f };

	ImGui::GetStyle().Colors[ImGuiCol_::ImGuiCol_Button] = { 0.9f,0.45f,0.0f,1.0f };
	ImGui::GetStyle().Colors[ImGuiCol_::ImGuiCol_ButtonActive] = { 0.8f,0.37f,0.0f,1.0f };
	ImGui::GetStyle().Colors[ImGuiCol_::ImGuiCol_ButtonHovered] = { 0.95f,0.5f,0.0f,1.0f };

	ImGui::GetStyle().Colors[ImGuiCol_::ImGuiCol_FrameBg] = { 1.0f,0.6f,0.0f,0.2f };
	ImGui::GetStyle().Colors[ImGuiCol_::ImGuiCol_FrameBgActive] = { 0.8f,0.37f,0.0f,0.5f };
	ImGui::GetStyle().Colors[ImGuiCol_::ImGuiCol_FrameBgHovered] = { 0.95f,0.5f,0.0f,0.5f };

	ImGui::GetStyle().WindowMinSize = { 125, 100 };
	return true;
}

update_status ModuleImGui::PreUpdate()
{	
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();

	ImGui::SetNextWindowPos({ 0,0 });
	ImGui::SetNextWindowSize({ (float)App->window->width, (float)App->window->height });
	ImGui::SetNextWindowBgAlpha(0.0f);

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace", 0, window_flags);
	ImGui::PopStyleVar(3);

	//ImGuiID dockspace_id = ImGui::GetID("MyDockspace");
	//ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
	return UPDATE_CONTINUE;
}

update_status ModuleImGui::Update()
{
	
	return UPDATE_CONTINUE;
}

update_status ModuleImGui::PostUpdate()
{
	//SDL_GL_SwapWindow(App->window->window);
	return UPDATE_CONTINUE;
}

bool ModuleImGui::CleanUp()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
	return true;
}

void ModuleImGui::Draw()
{
	float horizontalFactor = (float)App->window->width / SCREEN_WIDTH;
	float verticalFactor = (float)App->window->height / SCREEN_HEIGHT;

	ImGui::GetStyle().WindowMinSize = { 125 * horizontalFactor, 100 * verticalFactor };

	ImGui::SetWindowPos("Hardware", { 0 * horizontalFactor,646 * verticalFactor });
	ImGui::SetWindowSize("Hardware", { 1083 * horizontalFactor, 118 * verticalFactor });

	ImGui::SetWindowPos("Console", { 0 * horizontalFactor,810 * verticalFactor });
	ImGui::SetWindowSize("Console", { 1292 * horizontalFactor, 140 * verticalFactor });

	consoleDraw();
	hardwareDraw();

	ImGui::End();

	ImGui::Render();
	SDL_GL_MakeCurrent(App->window->window, App->renderer->context);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	// Update and Render additional Platform Windows
	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}

	SDL_GL_MakeCurrent(App->window->window, App->renderer->context);

}

void ModuleImGui::consoleDraw()
{
	ImGui::Begin("Console", &active, ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_MenuBar);

	ImGui::BeginMenuBar();
	ImGui::PushStyleColor(ImGuiCol_Button, { 0.9f,0.45f,0.0f,0.7f });
	if (ImGui::Button("Clear"))
	{
	}
	ImGui::PopStyleColor();
	ImGui::EndMenuBar();

	ImGui::End();
}

void ModuleImGui::hardwareDraw()
{
	ImGui::Begin("Hardware", &active, ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_MenuBar);

	ImGui::Text("CPUs:");
	ImGui::SameLine();
	ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Text, { 1,1,0,1 });
	ImGui::Text("%d (Cache: %dB)", SDL_GetCPUCount(), SDL_GetCPUCacheLineSize());
	ImGui::PopStyleColor();

	ImGui::Text("System RAM:");
	ImGui::SameLine();
	ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Text, { 1,1,0,1 });
	ImGui::Text("%dGb", SDL_GetSystemRAM() / 1000);
	ImGui::PopStyleColor();

	ImGui::Text("Caps:");
	ImGui::SameLine();
	ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Text, { 1,1,0,1 });
	std::string caps;
	if (SDL_Has3DNow())
		caps += "3DNow!, ";
	if (SDL_HasAVX())
		caps += "AVX, ";
	if (SDL_HasAVX2())
		caps += "AVX2, ";
	if (SDL_HasAltiVec())
		caps += "AltiVec, ";
	if (SDL_HasMMX())
		caps += "MMX, ";
	if (SDL_HasRDTSC())
		caps += "RDTSC, ";
	if (caps.size() > 5)
		caps += "\n";
	if (SDL_HasSSE())
		caps += "SSE, ";
	if (SDL_HasSSE2())
		caps += "SSE2, ";
	if (SDL_HasSSE3())
		caps += "SSE3, ";
	if (SDL_HasSSE41())
		caps += "SSE41, ";
	if (SDL_HasSSE42())
		caps += "SSE42, ";
	ImGui::Text(caps.c_str());
	ImGui::PopStyleColor();

	ImGui::End();
}

void ModuleImGui::hardwareDraw() 
{
	ImGui::Begin("Configuration", &active, ImGuiWindowFlags_NoFocusOnAppearing);

	if (ImGui::CollapsingHeader("Application"))
	{
		ImGui::PushItemWidth(200.0f);
		static char app_name[120];
		strcpy_s(app_name, 120, App->getAppName());
		if (ImGui::InputText("App Name", app_name, 120, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
			App->setAppName(app_name);

		static int framerateCap = App->getFramerateCap();
		if (ImGui::SliderInt("MaxFPS", &framerateCap, 1, 120))
			App->setFramerateCap(framerateCap);
		ImGui::PopItemWidth();

		if (ImGui::Checkbox("Vsync", &App->renderer3D->vsync))
		{
			if (App->renderer3D->vsync)
				SDL_GL_SetSwapInterval(1);
			else
				SDL_GL_SetSwapInterval(0);
		}

		char title[25];
		sprintf_s(title, 25, "Framerate %.1f", fps_log[fps_log.size() - 1]);
		ImGui::PlotHistogram("##framerate", &fps_log[0], fps_log.size(), 0, title, 0.0f, 100.0f, ImVec2(310, 100));

		sprintf_s(title, 25, "Milliseconds %0.1f", ms_log[ms_log.size() - 1]);
		ImGui::PlotHistogram("##milliseconds", &ms_log[0], ms_log.size(), 0, title, 0.0f, 40.0f, ImVec2(310, 100));

		sMStats stats = m_getMemoryStatistics();
		addMemory((float)stats.totalReportedMemory);

		ImGui::PlotHistogram("##memory", &memory_log[0], memory_log.size(), 0, "Memory Consumption (Bytes)", 0.0f, (float)stats.peakReportedMemory * 1.2f, ImVec2(310, 100));

		ImGui::Text("Total Reported Mem: %u", stats.totalReportedMemory);
		ImGui::Text("Total Actual Mem: %u", stats.totalActualMemory);
		ImGui::Text("Peak Reported Mem: %u", stats.peakReportedMemory);
		ImGui::Text("Peak Actual Mem: %u", stats.peakActualMemory);
		ImGui::Text("Accumulated Reported Mem: %u", stats.accumulatedReportedMemory);
		ImGui::Text("Accumulated Actual Mem: %u", stats.accumulatedActualMemory);
		ImGui::Text("Accumulated Alloc Unit Count: %u", stats.accumulatedAllocUnitCount);
		ImGui::Text("Total Alloc Unit Count: %u", stats.totalAllocUnitCount);
		ImGui::Text("Peak Alloc Unit Count: %u", stats.peakAllocUnitCount);
	}

	if (ImGui::CollapsingHeader("Window"))
	{
		ImGui::PushItemWidth(200.0f);
		if (!fullscreen && (ImGui::InputInt("Width", &App->window->width, 50, 100, ImGuiInputTextFlags_EnterReturnsTrue) || ImGui::InputInt("Height", &App->window->height, 50, 100, ImGuiInputTextFlags_EnterReturnsTrue)))
		{
			SDL_SetWindowSize(App->window->window, App->window->width, App->window->height);
			App->renderer3D->OnResize(App->window->width, App->window->height);
		}

		if (ImGui::SliderFloat("Brightness", &brightness, 0.0f, 1.0f))
		{
			SDL_SetWindowBrightness(App->window->window, brightness);
		}
		ImGui::PopItemWidth();

		ImGui::Text("Refresh rate: %d", mode.refresh_rate);

		if (ImGui::Checkbox("FullScreen", &fullscreen))
		{
			if (fullscreen)
			{
				SDL_SetWindowFullscreen(App->window->window, SDL_WINDOW_FULLSCREEN);
				prevWidth = App->window->width;
				prevHeight = App->window->height;

				SDL_DisplayMode dM;
				SDL_GetDesktopDisplayMode(0, &dM);
				SDL_SetWindowSize(App->window->window, dM.w, dM.h);
				App->window->OnResize(dM.w, dM.h);
				App->renderer3D->OnResize(dM.w, dM.h);
			}
			else
			{
				SDL_SetWindowSize(App->window->window, prevWidth, prevHeight);
				App->window->OnResize(prevWidth, prevHeight);
				App->renderer3D->OnResize(prevWidth, prevHeight);

				SDL_SetWindowFullscreen(App->window->window, 0);
			}
		}
		ImGui::SameLine();
		if (ImGui::Checkbox("Resizable", &resizable))
		{

			if (resizable)
				SDL_SetWindowResizable(App->window->window, (SDL_bool)1);
			else
				SDL_SetWindowResizable(App->window->window, (SDL_bool)0);
		}

		ImGui::NewLine();
		if (ImGui::Checkbox("Borderless", &borderless))
		{
			if (borderless)
				SDL_SetWindowBordered(App->window->window, (SDL_bool)(!borderless));
			else
				SDL_SetWindowBordered(App->window->window, (SDL_bool)1);
		}

		ImGui::SameLine();
		if (ImGui::Checkbox("Full Desktop", &full_desktop))
		{
			if (full_desktop) {
				SDL_SetWindowFullscreen(App->window->window, SDL_WINDOW_FULLSCREEN_DESKTOP);
			}
			else
				SDL_SetWindowFullscreen(App->window->window, 0);
		}
	}

	if (ImGui::CollapsingHeader("Renderer 3D"))
	{
		if (ImGui::Checkbox("Depth Test", &App->renderer3D->Depth_Test))
		{
			if (App->renderer3D->Depth_Test)
				glEnable(GL_DEPTH_TEST);

			else
				glDisable(GL_DEPTH_TEST);
		}
		if (ImGui::Checkbox("Cull Face", &App->renderer3D->Cull_Face))
		{
			if (App->renderer3D->Cull_Face)
				glEnable(GL_CULL_FACE);

			else
				glDisable(GL_CULL_FACE);
		}
		if (ImGui::Checkbox("Lighting", &App->renderer3D->Lighting))
		{
			if (App->renderer3D->Lighting)
				glEnable(GL_LIGHTING);

			else
				glDisable(GL_LIGHTING);
		}
		if (ImGui::Checkbox("Color Material", &App->renderer3D->Color_Material))
		{
			if (App->renderer3D->Color_Material)
				glEnable(GL_COLOR_MATERIAL);

			else
				glDisable(GL_COLOR_MATERIAL);
		}
		if (ImGui::Checkbox("Texture 2D", &App->renderer3D->Texture_2D))
		{
			if (App->renderer3D->Texture_2D)
				glEnable(GL_TEXTURE_2D);

			else
				glDisable(GL_TEXTURE_2D);
		}
		ImGui::Checkbox("Wireframe", &App->renderer3D->Wireframe);
		ImGui::Checkbox("Normals", &App->renderer3D->Normals);
		ImGui::Checkbox("Face Normals", &App->renderer3D->Faces);
		ImGui::Checkbox("Show All Bounding Boxes", &App->renderer3D->ShowBB);
		ImGui::Checkbox("Show QuadTree", &App->renderer3D->ShowQT);
		ImGui::Checkbox("Frustum Culling", &App->renderer3D->Frustum_Culling);
		ImGui::Checkbox("Show Click Ray", &App->renderer3D->ShowClickRay);
	}

	if (ImGui::CollapsingHeader("Camera"))
	{
		ImGui::Text("Camera Position:");
		ImGui::Text("X: "); ImGui::SameLine();
		ImGui::Text(std::to_string(App->camera->Position.x).c_str());
		ImGui::Text("Y: "); ImGui::SameLine();
		ImGui::Text(std::to_string(App->camera->Position.y).c_str());
		ImGui::Text("Z: "); ImGui::SameLine();
		ImGui::Text(std::to_string(App->camera->Position.z).c_str());

		ImGui::InputFloat("Near Plane", &App->camera->camera->frustum.nearPlaneDistance);
		ImGui::InputFloat("Far Plane", &App->camera->camera->frustum.farPlaneDistance);
		ImGui::NewLine();

		float AR = App->camera->camera->frustum.AspectRatio();
		float VFOV = App->camera->camera->frustum.verticalFov*RADTODEG;
		if (ImGui::SliderFloat("FOV", &VFOV, 30, 135))
		{
			App->camera->camera->frustum.verticalFov = VFOV * DEGTORAD;
			App->camera->camera->setAspectRatio(AR);
			App->renderer3D->changedFOV = true;
		}
		ImGui::NewLine();

		ImGui::PushItemWidth(100.0f);
		ImGui::InputFloat("Camera Speed", &App->camera->cameraSpeed);
		ImGui::InputFloat("Mouse Sensitivity", &App->camera->mouseSensitivity);
		ImGui::InputFloat("Wheel Sensitivity", &App->camera->wheelSensitivity);
		ImGui::PopItemWidth();
	}

	if (ImGui::CollapsingHeader("Input"))
	{
		ImGui::Text("Mouse Position:");
		ImGui::Text("X: %d | Y: %d", App->input->mouse_x, App->input->mouse_y);
	}

	if (ImGui::CollapsingHeader("Time Manager"))
	{
		static int GameFrCap = App->getGameFramerateCap();
		if (ImGui::SliderInt("Game MaxFPS", &GameFrCap, 1, 120))
			App->setGameFramerateCap(GameFrCap);

		if (ImGui::Checkbox("Game Vsync", &App->renderer3D->vsync))
		{
			if (App->renderer3D->vsync)
				SDL_GL_SetSwapInterval(1);
			else
				SDL_GL_SetSwapInterval(0);
		}

		char title[25];
		sprintf_s(title, 25, "Game Framerate %.1f", game_fps_log[game_fps_log.size() - 1]);
		ImGui::PlotHistogram("##Game framerate", &game_fps_log[0], game_fps_log.size(), 0, title, 0.0f, 100.0f, ImVec2(310, 100));

		sprintf_s(title, 25, "Game Milliseconds %0.1f", game_ms_log[game_ms_log.size() - 1]);
		ImGui::PlotHistogram("##Game milliseconds", &game_ms_log[0], game_ms_log.size(), 0, title, 0.0f, 40.0f, ImVec2(310, 100));
	}

	ImGui::End();
}
