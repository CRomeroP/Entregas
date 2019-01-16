#ifndef __RESOURCE_TEXTURE_H__
#define __RESOURCE_TEXTURE_H__

#include "Resources.h"

class ResourceTexture : public Resource
{
	friend class ModuleTextures;
public:

	enum Format {
		color_index,
		rgb,
		rgba,
		bgr,
		bgra,
		luminance,
		unknown
	};

public:

	ResourceTexture(unsigned long long id);
	virtual ~ResourceTexture();

	const char* GetFormatStr() const;

	bool LoadInMemory() override;
	void ReleaseFromMemory() override;

	void Save(Config& config) const override;
	void Load(const Config& config) override;

	unsigned int GetID() const { return gpu_id; }
	unsigned int GetWidth() const { return width; }
	unsigned int GetHeight() const { return height; }
	unsigned int GetDepth() const { return depth; }
	unsigned int GetBPP() const { return bpp; }
	bool HasMips() const { return has_mips; }
	unsigned int GetBytes() const { return bytes; }
	bool GetLinear() const { return linear; }
	bool GetCompressed() const { return compressed; }

	void SetLinear(bool l);
	void EnableMips(bool enable);

private:
	unsigned int width = 0;
	unsigned int height = 0;
	unsigned int depth = 0;
	unsigned int bpp = 0;
	bool has_mips = false;
	bool linear = true;
	bool compressed = true;
	unsigned int bytes = 0;
	unsigned int gpu_id = 0;

	Format format = unknown;
};

#endif // __RESOURCE_TEXTURE_H__
