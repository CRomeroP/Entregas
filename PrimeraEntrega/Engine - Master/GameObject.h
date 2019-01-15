#ifndef __GAMEOBJECT_H__
#define __GAMEOBJECT_H__

#include "Math.h"
#include "Component.h"
#include <list>
#include <map>
#include <vector>

class GameObject
{
public:
	GameObject(GameObject* parent, const char* name);
	GameObject(GameObject* parent, const char* name, const float3& translation, const float3& scale, const Quat& rotation);

	bool Save();
	void Load()




	~GameObject();
};

#endif
