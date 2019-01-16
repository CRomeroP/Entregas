#include "Globals.h"
#include "Application.h"
#include "ModuleResources.h"
#include "ModuleFileSystem.h"
#include "ModuleTextures.h"
#include "Event.h"
//#include "ResourceTexture.h"
//#include "ResourceMaterial.h"
#include "ResourceMesh.h"
//#include "ResourceModel.h"
#include "Config.h"
#include <string>

#include "mmgr/mmgr.h"

#define LAST_UID_FILE "LAST_UID"

using namespace std;

ModuleResources::ModuleResources(bool start_enabled) : Module("Resource Manager", start_enabled), asset_folder(ASSETS_FOLDER)
{
	
}

// Destructor
ModuleResources::~ModuleResources()
{
	
}

// Called before render is available
bool ModuleResources::Init(Config* config)
{
	LOG("Loading Resource Manager");
	bool ret = true;

	LoadUID();
	LoadResources();

	return ret;
}

bool ModuleResources::Start(Config * config)
{
	checkers = (ResourceTexture*)CreateNewResource(Resource::Type::texture, 2);
	App->textures->LoadCheckers(checkers);
	checkers->loaded = 1;

	white_fallback = new ResourceTexture(0);
	black_fallback = new ResourceTexture(0);

	App->textures->LoadFallback(white_fallback, float3(1.0f));
	App->textures->LoadFallback(black_fallback, float3(0.0f));

	return true;
}

// Called before quitting
bool ModuleResources::CleanUp()
{
	LOG("Unloading Resource Manager");

	SaveResources();

	for (map<unsigned long long, Resource*>::iterator it = resources.begin(); it != resources.end(); ++it)
		RELEASE(it->second);

	for (vector<Resource*>::iterator it = removed.begin(); it != removed.end(); ++it)
		RELEASE(*it);

	resources.clear();

	delete white_fallback;
	delete black_fallback;

	return true;
}

void ModuleResources::ReceiveEvent(const Event& event)
{
	switch (event.type)
	{
	case Event::file_dropped:
		LOG("File dropped: %s", event.string.ptr);
		ImportFileOutsideVFM(event.string.ptr);
		break;
	}
}

void ModuleResources::SaveResources() const
{
	bool ret = true;

	Config save;

	// Add header info
	Config desc(save.AddSection("Header"));

	// Serialize GameObjects recursively
	save.AddArray("Resources");

	for (map<unsigned long long, Resource*>::const_iterator it = resources.begin(); it != resources.end(); ++it)
	{
		if (it->first > RESERVED_RESOURCES)
		{
			Config resource;
			it->second->Save(resource);
			save.AddArrayEntry(resource);
		}
	}

	// Finally save to file
	char* buf = nullptr;
	unsigned int size = save.Save(&buf, "Resources setup from the EDU Engine");
	App->fs->Save(SETTINGS_FOLDER "resources.json", buf, size);
	RELEASE_ARRAY(buf);
}

void ModuleResources::LoadResources()
{
	char* buffer = nullptr;
	unsigned int size = App->fs->Load(SETTINGS_FOLDER "resources.json", &buffer);

	if (buffer != nullptr && size > 0)
	{
		Config config(buffer);

		// Load level description
		Config desc(config.GetSection("Header"));

		int count = config.GetArrayCount("Resources");
		for (int i = 0; i < count; ++i)
		{
			Config resource(config.GetArray("Resources", i));
			Resource::Type type = (Resource::Type) resource.GetInt("Type");
			unsigned long long uid = resource.GetUID("UID");

			if (Get(uid) != nullptr)
			{
				LOG("Skipping suplicated resource id %llu", uid);
				continue;
			}

			Resource* res = CreateNewResource(type, uid);
			res->Load(config.GetArray("Resources", i));
		}
		RELEASE_ARRAY(buffer);
	}
}

