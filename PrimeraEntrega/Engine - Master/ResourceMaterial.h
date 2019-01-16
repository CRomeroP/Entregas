#ifndef __RESOURCE_MATERIAL_H__
#define __RESOURCE_MATERIAL_H__

#include "Resources.h"
#include "MathGeoLib.h"
#include "utils/SimpleBinStream.h"

struct aiMaterial;

class ResourceMaterial : public Resource
{
public:
	enum Texture
	{
		TextureDiffuse = 0,
		TextureSpecular,
		TextureNormal,
		TextureOcclusion,
		TextureEmissive,
		TextureCount
	};

private:

	float4      diffuse_color = float4::one;
	float3      specular_color = float3::zero;
	float3      emissive_color = float3::zero;
	unsigned long long         textures[TextureCount] = { 0, 0, 0, 0, 0 };
	float       shininess = 0.5f;
	float       k_ambient = 0.5f;
	float       k_diffuse = 0.5f;
	float       k_specular = 0.5f;

public:

	explicit ResourceMaterial(unsigned long long id);
	virtual ~ResourceMaterial();

	bool                    LoadInMemory() override;
	void                    ReleaseFromMemory() override;
	bool                    Save(std::string& output);
	bool                    Save();

	static unsigned long long Import(const aiMaterial* material, const char* source_file);

	const float4&           GetDiffuseColor() const { return diffuse_color; }
	void                    SetDiffuseColor(const float4& value) { diffuse_color = value; }

	const float3&           GetSpecularColor() const { return specular_color; }
	void                    SetSpecularColor(const float3& value) { specular_color = value; }

	const float3&           GetEmissiveColor() const { return emissive_color; }
	void                    SetEmissiveColor(const float3& value) { emissive_color = value; }

	float                   GetKSpecular() const { return k_specular; }
	void                    SetKSpecular(float value) { k_specular = value; }

	float                   GetKDiffuse() const { return k_diffuse; }
	void                    SetKDiffuse(float value) { k_diffuse = value; }

	float                   GetKAmbient() const { return k_ambient; }
	void                    SetKAmbient(float value) { k_ambient = value; }

	unsigned long long      GetTexture(Texture texture) const { return textures[texture]; }
	const ResourceTexture*  GetTextureRes(Texture texture) const;
	ResourceTexture*        GetTextureRes(Texture texture);
	void                    SetTexture(Texture texture, unsigned long long uid);

	float                   GetShininess() const { return shininess; }
	void                    SetShininess(float s) { shininess = s; }

private:
	void                    SaveToStream(simple::mem_ostream<std::true_type>& write_stream) const;
};


#endif /* __RESOURCE_MATERIAL_H__ */
