#ifndef __RESOURCE_H__
#define __RESOURCE_H__

// Base class for all possible game resources

#include "Globals.h"
#include <string>

class Config;

class Resource
{
	friend class ModuleResources;
public:
	enum Type {
		model,
		material,
		texture,
		mesh,
		unkown
	
	};

public:
	Resource(unsigned long long uid, Resource::Type type);
	virtual ~Resource();

	Resource::Type      GetType() const;
	const char*         GetTypeStr() const;
	unsigned long long  GetUID() const;
	const char*         GetFile() const;
	const char*         GetExportedFile() const;
	const char*         GetName() const { return user_name.c_str(); }

	bool                IsLoadedToMemory() const;
	bool                LoadToMemory();
	void                Release();

	unsigned int        CountReferences() const;

	virtual void        Save(Config& config) const;
	virtual void        Load(const Config& config);

	static const char*  GetTypeStr(Type type);

protected:

	virtual bool    LoadInMemory() = 0;
	virtual void    ReleaseFromMemory() = 0;

protected:

	unsigned long long uid = 0;
	std::string file;
	std::string exported_file;
	std::string user_name;

	Type type = unkown;
	unsigned int loaded = 0;
};

#endif // __Resource_H__
