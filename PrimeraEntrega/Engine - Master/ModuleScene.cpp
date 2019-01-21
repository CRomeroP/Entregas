#include "Globals.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ModuleFileSystem.h"
#include "GameObject.h"
#include "Config.h"

#include "ModuleRender.h"
#include "ModuleCamera.h"
#include "ModuleImGui.h"
#include "ModuleResources.h"

#include "ResourceModel.h"
#include "ResourceMesh.h"

#include "ComponentCamera.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"

#include "Event.h"

#include "mmgr/mmgr.h"

using namespace std;

ModuleScene::ModuleScene(bool start_enabled) : Module()
{

}

// Destructor
ModuleScene::~ModuleScene()
{
	
}

// Called before render is available
bool ModuleScene::Init(Config* config)
{
	bool ret = true;
	LOG("Loading Level Manager");

	// create an empty game object to be the root of everything
	root = new GameObject(nullptr, "root");
	quadtree.SetBoundaries(AABB(float3(-500, 0, -500), float3(500, 30, 500)));

	return ret;
}

bool ModuleScene::Start(Config * config)
{
	// Load a default map
	Load("default.eduscene");

	return true;
}

update_status ModuleScene::PreUpdate(float dt)
{
	DestroyFlaggedGameObjects();
	// Update transformations tree for this frame
	root->RecursiveCalcGlobalTransform(root->GetLocalTransform(), false);
	root->RecursiveCalcBoundingBoxes();

	return UPDATE_CONTINUE;
}

