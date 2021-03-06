#ifndef __MODULE_LEVELMANAGER_H__
#define __MODULE_LEVELMANAGER_H__

#include "Globals.h"
#include "Module.h"
#include "MathGeoLib.h"
#include "QuadTree.h"

class GameObject;
class PointLight;
class SpotLight;
class DirLight;
class AmbientLight;

class ModuleScene : public Module
{
public:

	ModuleScene(bool start_enabled = true);
	~ModuleScene();

	bool Init(Config* config = nullptr);
	bool Start(Config* config = nullptr);
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	bool CleanUp() override;

	void ReceiveEvent(const Event& event);

	// Utils
	const GameObject*   GetRoot() const;
	GameObject*         GetRoot();
	const GameObject*   Find(unsigned int uid) const;
	GameObject*         Find(unsigned int uid);

	// Manage whole levels
	bool CreateNewEmpty(const char* name);
	bool Load(const char* file);
	bool Save(const char* file = nullptr);
	void UnloadCurrent();

	// Add or remove from the hierarchy
	GameObject* CreateGameObject(GameObject * parent, const float3& pos, const float3& scale, const Quat& rot, const char* name = nullptr);
	GameObject* CreateGameObject(GameObject * parent = nullptr);
	GameObject* Duplicate(const GameObject* original);

	// Utils
	GameObject*         Validate(const GameObject* pointer) const;
	GameObject*         CastRay(const LineSegment& segment, float& dist) const;
	GameObject*         CastRay(const Ray& ray, float& dist) const;
	GameObject*         CastRayOnBoundingBoxes(const LineSegment& segment, float& dist, float3& normal) const;
	void                FindNear(const float3& position, float radius, std::vector<GameObject*>& results) const;

	GameObject*         AddModel(unsigned long long model);

	const DirLight*     GetDirLight() const { return directional; }
	DirLight*           GetDirLight() { return directional; }

	const AmbientLight* GetAmbientLight() const { return ambient; }
	AmbientLight*       GetAmbientLight() { return ambient; }

private:

	void RecursiveTestRayBBox(const LineSegment& segment, float& dist, float3& normal, GameObject** best_candidate) const;
	void RecursiveTestRay(const LineSegment& segment, float& dist, GameObject** best_candidate) const;
	void RecursiveTestRay(const Ray& ray, float& dist, GameObject** best_candidate) const;
	void RecursiveProcessEvent(GameObject* go, const Event& event) const;
	void RecursiveUpdate(GameObject* go, float dt) const;
	GameObject* RecursiveFind(unsigned int  uid, GameObject* go) const;
	void DestroyFlaggedGameObjects();

	void LoadGameObjects(const Config& config);
	void LoadLights(const Config& config);

	void SaveLights(Config& config) const;
	void RemoveLights();

public:
	Quadtree quadtree;
	bool draw_quadtree = false;

private:
	GameObject* root = nullptr;
	AmbientLight* ambient = nullptr;
	DirLight* directional = nullptr;
	std::vector<PointLight*> points;
	std::vector<SpotLight*> spots;
	std::string name;
};

#endif // __MODULE_LEVELMANAGER_H__
