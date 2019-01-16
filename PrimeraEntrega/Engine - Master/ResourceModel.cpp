#include "ResourceModel.h"

#include "Application.h"
#include "ModuleFileSystem.h"
#include "ModuleResources.h"

#include "Assimp/include/assimp/scene.h"
#include "Assimp/include/assimp/cimport.h"
#include "Assimp/include/assimp/postprocess.h"

#include "ResourceMaterial.h"
#include "ResourceMesh.h"

#include "utils/SimpleBinStream.h"
#include "HashString.h"

#pragma comment (lib, "Assimp/libx86/assimp.lib")

ResourceModel::ResourceModel(unsigned long long id) : Resource(id, Resource::Type::model)
{
}

ResourceModel::~ResourceModel()
{
}

void ResourceModel::Save(Config& config) const
{
	Resource::Save(config);
}

void ResourceModel::Load(const Config& config)
{
	Resource::Load(config);
}

bool ResourceModel::LoadInMemory()
{
	if (GetExportedFile() != nullptr)
	{
		char* buffer = nullptr;
		unsigned int size = App->fs->Load(LIBRARY_MESH_FOLDER, GetExportedFile(), &buffer);

		simple::mem_istream<std::true_type> read_stream(buffer, size);

		unsigned int node_size = 0;

		read_stream >> node_size;

		nodes.reserve(node_size);

		for (unsigned int i = 0; i < node_size; ++i)
		{
			Node node;

			read_stream >> node.name;

			for (unsigned int i = 0; i < 16; ++i)
			{
				read_stream >> reinterpret_cast<float*>(&node.transform)[i];
			}

			read_stream >> node.parent;
			read_stream >> node.mesh;
			read_stream >> node.material;

			nodes.push_back(node);
		}

		for (unsigned int i = 0; i < nodes.size(); ++i)
		{
			if (nodes[i].mesh != 0)
			{
				App->resources->Get(nodes[i].mesh)->LoadToMemory();
			}

			if (nodes[i].material != 0)
			{
				App->resources->Get(nodes[i].material)->LoadToMemory();
			}
		}

		return true;
	}

	return false;
}

void ResourceModel::ReleaseFromMemory()
{
	for (unsigned int i = 0; i < nodes.size(); ++i)
	{
		if (nodes[i].mesh != 0)
		{
			App->resources->Get(nodes[i].mesh)->Release();
		}

		if (nodes[i].material != 0)
		{
			App->resources->Get(nodes[i].material)->Release();
		}
	}

	nodes.clear();
}

bool ResourceModel::Save(std::string& output) const
{
	simple::mem_ostream<std::true_type> write_stream;

	write_stream << unsigned int(nodes.size());

	for (unsigned int i = 0; i < nodes.size(); ++i)
	{
		write_stream << nodes[i].name;
		write_stream << nodes[i].transform;
		write_stream << nodes[i].parent;
		write_stream << nodes[i].mesh;
		write_stream << nodes[i].material;
	}

	const std::vector<char>& data = write_stream.get_internal_vec();

	return App->fs->SaveUnique(output, &data[0], data.size(), LIBRARY_MESH_FOLDER, "model", "edumodel");
}


bool ResourceModel::Import(const char* full_path, std::string& output)
{
	unsigned flags = aiProcess_CalcTangentSpace | \
		aiProcess_GenSmoothNormals | \
		aiProcess_JoinIdenticalVertices | \
		aiProcess_ImproveCacheLocality | \
		aiProcess_LimitBoneWeights | \
		aiProcess_SplitLargeMeshes | \
		aiProcess_Triangulate | \
		aiProcess_GenUVCoords | \
		aiProcess_SortByPType | \
		aiProcess_FindDegenerates | \
		aiProcess_FindInvalidData |
		0;

	aiString assimp_path(".");
	assimp_path.Append(full_path);

	const aiScene* scene = aiImportFile(assimp_path.data, flags);

	if (scene)
	{
		ResourceModel m(0);

		std::vector<unsigned long long> materials, meshes;
		m.GenerateMaterials(scene, full_path, materials);
		m.GenerateMeshes(scene, full_path, meshes);
		m.GenerateNodes(scene, scene->mRootNode, 0, meshes, materials);

		aiReleaseImport(scene);

		return m.Save(output);
	}

	return false;
}

void ResourceModel::GenerateMaterials(const aiScene* scene, const char* file, std::vector<unsigned long long>& materials)
{
	materials.reserve(scene->mNumMaterials);

	for (unsigned i = 0; i < scene->mNumMaterials; ++i)
	{
		materials.push_back(ResourceMaterial::Import(scene->mMaterials[i], file));

		assert(materials.back() != 0);
	}
}


void ResourceModel::GenerateMeshes(const aiScene* scene, const char* file, std::vector<unsigned long long>& meshes)
{
	meshes.reserve(scene->mNumMeshes);

	for (unsigned i = 0; i < scene->mNumMeshes; ++i)
	{
		meshes.push_back(ResourceMesh::Import(scene->mMeshes[i], file));

		assert(meshes.back() != 0);
	}
}

void ResourceModel::GenerateNodes(const aiScene* model, const aiNode* node, unsigned int parent, const std::vector<unsigned long long>& meshes, const std::vector<unsigned long long>& materials)
{
	unsigned int index = nodes.size();

	Node dst;

	dst.transform = reinterpret_cast<const float4x4&>(node->mTransformation);
	dst.name = node->mName.C_Str();
	dst.parent = parent;

	nodes.push_back(dst);

	for (unsigned i = 0; i < node->mNumChildren; ++i)
	{
		GenerateNodes(model, node->mChildren[i], index, meshes, materials);
	}

	if (node->mNumMeshes == 1)
	{
		unsigned int mesh_index = node->mMeshes[0];

		nodes[index].mesh = meshes[mesh_index];
		nodes[index].material = materials[model->mMeshes[mesh_index]->mMaterialIndex];
	}
	else
	{
		for (unsigned int i = 0; i < node->mNumMeshes; ++i)
		{
			Node mesh;

			unsigned int mesh_index = node->mMeshes[i];

			mesh.parent = index;
			mesh.name = model->mMeshes[mesh_index]->mName.C_Str();
			mesh.mesh = meshes[mesh_index];
			mesh.material = materials[model->mMeshes[mesh_index]->mMaterialIndex];

			if (mesh.name.length() == 0)
			{
				char buff[100];
				snprintf(buff, sizeof(buff), "mesh_%d", i);
				mesh.name = buff;
			}

			nodes.push_back(mesh);
		}
	}
}


