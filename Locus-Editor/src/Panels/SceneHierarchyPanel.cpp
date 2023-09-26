#include "SceneHierarchyPanel.h"

#include <ImGui/imgui.h>
#include <ImGui/imgui_internal.h>
#include <glm/gtc/type_ptr.hpp>

#include "Command/Command.h"
#include "Command/CommandHistory.h"
#include "Command/EntityCommands.h"

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
		if (ImGui::ImageButton((ImTextureID)(uintptr_t)m_PlusButton->GetRendererID(), { 15.0f, 15.0f }))
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
			m_ActiveScene->m_Registry.sort<TagComponent>([&](entt::entity lhs, entt::entity rhs)
				{
					return m_ActiveScene->m_Registry.get<TagComponent>(lhs).HierarchyPos < m_ActiveScene->m_Registry.get<TagComponent>(rhs).HierarchyPos;
				});

			auto view = m_ActiveScene->m_Registry.view<TagComponent>();
			for (auto e : view)
			{
				Entity entity = Entity(e, m_ActiveScene.get());
				if (entity.HasComponent<TransformComponent>())
				{
					if (entity.GetComponent<TransformComponent>().Parent == Entity::Null)
						DrawEntityNode(entity);
				}
			}

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
		bool openOnCreate = false;
		if (m_SelectedEntity && ImGui::BeginPopupContextItem()) // TODO: Create child when hovering too
		{
			if (ImGui::MenuItem("Create Empty Entity"))
			{
				CommandHistory::AddCommand(new CreateChildEntityCommand(m_ActiveScene, "Empty Entity", m_SelectedEntity));
				openOnCreate = true;
			}

			if (ImGui::MenuItem("Delete Entity"))
				entityDeleted = true;
			ImGui::EndPopup();
		}

		if (openOnCreate)
			ImGui::GetStateStorage()->SetBool(ImGui::GetFocusID(), true);

		if (opened)
		{
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
			if (entity.HasComponent<ChildComponent>())
			{
				auto& cc = entity.GetComponent<ChildComponent>();
				for (auto childEntity : cc.ChildEntities)
				{
					DrawEntityNode(childEntity);
				}
			}
			ImGui::TreePop();
		}

		// Process drag drop
		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
		{
			ImGui::SetDragDropPayload("ENTITY_POSITION", &entity, sizeof(Entity));
			ImGui::EndDragDropSource();
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY_POSITION"))
			{
				uint32_t curPos = entity.GetComponent<TagComponent>().HierarchyPos;
				Entity* payloadEntity = (Entity*)payload->Data;
				entity.GetComponent<TagComponent>().HierarchyPos = payloadEntity->GetComponent<TagComponent>().HierarchyPos;
				payloadEntity->GetComponent<TagComponent>().HierarchyPos = curPos;
			}
			ImGui::EndDragDropTarget();
		}


		if (entityDeleted)
		{
			CommandHistory::AddCommand(new DestroyEntityCommand(m_ActiveScene, entity));
			if (m_SelectedEntity == entity)
				m_SelectedEntity = {};
		}

	}
}