update_status ModuleScene::Update(float dt)
{
	RecursiveUpdate(root, dt);

	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleScene::CleanUp()
{
	LOG("Freeing Level Manager");

	// This recursively must destroy all gameobjects
	RELEASE(root);

	RemoveLights();

	return true;
}

void ModuleScene::ReceiveEvent(const Event & event)
{
	RecursiveProcessEvent(root, event);
}

const GameObject * ModuleScene::GetRoot() const
{
	return root;
}

GameObject * ModuleScene::GetRoot()
{
	return root;
}

const GameObject * ModuleScene::Find(unsigned int uid) const
{
	if (uid > 0)
		return RecursiveFind(uid, root);

	return nullptr;
}

GameObject * ModuleScene::Find(unsigned int uid)
{
	if (uid > 0)
		return RecursiveFind(uid, root);

	return nullptr;
}

bool ModuleScene::CreateNewEmpty(const char * name)
{
	UnloadCurrent();
	return false;
}

GameObject * ModuleScene::CreateGameObject(GameObject * parent, const float3 & pos, const float3 & scale, const Quat & rot, const char * name)
{
	GameObject* ret = new GameObject(parent, name, pos, scale, rot);

	return ret;
}

GameObject * ModuleScene::CreateGameObject(GameObject * parent)
{
	if (parent == nullptr)
		parent = root;

	return new GameObject(parent, "Unnamed");
}

GameObject * ModuleScene::Duplicate(const GameObject * original)
{
	GameObject* ret = nullptr;

	if (original != nullptr)
	{
		Config save;
		save.AddArray("Game Objects");
		map<unsigned int, unsigned int> new_uids;
		original->Save(save, &new_uids);

		LoadGameObjects(save);
	}

	return ret;
}

void ModuleScene::DestroyFlaggedGameObjects()
{
	// we never really have to remove root, but we remove all its childs
	if (root->remove == true)
	{
		for (list<GameObject*>::iterator it = root->childs.begin(); it != root->childs.end(); ++it)
			(*it)->remove = true;
		root->remove = false;
	}

	// Find parent and cut connection
	if (root->RecursiveRemoveFlagged())
	{
		// Notify everybody that a GameObject has been destroyed
		// this gives the oportunity to other modules to Validate()
		// their pointers to GameObjects
		Event event(Event::gameobject_destroyed);
		App->BroadcastEvent(event);
	}
}

void ModuleScene::LoadGameObjects(const Config & config)
{
	int count = config.GetArrayCount("Game Objects");
	map<GameObject*, unsigned int> relations;
	for (int i = 0; i < count; ++i)
	{
		GameObject* go = CreateGameObject();
		go->Load(&config.GetArray("Game Objects", i), relations);
	}

	// Second pass to tide up the hierarchy
	for (map<GameObject*, unsigned int>::iterator it = relations.begin(); it != relations.end(); ++it)
	{
		unsigned int parent_id = it->second;
		GameObject* go = it->first;

		if (parent_id > 0)
		{
			GameObject* parent_go = Find(parent_id);
			if (parent_go != nullptr)
				go->SetNewParent(parent_go);
		}
	}

	// Reset all info about the level (this also fill in the quadtree)
	root->RecursiveCalcGlobalTransform(root->GetLocalTransform(), true);
	root->RecursiveCalcBoundingBoxes();

	// Third pass: call OnStart on all new GameObjects
	for (map<GameObject*, unsigned int>::iterator it = relations.begin(); it != relations.end(); ++it)
		it->first->OnStart();
}


bool ModuleScene::Load(const char * file)
{
	bool ret = false;

	if (file != nullptr)
	{
		int len = strlen(file);

		char* buffer = nullptr;
		unsigned int size = App->fs->Load(file, &buffer);

		if (buffer != nullptr && size > 0)
		{
			Config config(buffer);

			// Load level description
			Config desc(config.GetSection("Description"));
			name = desc.GetString("Name", "Unnamed level");
			//App->hints->Init(&desc);
			//App->camera->Load(&desc);

			LoadLights(config.GetSection("Lights"));
			LoadGameObjects(config);

		}

		RELEASE_ARRAY(buffer);
	}

	return ret;
}

bool ModuleScene::Save(const char * file)
{
	bool ret = true;

	Config save;

	// Add header info
	Config desc(save.AddSection("Description"));
	desc.AddString("Name", name.c_str());
	//App->hints->Save(&desc);
	//App->camera->Save(&desc);

	SaveLights(save.AddSection("Lights"));

	// Serialize GameObjects recursively
	save.AddArray("Game Objects");

	for (list<GameObject*>::const_iterator it = root->childs.begin(); it != root->childs.end(); ++it)
		(*it)->Save(save);

	// Finally save to file
	char* buf = nullptr;
	unsigned int size = save.Save(&buf, "Level save file from EDU Engine");
	App->fs->Save(file, buf, size);
	RELEASE_ARRAY(buf);

	return ret;
}

void ModuleScene::UnloadCurrent()
{
}

void ModuleScene::RecursiveProcessEvent(GameObject * go, const Event & event) const
{
	switch (event.type)
	{
	case Event::EventType::play: go->OnPlay(); break;
	case Event::EventType::stop: go->OnStop(); break;
	case Event::EventType::pause: go->OnPause(); break;
	case Event::EventType::unpause: go->OnUnPause(); break;
	case Event::EventType::gameobject_destroyed: go->OnGoDestroyed(); break;
	}

	for (list<GameObject*>::const_iterator it = go->childs.begin(); it != go->childs.end(); ++it)
		RecursiveProcessEvent(*it, event);
}

void ModuleScene::RecursiveUpdate(GameObject * go, float dt) const
{
	go->OnUpdate(dt);

	for (list<GameObject*>::const_iterator it = go->childs.begin(); it != go->childs.end(); ++it)
		RecursiveUpdate(*it, dt);
}

GameObject* ModuleScene::RecursiveFind(unsigned int uid, GameObject * go) const
{
	if (uid == go->GetUID())
		return go;

	GameObject* ret = nullptr;

	for (list<GameObject*>::const_iterator it = go->childs.begin(); it != go->childs.end() && ret == nullptr; ++it)
		ret = RecursiveFind(uid, *it);

	return ret;
}

GameObject * ModuleScene::Validate(const GameObject * pointer) const
{
	if (pointer == root)
		return root;

	for (list<GameObject*>::const_iterator it = root->childs.begin(); it != root->childs.end(); ++it)
		if (pointer == *it)
			return (GameObject *)pointer;

	return nullptr;
}

GameObject* ModuleScene::CastRay(const LineSegment& segment, float& dist) const
{
	dist = inf;
	GameObject* candidate = nullptr;
	RecursiveTestRay(segment, dist, &candidate);
	return candidate;
}

void ModuleScene::RecursiveTestRayBBox(const LineSegment & segment, float & dist, float3 & normal, GameObject ** best_candidate) const
{
	map<float, GameObject*> objects;
	quadtree.CollectIntersections(objects, segment);

	for (map<float, GameObject*>::const_iterator it = objects.begin(); it != objects.end(); ++it)
	{
		// Look for meshes
		GameObject* go = it->second;
		if (go->HasComponent(Component::TypeComponent::Mesh) == true)
		{
			float closer = inf;
			*best_candidate = (GameObject*)go;
			dist = it->first;

			// let's find out the plane that hit the segment and fill in the normal
			for (int i = 0; i < 6; ++i)
			{
				Plane p(go->global_bbox.FacePlane(i));
				float d;
				if (p.Intersects(segment, &d))
				{
					if (d < closer)
						normal = p.normal;
				}
			}
		}
	}
}

void ModuleScene::RecursiveTestRay(const LineSegment& segment, float& dist, GameObject** best_candidate) const
{
	map<float, GameObject*> objects;
	quadtree.CollectIntersections(objects, segment);

	for (map<float, GameObject*>::const_iterator it = objects.begin(); it != objects.end(); ++it)
	{
		// Look for meshes, nothing else can be "picked" from the screen
		GameObject* go = it->second;
		vector<Component*> meshes;
		go->FindComponents(Component::TypeComponent::Mesh, meshes);

		if (meshes.size() > 0)
		{
			const ComponentMesh* cmesh = (const ComponentMesh*)meshes[0];
			const ResourceMesh* mesh = (ResourceMesh*)cmesh->GetResource();

			// test all triangles
			LineSegment segment_local_space(segment);
			segment_local_space.Transform(go->GetGlobalTransformation().Inverted());

			Triangle tri;
			for (unsigned int i = 0; i < mesh->num_indices;)
			{
				tri.a = mesh->src_vertices[mesh->src_indices[i++] * 3];
				tri.b = mesh->src_vertices[mesh->src_indices[i++] * 3];
				tri.c = mesh->src_vertices[mesh->src_indices[i++] * 3];

				float distance;
				float3 hit_point;
				if (segment_local_space.Intersects(tri, &distance, &hit_point))
				{
					if (distance < dist)
					{
						dist = distance;
						*best_candidate = (GameObject*)go;
					}
				}
			}
		}
	}
}

GameObject* ModuleScene::CastRay(const Ray & ray, float& dist) const
{
	dist = inf;
	GameObject* candidate = nullptr;
	RecursiveTestRay(ray, dist, &candidate);
	return candidate;
}

GameObject * ModuleScene::CastRayOnBoundingBoxes(const LineSegment & segment, float & dist, float3 & normal) const
{
	dist = inf;
	normal = float3::zero;
	GameObject* candidate = nullptr;
	RecursiveTestRayBBox(segment, dist, normal, &candidate);
	return candidate;
}

void ModuleScene::RecursiveTestRay(const Ray& ray, float& dist, GameObject** best_candidate) const
{
	map<float, GameObject*> objects;
	quadtree.CollectIntersections(objects, ray);

	for (map<float, GameObject*>::const_iterator it = objects.begin(); it != objects.end(); ++it)
	{
		// Look for meshes, nothing else can be "picked" from the screen
		GameObject* go = it->second;

		vector<Component*> meshes;
		go->FindComponents(Component::TypeComponent::Mesh, meshes);

		if (meshes.size() > 0)
		{
			const ComponentMesh* cmesh = (const ComponentMesh*)meshes[0];
			const ResourceMesh* mesh = (ResourceMesh*)cmesh->GetResource();

			// test all triangles
			Ray ray_local_space(ray);
			ray_local_space.Transform(go->GetGlobalTransformation().Inverted());
			ray_local_space.dir.Normalize();

			// Experiment using a TriangleMesh instead of raw triangles
			Triangle tri;
			for (unsigned int i = 0; i < mesh->num_indices;)
			{
				tri.a = mesh->src_vertices[mesh->src_indices[i++] * 3];
				tri.b = mesh->src_vertices[mesh->src_indices[i++] * 3];
				tri.c = mesh->src_vertices[mesh->src_indices[i++] * 3];
				// TODO I got a bug twice here, looks like a problem creating the triangle

				float distance;
				float3 hit_point;
				if (ray_local_space.Intersects(tri, &distance, &hit_point))
				{
					if (distance < dist)
					{
						dist = distance;
						*best_candidate = (GameObject*)go;
					}
				}
			}
		}
	}
}

void ModuleScene::FindNear(const float3 & position, float radius, std::vector<GameObject*>& results) const
{
	quadtree.CollectIntersections(results, Sphere(position, radius));
}

GameObject* ModuleScene::AddModel(unsigned long long id)
{
	Resource* res = App->resources->Get(id);

	bool ok = res->GetType() == Resource::model;
	std::vector<GameObject*> gos;

	if (ok)
	{
		ResourceModel* model = static_cast<ResourceModel*>(res);
		model->LoadToMemory();

		gos.reserve(model->GetNumNodes());

		for (unsigned int i = 0, count = model->GetNumNodes(); ok && i < count; ++i)
		{
			const ResourceModel::Node& node = model->GetNode(i);

			GameObject* parent = i == 0 ? nullptr : gos[node.parent];
			GameObject* go = CreateGameObject(parent);

			go->SetLocalTransform(node.transform);
			go->name = node.name.c_str();

			if (node.mesh != 0)
			{
				ComponentMesh* mesh = new ComponentMesh(go);
				ok = mesh->SetResource(node.mesh);
				go->components.push_back(mesh);
			}

			if (ok && node.material != 0)
			{
				ComponentMaterial* material = new ComponentMaterial(go);
				ok = material->SetResource(node.material);
				go->components.push_back(material);
			}

			gos.push_back(go);
		}

		model->Release();

	}

	GameObject* ret = nullptr;

	if (!gos.empty())
	{
		if (!ok)
		{
			gos.front()->Remove();
		}
		else
		{
			ret = gos.front();
		}
	}

	return ret;
}
