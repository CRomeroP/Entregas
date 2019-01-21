#include "Component.h"
#include "Globals.h"

// ---------------------------------------------------------
Component::Component(GameObject* container, Component::TypeComponent type) : game_object(container), type(type)
{
	if (game_object != nullptr)
		SetActive(true);
}

// ---------------------------------------------------------
Component::~Component()
{}

// ---------------------------------------------------------
void Component::SetActive(bool active)
{
	if (this->active != active)
	{
		this->active = active;
		if (active)
			OnActivate();
		else
			OnDeActivate();
	}
}

// ---------------------------------------------------------
bool Component::IsActive() const
{
	return active;
}

// ---------------------------------------------------------
Component::TypeComponent Component::GetType() const
{
	return type;
}

// ---------------------------------------------------------
const char * Component::GetTypeStr() const
{
	assert(Component::TypeComponent::Unknown == 4);

	static const char* names[] = {
	"Mesh",
	"Material",
	"Camera",
	"Invalid" };

	return names[type];
}

// ---------------------------------------------------------
const GameObject * Component::GetGameObject() const
{
	return game_object;
}

// ---------------------------------------------------------
GameObject * Component::GetGameObject()
{
	return game_object;
}
