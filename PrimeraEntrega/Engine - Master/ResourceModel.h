#ifndef __RESOURCE_MODEL_H__
#define __RESOURCE_MODEL_H__

#include "Resources.h"
#include "MathGeoLib.h"

struct aiScene;
struct aiNode;

class ResourceModel : public Resource
{
public:

	struct Node
	{
		std::string         name;
		float4x4            transform = float4x4::identity;
		unsigned int        parent = 0;
		unsigned long long  mesh = 0;
		unsigned long long  material = 0;
	};

public:

	ResourceModel(unsigned long long id);
	virtual ~ResourceModel();

	void        Save(Config& config) const override;
	void        Load(const Config& config) override;

	bool        LoadInMemory() override;
	void        ReleaseFromMemory() override;
	bool        Save(std::string& output) const;

	static bool Import(const char* full_path, std::string& output);

	unsigned    GetNumNodes() const { return nodes.size(); }
	const Node& GetNode(unsigned int index) const { return nodes[index]; }

private:

	void        GenerateNodes(const aiScene* model, const aiNode* node, unsigned int parent, const std::vector<unsigned long long>& meshes, const std::vector<unsigned long long>& materials);
	void        GenerateMaterials(const aiScene* scene, const char* file, std::vector<unsigned long long>& materials);
	void        GenerateMeshes(const aiScene* scene, const char* file, std::vector<unsigned long long>& meshes);


private:

	std::vector<Node> nodes;

};

#endif /* __RESOURCE_MODEL_H__ */
