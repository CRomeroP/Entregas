#ifndef __ModuleModelLoader_h__
#define __ModuleModelLoader_h__

#include "Module.h"
#include "MathGeoLib.h"

class ModuleModelLoader :
	public Module
{
public:
	ModuleModelLoader();
	~ModuleModelLoader();
	bool            Init();
	update_status   Update();
	bool            CleanUp();

private:

	void            GenerateMeshData(const aiMesh* scene);
	void            GenerateMaterialData(const aiMaterial* scene);


};
#endif

