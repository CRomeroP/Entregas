#include "Globals.h"
#include "PanelGOTree.h"
#include "Application.h"
#include "ModuleModelLoader.h"



PanelGOTree::PanelGOTree(const char* name) : Panel(name)
{
	active = true;
}


PanelGOTree::~PanelGOTree()
{
}

void PanelGOTree::Draw()
{
	ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(256.0f, 420.0f), ImGuiCond_FirstUseEver);

	if (ImGui::Begin("GameObjects Hierarchy"))
	{
		for (unsigned i = 0; i < App->model->meshList.size(); ++i)
		{
			unsigned flags = ImGuiTreeNodeFlags_Leaf;
			flags |= i == selected ? ImGuiTreeNodeFlags_Selected : 0;

			if (ImGui::TreeNodeEx(App->model->meshList[i].mesh->mName.C_Str(), flags))
			{
				if (ImGui::IsItemHoveredRect())
				{
					if (ImGui::IsMouseClicked(0))
					{
						selected = i;
					}
				}

				ImGui::TreePop();
			}
		}

		ImGui::End();
	}
}
