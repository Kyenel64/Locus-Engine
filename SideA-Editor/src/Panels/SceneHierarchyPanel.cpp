#include "SceneHierarchyPanel.h"

#include <ImGui/imgui.h>
#include <glm/gtc/type_ptr.hpp>

#include "SideA/Scene/Components.h"

namespace SideA
{
	SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& context)
	{
		SetContext(context);
	}

	void SceneHierarchyPanel::SetContext(const Ref<Scene>& context)
	{
		m_Context = context;
	}

	void SceneHierarchyPanel::OnImGuiRender()
	{
		if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
			m_SelectedEntity = {};

		// --- Scene Hierarchy Panel ---
		ImGui::Begin("Scene Hierarchy");
		m_Context->m_Registry.each([&](auto entityID)
			{
				Entity entity(entityID, m_Context.get());
				DrawEntityNode(entity);
			});
		ImGui::End();

		// --- Properties Panel ---
		ImGui::Begin("Properties");
		if (m_SelectedEntity)
			DrawComponents(m_SelectedEntity);
		ImGui::End();
	}

	void SceneHierarchyPanel::DrawEntityNode(Entity entity)
	{
		auto& tag = entity.GetComponent<TagComponent>().Tag;

		ImGuiTreeNodeFlags flags = ((m_SelectedEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0)
			| ImGuiTreeNodeFlags_OpenOnArrow;
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());

		if (ImGui::IsItemClicked())
		{
			m_SelectedEntity = entity;
		}

		if (opened)
		{
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
			// temp
			bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());
			if (opened)
				ImGui::TreePop();

			ImGui::TreePop();
		}
	}

	void SceneHierarchyPanel::DrawComponents(Entity entity)
	{
		ImGui::Begin("Properties");

		// --- Tag Component ---
		if (entity.HasComponent<TagComponent>())
		{
			auto& tag = entity.GetComponent<TagComponent>().Tag;
			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			strcpy_s(buffer, sizeof(buffer), tag.c_str());
			if (ImGui::InputText("Tag", buffer, sizeof(buffer)))
			{
				tag = std::string(buffer);
			}
		}

		// --- Transform Component ---
		if (entity.HasComponent<TransformComponent>())
		{
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen;
			if (ImGui::TreeNodeEx((void*)typeid(TransformComponent).hash_code(), flags, "Transform"))
			{
				auto& transform = entity.GetComponent<TransformComponent>().Transform;
				ImGui::DragFloat3("Position", glm::value_ptr(transform[3]), 0.1f);

				ImGui::TreePop();
			}
		}


		ImGui::End();
	}

}