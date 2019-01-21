#ifndef __COMPONENT_MESH_H__
#define __COMPONENT_MESH_H__

//#include "ComponentWithResource.h"
#include "Component.h"

class ResourceMesh;

class ComponentMesh : public Component
{
public:
	explicit ComponentMesh(GameObject* go);
	~ComponentMesh();

	bool                SetResource(unsigned long long uid);
	const ResourceMesh* GetResource() const;
	virtual void        OnSave(Config& config) const override;
	virtual void        OnLoad(Config* config) override;

	void                GetBoundingBox(AABB& box) const override;

	static TypeComponent GetClassType() { return Mesh; }

	bool                GetVisible() const { return visible; }
	void                SetVisible(bool v) { visible = v; }

private:
	unsigned long long resource = 0;
	bool visible = true;
};

#endif /* __COMPONENT_MESH_H__ */
