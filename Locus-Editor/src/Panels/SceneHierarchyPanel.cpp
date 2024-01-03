#include "SceneHierarchyPanel.h"

#include <ImGui/imgui.h>
#include <ImGui/imgui_internal.h>
#include <glm/gtc/type_ptr.hpp>

#include "Command/Command.h"
#include "Command/CommandHistory.h"
#include "Command/EntityCommands.h"

namespace Locus
{
	extern Entity g_SelectedEntity;

	SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& context)
	{
		SetScene(context);
	}

	void SceneHierarchyPanel::SetScene(const Ref<Scene>& context)
	{
		m_ActiveScene = context;
		m_PlusIcon = Texture2D::Create("resources/icons/PlusIcon.png");
	}

	void SceneHierarchyPanel::OnImGuiRender()
	{
		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_TabBarAlignLeft | ImGuiWindowFlags_DockedWindowBorder;
		ImGui::Begin("Scene Hierarchy", false, windowFlags);

		// --- Top bar ---
		// Plus button
		ImVec2 textSize = ImGui::CalcTextSize("Search");
		ImGui::PushStyleColor(ImGuiCol_Button, LocusColors::Transparent);
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { ImGui::GetStyle().FramePadding.y, ImGui::GetStyle().FramePadding.y });
		if (ImGui::ImageButton((ImTextureID)(uintptr_t)m_PlusIcon->GetRendererID(), { textSize.y, textSize.y }, { 0, 1 }, { 1, 0 }))
			ImGui::OpenPopup("ButtonPopup");
		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor();

		// Popup when clicking plus button. 
		ImGui::SetNextWindowPos({ ImGui::GetWindowPos().x + ImGui::GetCursorPosX(), ImGui::GetWindowPos().y + ImGui::GetCursorPosY() - ImGui::GetStyle().ItemSpacing.y });
		if (ImGui::BeginPopup("ButtonPopup"))
		{
			if (ImGui::MenuItem("Create Empty Entity"))
				CommandHistory::AddCommand(new CreateEntityCommand(m_ActiveScene, "Empty Entity"));
			ImGui::EndPopup();
		}

		ImGui::SameLine();

		// Search bar
		ImGuiTextFilter filter;
		bool showSearchText = true;
		float inputPosY = ImGui::GetCursorPosY();

		char buffer[256];
		memset(buffer, 0, sizeof(buffer));
		float width = ImGui::GetWindowSize().x * 0.5f;
		ImGui::SetCursorPosX(width - (width * 0.5f));
		ImGui::PushItemWidth(width);
		if (filter.Draw("##Search"))
			showSearchText = false;
		ImGui::PopItemWidth();
		// Draw "Search" text
		if (showSearchText)
		{
			ImDrawList* drawList = ImGui::GetWindowDrawList();
			drawList->AddText({ ImGui::GetWindowPos().x + (ImGui::GetWindowSize().x - textSize.x) * 0.5f, ImGui::GetWindowPos().y + inputPosY + ImGui::GetStyle().FramePadding.y }, 
				ImGui::GetColorU32(LocusColors::Orange), "Search");
		}


		// --- Hierarchy ---
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });
		ImGui::PushStyleColor(ImGuiCol_ChildBg, LocusColors::DarkGrey);
		ImGui::BeginChild("Hierarchy", { -1.0f, -1.0f }, true);
		ImGui::PopStyleColor();
		ImGui::PopStyleVar();
		
		// Display each entity
		if (m_ActiveScene)
		{
			auto view = m_ActiveScene->GetEntitiesWith<TagComponent>();
			for (auto e : view)
			{
				Entity entity = Entity(e, m_ActiveScene.get());
				if (entity.HasComponent<TagComponent>()) 
				{
					if (filter.PassFilter(entity.GetComponent<TagComponent>().Tag.c_str()))
					{
						if (!entity.GetComponent<TransformComponent>().Parent)
							DrawEntityNode(entity);
					}
				}
			}
		}

		// Select nothing if clicking in blank space
		if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered() || !g_SelectedEntity.IsValid())
			g_SelectedEntity = {};

		// Popup when clicking in empty space.
		if (ImGui::BeginPopupContextWindow("HierarchyPopup", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems))
		{
			if (ImGui::MenuItem("Create Empty Entity"))
				CommandHistory::AddCommand(new CreateEntityCommand(m_ActiveScene, "Empty Entity"));
			ImGui::EndPopup();
		}

		ImGui::EndChild();

		ImGui::End();
	}

	void SceneHierarchyPanel::DrawEntityNode(Entity entity)
	{
		auto& tag = entity.GetComponent<TagComponent>().Tag;

		ImGuiTreeNodeFlags flags = ((g_SelectedEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0)
			| ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Framed;
		ImVec4 headerColor = LocusColors::Transparent;
		if (g_SelectedEntity == entity)
			headerColor = LocusColors::Orange;

		ImGui::PushStyleColor(ImGuiCol_Header, headerColor);
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, headerColor);
		ImGui::PushStyleColor(ImGuiCol_HeaderActive, headerColor);
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { ImGui::GetStyle().ItemSpacing.x, 0.0f });
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());
		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor(3);

		if (ImGui::IsItemClicked())
			g_SelectedEntity = entity;

		bool entityDeleted = false;
		bool openOnCreate = false;
		if (g_SelectedEntity && ImGui::BeginPopupContextItem()) // TODO: Create child when hovering too
		{
			if (ImGui::MenuItem("Create Empty Entity"))
			{
				CommandHistory::AddCommand(new CreateChildEntityCommand(m_ActiveScene, "Empty Entity", g_SelectedEntity));
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
				for (auto& childEntity : cc.ChildEntities)
				{
					DrawEntityNode(m_ActiveScene->GetEntityByUUID(childEntity));
				}
			}
			ImGui::TreePop();
		}

		if (entityDeleted)
		{
			CommandHistory::AddCommand(new DestroyEntityCommand(m_ActiveScene, entity));
			if (g_SelectedEntity == entity)
				g_SelectedEntity = {};
		}

	}
}
