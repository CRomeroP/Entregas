#ifndef __COMPONENT_MATERIAL_H__
#define __COMPONENT_MATERIAL_H__

#include "Component.h"

struct TextureInfo;
class ResourceMaterial;

class ComponentMaterial : public Component
{
public:
	enum ERenderMode
	{
		RENDER_OPAQUE = 0,
		RENDER_TRANSPARENT,
		RENDER_COUNT
	};
public:

	ComponentMaterial(GameObject* container);
	~ComponentMaterial();

	bool                    SetResource(unsigned long long uid);
	const ResourceMaterial* GetResource() const;
	ResourceMaterial*       GetResource();

	bool                    CastShadows() const { return cast_shadows; }
	bool                    RecvShadows() const { return recv_shadows; }

	ERenderMode             RenderMode() const { return render_mode; }
	void                    SetRenderMode(ERenderMode mode) { render_mode = mode; }

	void                    OnSave(Config& config) const override;
	void                    OnLoad(Config* config) override;

	static TypeComponent    GetClassType() { return Material; }

private:

	unsigned long long  resource = 0;
	bool cast_shadows = true;
	bool recv_shadows = true;
	ERenderMode render_mode = RENDER_OPAQUE;

};

#endif // __COMPONENT_MESH_H__
