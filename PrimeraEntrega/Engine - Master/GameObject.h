#ifndef __GAMEOBJECT_H__
#define __GAMEOBJECT_H__M 

#include "MathGeoLib.h"
#include "MathBuildConfig.h"
#include "Component.h"

#include <list>
#include <vector>

class GameObject
{
	friend class Component;
public:
	GameObject(GameObject* parent, const char* name);
	~GameObject();

	Component* CreateComponent(Component::TypeComponent type);

public:
	bool active;
	std::string name;
	std::list<Component*> components;


};

#endif // ! __GAMEOBJECT_H 
