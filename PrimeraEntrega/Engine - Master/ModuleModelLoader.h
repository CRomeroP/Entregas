#ifndef __ModuleModelLoader_h__
#define __ModuleModelLoader_h__

#include "Module.h"
#include "MathGeoLib.h"
#include <assimp/scene.h>
#include <assimp/cimport.h>
#include <assimp/postprocess.h>

struct SDL_Texture;

class ModuleModelLoader :
	public Module
{
public:
	ModuleModelLoader();
	~ModuleModelLoader();
	bool            Init();
	update_status   Update();
	bool            CleanUp();

private:

	void            GenerateMeshData(aiMesh* scene);
	void            GenerateMaterialData(aiMaterial* material);


public:

	struct meshComplete
	{
		aiMesh* mesh;
		unsigned vbo;
		unsigned ibo;
	};

	std::vector<meshComplete> meshList;
	std::vector<unsigned> materialList;

	unsigned texture0;
};
#endif