Resource::Type ModuleResources::TypeFromExtension(const char * extension) const
{
	Resource::Type ret = Resource::unknown;

	if (extension != nullptr)
	{
		
		if (_stricmp(extension, "dds") == 0)
			ret = Resource::texture;
		else if (_stricmp(extension, "png") == 0)
			ret = Resource::texture;
		else if (_stricmp(extension, "jpg") == 0)
			ret = Resource::texture;
		else if (_stricmp(extension, "tga") == 0)
			ret = Resource::texture;
		else if (_stricmp(extension, "tif") == 0)
			ret = Resource::texture;
		else if (_stricmp(extension, "fbx") == 0)
			ret = Resource::model;
		else if (_stricmp(extension, "dae") == 0)
			ret = Resource::model;
	}

	return ret;
}

unsigned long long ModuleResources::Find(const char * file_in_assets) const
{
	string file(file_in_assets);
	App->fs->NormalizePath(file);

	for (map<unsigned long long, Resource*>::const_iterator it = resources.begin(); it != resources.end(); ++it)
	{
		if (it->second->file.compare(file) == 0)
			return it->first;
	}
	return 0;
}

unsigned long long ModuleResources::ImportFileOutsideVFM(const char * full_path)
{
	unsigned long long ret = 0;

	string final_path;

	App->fs->SplitFilePath(full_path, nullptr, &final_path);
	final_path = asset_folder + final_path;

	if (App->fs->CopyFromOutsideFS(full_path, final_path.c_str()) == true)
		ret = ImportFile(final_path.c_str());

	return ret;
}

unsigned long long  ModuleResources::ImportFile(const char * new_file_in_assets, bool force)
{
	unsigned long long ret = 0;

	// Check is that file has been already exported
	if (force == true)
	{
		ret = Find(new_file_in_assets);

		if (ret != 0)
			return ret;
	}

	// Find out the type from the extension and send to the correct exporter
	string extension;
	App->fs->SplitFilePath(new_file_in_assets, nullptr, nullptr, &extension);

	Resource::Type type = TypeFromExtension(extension.c_str());

	bool import_ok = false;
	string written_file;

	switch (type)
	{
	case Resource::texture:
		import_ok = App->textures->Import(new_file_in_assets, "", written_file, true);
		break;
	case Resource::model:
		import_ok = ResourceModel::Import(new_file_in_assets, written_file);
		break;
	}

	// If export was successfull, create a new resource
	if (import_ok == true)
	{
		ret = ImportSuccess(type, new_file_in_assets, written_file);
	}
	else
		LOG("Importing of [%s] FAILED", new_file_in_assets);

	return ret;
}

unsigned int ModuleResources::ImportTexture(const char* file_name, bool compressed, bool mipmaps, bool srgb)
{
	unsigned int ret = 0;
	bool import_ok = false;
	string written_file;

	import_ok = App->tex->Import(file_name, "", written_file, compressed);

	// If export was successfull, create a new resource
	if (import_ok == true)
	{
		ret = ImportSuccess(Resource::texture, file_name, written_file);
		ResourceTexture* texture = static_cast<ResourceTexture*>(Get(ret));
		texture->EnableMips(mipmaps);
		texture->SetLinear(!srgb);
	}
	else
	{
		LOG("Importing of [%s] FAILED", file_name);
	}

	return ret;
}

unsigned long long ModuleResources::ImportSuccess(Resource::Type type, const char* file_name, const std::string& output)
{
	Resource* res = CreateNewResource(type);
	res->file = file_name;
	App->fs->NormalizePath(res->file);
	string file;
	App->fs->SplitFilePath(output.c_str(), nullptr, &file);
	res->exported_file = file.c_str();
	LOG("Imported successful from [%s] to [%s]", res->GetFile(), res->GetExportedFile());

	App->fs->SplitFilePath(res->file.c_str(), nullptr, &res->user_name, nullptr);

	if (res->user_name.empty())
	{
		res->user_name = res->exported_file;
	}

	size_t pos_dot = res->user_name.find_last_of(".");
	if (pos_dot != std::string::npos)
	{
		res->user_name.erase(res->user_name.begin() + pos_dot, res->user_name.end());
	}

	return res->uid;
}

