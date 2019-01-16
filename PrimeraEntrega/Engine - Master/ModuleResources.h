#ifndef __MODULERESOURCES_H__
#define __MODULERESOURCES_H__

#include "Globals.h"
#include "Module.h"
#include "Resources.h"
#include <map>
#include <vector>

#define RESERVED_RESOURCES 6 // cube/sphere/cylinde/cone/pyramid primitives + checker texture

class Resource;
class LoaderAnimation;
class ResourceMesh;
class ResourceTexture;

class ModuleResources : public Module
{
public:

	ModuleResources(bool start_enabled = true);

	// Destructor
	~ModuleResources();

	bool Init(Config* config);
	bool Start(Config* config);
	bool CleanUp();
	//void ReceiveEvent(const Event& event);

	void SaveResources() const;
	void LoadResources();

	Resource::Type TypeFromExtension(const char* extension) const;
	unsigned long long Find(const char* file_in_assets) const;
	unsigned long long ImportFileOutsideVFM(const char* full_path);
	unsigned long long ImportFile(const char* new_file_in_assets, bool force = false);
	unsigned long long ImportBuffer(const void* buffer, unsigned int size, Resource::Type type, const char* source_file = nullptr);
	unsigned long long ImportTexture(const char* file_name, bool compressed, bool mipmaps, bool srgb);

	unsigned long long GenerateNewUID();
	const Resource* Get(unsigned long long uid) const;
	Resource * Get(unsigned long long uid);
	Resource* CreateNewResource(Resource::Type type, unsigned long long force_uid = 0);
	//TODO: const Resource* Attach(GameObject* gameobject, UID uid));
	void GatherResourceType(std::vector<const Resource*>& resources, Resource::Type type) const;

	void RemoveResource(unsigned long long uid);

	const LoaderAnimation* GetAnimationLoader() const;

	const ResourceTexture* GetWhiteFallback() const { return white_fallback; }
	const ResourceTexture* GetBlackFallback() const { return black_fallback; }
private:

	void LoadUID();
	void SaveUID() const;
	unsigned long long ImportSuccess(Resource::Type type, const char* file_name, const std::string& output);

private:
	std::string asset_folder;
	unsigned long long last_uid = RESERVED_RESOURCES + 1; // reserve 1 for standard cube mesh
	std::map<unsigned long long, Resource*> resources;
	std::vector<Resource*> removed;
	LoaderAnimation* anim_loader = nullptr;

	// Presets
	ResourceMesh* cube = nullptr;
	ResourceMesh* sphere = nullptr;
	ResourceMesh* cone = nullptr;
	ResourceMesh* pyramid = nullptr;
	ResourceMesh* cylinder = nullptr;
	ResourceTexture* checkers = nullptr;
	ResourceTexture* white_fallback = nullptr;
	ResourceTexture* black_fallback = nullptr;
};

#endif // __MODULERESOURCES_H__
