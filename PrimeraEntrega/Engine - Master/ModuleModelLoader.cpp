#include "Globals.h"

#include "Application.h"
#include "ModuleModelLoader.h"
#include "ModuleTextures.h"

#include "GL/glew.h"


ModuleModelLoader::ModuleModelLoader()
{
}


ModuleModelLoader::~ModuleModelLoader()
{
}

bool ModuleModelLoader::Init()
{
	const aiScene* scene = aiImportFile("BakerHouse.fbx", aiProcess_Triangulate);
	if (scene == NULL)
	{
		const char* error = aiGetErrorString();
		printf(error);
	}

	for (unsigned i = 0; i < scene->mNumMeshes; ++i)
	{
		GenerateMeshData(scene->mMeshes[i]);
	}

	for (unsigned i = 0; i < scene->mNumMaterials; ++i)
	{
		GenerateMaterialData(scene->mMaterials[i]);
	}

	return true;
}

update_status ModuleModelLoader::Update()
{
	return UPDATE_CONTINUE;
}

bool ModuleModelLoader::CleanUp()
{
	return true;
}

void ModuleModelLoader::GenerateMeshData(aiMesh* mesh) 
{
	unsigned int vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, (sizeof(float) * 3 + sizeof(float) * 2)*mesh->mNumVertices, nullptr, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, (sizeof(float) * 3)*mesh->mNumVertices, mesh->mVertices);
	math::float2* texture_coords = (math::float2*)glMapBufferRange(GL_ARRAY_BUFFER, sizeof(float) * 3 * mesh->mNumVertices, sizeof(float) * 2 * mesh->mNumVertices, GL_MAP_WRITE_BIT);

	for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
	{
		texture_coords[i] = math::float2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
	}

	glUnmapBuffer(GL_ARRAY_BUFFER);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	unsigned ibo;

	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned)*mesh->mNumFaces * 3, nullptr, GL_STATIC_DRAW);

	unsigned* index = (unsigned*)glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(unsigned)*mesh->mNumFaces * 3, GL_MAP_WRITE_BIT);

	for (unsigned i = 0; i < mesh->mNumFaces; ++i) 
	{
		assert(mesh->mFaces[i].mNumIndices == 3);
		*(index++) = mesh->mFaces[i].mIndices[0];
		*(index++) = mesh->mFaces[i].mIndices[1];
		*(index++) = mesh->mFaces[i].mIndices[2];
	}

	glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	meshList.push_back(mesh);
}

void ModuleModelLoader::GenerateMaterialData(aiMaterial * material)
{
	aiString source;
	aiTextureMapping map;
	unsigned uvindex = 0;

	if (material->GetTexture(aiTextureType_DIFFUSE, 0, &source, &map, &uvindex) == AI_SUCCESS)
	{
		texture0 = App->textures->Load(source.data);
	}

	materialList.push_back(material);
}
