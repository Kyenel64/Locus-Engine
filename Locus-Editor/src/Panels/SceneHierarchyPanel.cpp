#include "SceneHierarchyPanel.h"

#include <ImGui/imgui.h>
#include <ImGui/imgui_internal.h>
#include <glm/gtc/type_ptr.hpp>

#include "Command/Command.h"

namespace Locus
{
	extern const std::filesystem::path g_ProjectPath;

	SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& context)
	{
		SetContext(context);
	}

	void SceneHierarchyPanel::SetContext(const Ref<Scene>& context)
	{
		m_ActiveScene = context;
		m_SelectedEntity = {};
		m_PlusButton = Texture2D::Create("resources/icons/PlusButton.png");
	}

	void SceneHierarchyPanel::OnImGuiRender()
	{
		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoScrollbar;
		//ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { });
		ImGui::Begin(" Scene Hierarchy ", false, windowFlags);

		// --- Scene Hierarchy Panel ------------------------------------------
		// Top bar
		ImGui::PushStyleColor(ImGuiCol_Button, LocusColors::Transparent);
		if (ImGui::ImageButton((ImTextureID)m_PlusButton->GetRendererID(), { 15.0f, 15.0f }))
		{
			// TODO: Functionality
		}
		ImGui::PopStyleColor();

		ImGui::SameLine();

		char buffer[256];
		memset(buffer, 0, sizeof(buffer));
		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
		ImGui::InputText("##Search", buffer, sizeof(buffer), ImGuiInputTextFlags_None); // TODO: Functionality
		ImGui::PopItemWidth();

		// Hierarchy panel
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);
		ImGui::PushStyleColor(ImGuiCol_ChildBg, LocusColors::Grey);
		ImGui::BeginChild("Hierarchy");
		
		if (m_ActiveScene)
		{
			// Display each entity
			m_ActiveScene->m_Registry.each([&](auto entityID)
				{
					Entity entity = Entity(entityID, m_ActiveScene.get());
					DrawEntityNode(entity);
				});

			// Select nothing if clicking in blank space
			if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered() || !m_SelectedEntity.IsValid())
				m_SelectedEntity = {};

			// Open pop up menu when right clicking on blank space.
			if (ImGui::BeginPopupContextWindow(0, 1 | ImGuiPopupFlags_NoOpenOverItems))
			{
				if (ImGui::MenuItem("Create Empty Entity"))
					CommandHistory::AddCommand(new CreateEntityCommand(m_ActiveScene, "Empty Entity"));
				ImGui::EndPopup();
			}
		}

		ImGui::EndChild();
		ImGui::PopStyleColor();
		ImGui::PopStyleVar();

		ImGui::End();
	}

	void SceneHierarchyPanel::DrawEntityNode(Entity entity)
	{
		auto& tag = entity.GetComponent<TagComponent>().Tag;

		ImGuiTreeNodeFlags flags = ((m_SelectedEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0)
			| ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0.0f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, LocusColors::Tan);
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());
		ImGui::PopStyleVar();
		ImGui::PopStyleColor();

		if (ImGui::IsItemClicked())
			m_SelectedEntity = entity;

		bool entityDeleted = false;
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Delete Entity"))
				entityDeleted = true;
			ImGui::EndPopup();
		}

		if (opened)
		{
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
			// temp
			bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());
			if (opened)
				ImGui::TreePop();

			ImGui::TreePop();
		}

		if (entityDeleted)
		{
			CommandHistory::AddCommand(new DestroyEntityCommand(m_ActiveScene, entity));
			if (m_SelectedEntity == entity)
				m_SelectedEntity = {};
		}
	}
}