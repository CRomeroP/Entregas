#ifndef __RESOURCE_MESH_H__
#define __RESOURCE_MESH_H__

#include "Resources.h"
#include "Math.h"
#include "MathGeoLib/include/MathGeoLib.h"
#include "HashString.h"

#include <string>

class Resource;
struct aiMesh;
struct par_shapes_mesh_s;

class ResourceMesh : public Resource
{
	friend class ModuleMeshes;

public:
	
	ResourceMesh(unsigned long long id);
	virtual ~ResourceMesh();


	void        Save(Config& config) const override;
	void        Load(const Config& config) override;

	bool        LoadInMemory() override;
	void        ReleaseFromMemory() override;
	bool        Save(std::string& output) const;
	static unsigned long long  Import(const aiMesh* mesh, const char* source_file);

	static unsigned long long  LoadSphere(const char* sphere_name, float size, unsigned slices, unsigned stacks);
	static unsigned long long  LoadTorus(const char* torus_name, float inner_r, float outer_r, unsigned slices, unsigned stacks);
	static unsigned long long  LoadCube(const char* cube_name, float size);
	static unsigned long long  LoadCylinder(const char* cylinder_name, float height, float radius, unsigned slices, unsigned stacks);
	static unsigned long long  LoadPlane(const char* plane_name, float width, float height, unsigned slices, unsigned stacks);

private:

	static unsigned long long  Generate(const char* shape_name, par_shapes_mesh_s* shape);
	void        GenerateAttribInfo(par_shapes_mesh_s* shape);
	void        GenerateAttribInfo(const aiMesh* mesh);
	void        GenerateCPUBuffers(const aiMesh* mesh);
	void        GenerateCPUBuffers(par_shapes_mesh_s* shape);
	void        GenerateVBO(bool dynamic);
	void        GenerateBoneData(const aiMesh* mesh);
	void        GenerateVAO();
	void        GenerateTangentSpace();
	void        Clear();
	bool        Save(const char* source, std::string& output);

public:

	enum VertexAttribs
	{
		ATTRIB_TEX_COORDS_0 = 1 << 0,
		ATTRIB_NORMALS = 1 << 1,
		ATTRIB_TANGENTS = 1 << 2,
		ATTRIB_BONES = 1 << 3,
	};

	struct Weight
	{
		unsigned vertex = 0;
		float weight = 0.0f;
	};

	struct Bone
	{
		HashString	name;
		Weight*		weights = nullptr;
		unsigned	num_weights = 0;
		float4x4	bind = float4x4::identity;
	};

	HashString  name;

	unsigned int        vertex_size = 0;
	unsigned int        attribs = 0;
	unsigned int        texcoord_offset = 0;
	unsigned int        normal_offset = 0;
	unsigned int        tangent_offset = 0;
	unsigned int        bone_idx_offset = 0;
	unsigned int        bone_weight_offset = 0;

	unsigned int        num_vertices = 0;
	float3*     src_vertices = nullptr;
	float2*     src_texcoord0 = nullptr;
	float3*     src_normals = nullptr;
	float3*     src_tangents = nullptr;

	unsigned int        num_indices = 0;
	unsigned int*       src_indices = nullptr;

	unsigned int        num_bones = 0;
	Bone*       bones = nullptr;
	float4x4*   palette = 0;

	unsigned int        vao = 0;
	unsigned int        vbo = 0;
	unsigned int        ibo = 0;

	AABB        bbox;
};

#endif // __RESOURCE_MESH_H__