unsigned long long ModuleResources::ImportBuffer(const void * buffer, unsigned int size, Resource::Type type, const char* source_file)
{
	unsigned long long ret = 0;

	bool import_ok = false;
	string output;


	switch (type)
	{
	case Resource::texture:
		import_ok = App->tex->Import(buffer, size, output, true);
		break;
	case Resource::mesh:
		// Old school trick: if it is a Mesh, buffer will be treated as an AiMesh*
		// TODO: this can go bad in so many ways :)
		// \todo: import_ok = App->meshes->Import((aiMesh*) buffer, output);
	}

	// If export was successfull, create a new resource
	if (import_ok == true)
	{
		Resource* res = CreateNewResource(type);
		if (source_file != nullptr) {
			res->file = source_file;
			App->fs->NormalizePath(res->file);
		}
		string file;
		App->fs->SplitFilePath(output.c_str(), nullptr, &file);
		res->exported_file = file;
		ret = res->uid;
		LOG("Imported successful from BUFFER [%s] to [%s]", res->GetFile(), res->GetExportedFile());
	}
	else
		LOG("Importing of BUFFER [%s] FAILED", source_file);

	return ret;
}

unsigned long long ModuleResources::GenerateNewUID()
{
	++last_uid;
	SaveUID();
	return last_uid;
}

const Resource * ModuleResources::Get(unsigned long long uid) const
{
	if (resources.find(uid) != resources.end())
		return resources.at(uid);
	return nullptr;
}

Resource * ModuleResources::Get(unsigned long long uid)
{
	std::map<unsigned long long, Resource*>::iterator it = resources.find(uid);
	if (it != resources.end())
		return it->second;
	return nullptr;
}

Resource * ModuleResources::CreateNewResource(Resource::Type type, unsigned long long force_uid)
{
	Resource* ret = nullptr;
	unsigned long long uid;

	if (force_uid != 0 && Get(force_uid) == nullptr)
		uid = force_uid;
	else
		uid = GenerateNewUID();

	switch (type)
	{
	case Resource::texture:
		ret = (Resource*) new ResourceTexture(uid);
		break;
	case Resource::mesh:
		ret = (Resource*) new ResourceMesh(uid);
		break;
	case Resource::material:
		ret = new ResourceMaterial(uid);
		break;
	case Resource::model:
		ret = new ResourceModel(uid);
		break;
	}

	if (ret != nullptr)
	{
		resources[uid] = ret;
	}

	return ret;
}

void ModuleResources::GatherResourceType(std::vector<const Resource*>& resources, Resource::Type type) const
{
	for (map<unsigned long long, Resource*>::const_iterator it = this->resources.begin(); it != this->resources.end(); ++it)
	{
		if (it->second->type == type)
			resources.push_back(it->second);
	}
}

const LoaderAnimation * ModuleResources::GetAnimationLoader() const
{
	return anim_loader;
}

void ModuleResources::LoadUID()
{
	string file(SETTINGS_FOLDER);
	file += LAST_UID_FILE;

	char *buf = nullptr;
	unsigned int size = App->fs->Load(file.c_str(), &buf);

	if (size == sizeof(last_uid))
	{
		last_uid = *((unsigned long long*)buf);
		RELEASE_ARRAY(buf);
	}
	else
	{
		LOG("WARNING! Cannot read resource UID from file [%s] - Generating a new one", file.c_str());
		SaveUID();
	}
}

void ModuleResources::SaveUID() const
{
	string file(SETTINGS_FOLDER);
	file += LAST_UID_FILE;

	unsigned int size = App->fs->Save(file.c_str(), (const char*)&last_uid, sizeof(last_uid));

	if (size != sizeof(last_uid))
		LOG("WARNING! Cannot write resource UID into file [%s]", file.c_str());
}

void ModuleResources::RemoveResource(unsigned long long uid)
{
	map<unsigned long long, Resource*>::iterator it = resources.find(uid);
	if (it != resources.end())
	{
		App->fs->Remove(it->second->GetExportedFile());

		removed.push_back(it->second);

		resources.erase(it);
	}
}

