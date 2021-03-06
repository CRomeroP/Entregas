#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleWindow.h"
#include "ModuleCamera.h"
#include "ModulePrograms.h"
#include "ModuleModelLoader.h"
#include "ModuleImGui.h"
#include "SDL.h"
#include "GL/glew.h"

#include "DevIL\include\IL\il.h"
#include "DevIL\include\IL\ilut.h"

ModuleRender::ModuleRender()
{
}

// Destructor
ModuleRender::~ModuleRender()
{
}

// Called before render is available
bool ModuleRender::Init()
{
	LOG("Creating Renderer context");

	context = SDL_GL_CreateContext(App->window->window);

	glewInit();

	model = float4x4::identity;

	glMatrixMode(GL_PROJECTION);//Applies subsequent matrix operations to the projection matrix stack. (screen position)
	glLoadIdentity();
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glClearDepth(1.0f);//Specifies the depth value used when the depth buffer is cleared.

		//Initialize clear color
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);//clear values for the color buffers. The initial values are all 0.
	
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glEnable(GL_TEXTURE_2D);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Set texture interpolation method to use linear interpolation (no MIPMAPS)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 				// Type of texture
		0,				// Pyramid level (for mip-mapping) - 0 is the top level
		ilGetInteger(IL_IMAGE_FORMAT),	// Internal pixel format to use. Can be a generic type like GL_RGB or GL_RGBA, or a sized type
		ilGetInteger(IL_IMAGE_WIDTH),	// Image width
		ilGetInteger(IL_IMAGE_HEIGHT),	// Image height
		0,				// Border width in pixels (can either be 1 or 0)
		ilGetInteger(IL_IMAGE_FORMAT),	// Format of image pixel data
		GL_UNSIGNED_BYTE,		// Image data type
		ilGetData());
	glBindTexture(GL_TEXTURE_2D, 0);

    int width, height;
    SDL_GetWindowSize(App->window->window, &width, &height);
    glViewport(0, 0, width, height);

	return true;
}

update_status ModuleRender::PreUpdate()
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(App->camera->getViewMatrix());

	return UPDATE_CONTINUE;
}

// Called every draw update
update_status ModuleRender::Update()
{
	for (unsigned i = 0; i < App->model->meshList.size(); ++i)
	{
		glUseProgram(App->program->def_program);

		glUniformMatrix4fv(glGetUniformLocation(App->program->def_program, "model"), 1, GL_TRUE, &model[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(App->program->def_program, "view"), 1, GL_TRUE, &App->camera->view[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(App->program->def_program, "proj"), 1, GL_TRUE, &App->camera->frustum.ProjectionMatrix()[0][0]);

		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, App->model->materialList[App->model->meshList[i].mesh->mMaterialIndex]);
		glUniform1i(glGetUniformLocation(App->program->def_program, "texture0"), 0);


		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, App->model->meshList[i].vbo);
		glVertexAttribPointer(
			0,                  // attribute 0
			3,                  // number of componentes (3 floats)
			GL_FLOAT,           // data type
			GL_FALSE,           // should be normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);
		glVertexAttribPointer(
			1,                  // attribute 0
			2,                  // number of componentes (3 floats)
			GL_FLOAT,           // data type
			GL_FALSE,           // should be normalized?
			0,                  // stride
			(void*)(sizeof(float)*3*App->model->meshList[i].mesh->mNumVertices)// array buffer offset
		);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, App->model->meshList[i].ibo);
		glDrawElements(GL_TRIANGLES, App->model->meshList[i].mesh->mNumFaces * 3, GL_UNSIGNED_INT, nullptr);

		glLineWidth(1.0f);
		float d = 200.0f;
		glBegin(GL_LINES);
		for (float i = -d; i <= d; i += 1.0f)
		{
			glVertex3f(i, 0.0f, -d);
			glVertex3f(i, 0.0f, d);
			glVertex3f(-d, 0.0f, i);
			glVertex3f(d, 0.0f, i);
		}
		glEnd();

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glUseProgram(0);
	}

	return UPDATE_CONTINUE;
}

update_status ModuleRender::PostUpdate()
{

	App->imgui->Draw();
	SDL_GL_SwapWindow(App->window->window);

	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleRender::CleanUp()
{
	LOG("Destroying renderer");

	//Destroy window

	return true;
}

void ModuleRender::WindowResized(unsigned width, unsigned height)
{
    glViewport(0, 0, width, height); 
}

