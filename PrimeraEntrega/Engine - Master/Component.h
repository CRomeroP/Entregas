#ifndef __COMPONENT_H__
#define __COMPONENT_H__

// Base class for all possible components a GameObject could have

#include "Config.h"
#include "Math.h"

class GameObject;
class Config;

class Component
{
public:

	enum TypeComponent
	{
		Mesh,
		Material,
		Camera,
		Unknown
	};

public:
	Component(GameObject* container, TypeComponent type);
	virtual ~Component();

	void SetActive(bool active);
	bool IsActive() const;
	TypeComponent GetType() const;
	const char* GetTypeStr() const;
	const GameObject* GetGameObject() const;
	GameObject* GetGameObject();

	// Queries
	virtual void GetBoundingBox(AABB& box) const {}

	// Events
	virtual void OnSave(Config& config) const = 0;
	virtual void OnLoad(Config* config) = 0;

	virtual void OnActivate() {};
	virtual void OnDeActivate() {};

	virtual void OnStart() {};
	virtual void OnUpdate(float dt) {};
	virtual void OnFinish() {};

	virtual void OnPlay() {};
	virtual void OnStop() {};
	virtual void OnPause() {};
	virtual void OnUnPause() {};
	virtual void OnGoDestroyed() {};

	virtual void OnUpdateTransform() {};
	virtual void OnDebugDraw(bool selected) const {};

public:
	bool removal = false;

protected:
	TypeComponent type = TypeComponent::Unknown;
	bool active = false;
	GameObject* game_object = nullptr;
};

#endif // __COMPONENT_H__
