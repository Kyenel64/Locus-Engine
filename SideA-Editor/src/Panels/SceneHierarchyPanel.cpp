#include "SceneHierarchyPanel.h"

#include <ImGui/imgui.h>
#include <ImGui/imgui_internal.h>
#include <glm/gtc/type_ptr.hpp>

#include "SideA/Scene/Components.h"

#include "Command/Command.h"

namespace SideA
{
	SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& context)
	{
		SetContext(context);
	}

	void SceneHierarchyPanel::SetContext(const Ref<Scene>& context)
	{
		m_ActiveScene = context;
		m_SelectedEntity = {};
	}

	void SceneHierarchyPanel::OnImGuiRender()
	{
		// --- Scene Hierarchy Panel ------------------------------------------
		ImGui::Begin("Scene Hierarchy");

		// Display each entity
		m_ActiveScene->m_Registry.each([&](auto entityID)
		{
			Entity entity(entityID, m_ActiveScene.get());
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

		ImGui::End();

		// --- Properties Panel -----------------------------------------------
		ImGui::Begin("Properties");
		if (m_SelectedEntity)
		{
			DrawComponents(m_SelectedEntity);

			if (ImGui::Button("Add Component"))
				ImGui::OpenPopup("AddComponent");

			if (ImGui::BeginPopup("AddComponent"))
			{
				if (ImGui::MenuItem("Camera"))
				{
					if (!m_SelectedEntity.HasComponent<CameraComponent>())
						CommandHistory::AddCommand(new AddComponentCommand<CameraComponent>(m_SelectedEntity));
					else
						SIDEA_CORE_WARN("This entity already has a Camera Component");
					ImGui::CloseCurrentPopup();
				}
					
				if (ImGui::MenuItem("Sprite Renderer"))
				{
					if (!m_SelectedEntity.HasComponent<SpriteRendererComponent>())
						CommandHistory::AddCommand(new AddComponentCommand<SpriteRendererComponent>(m_SelectedEntity));
					else
						SIDEA_CORE_WARN("This entity already has a Sprite Renderer Component");
					ImGui::CloseCurrentPopup();
				}

				ImGui::EndPopup();
			}
		}
		ImGui::End();
	}

	void SceneHierarchyPanel::DrawEntityNode(Entity entity)
	{
		auto& tag = entity.GetComponent<TagComponent>().Tag;

		ImGuiTreeNodeFlags flags = ((m_SelectedEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0)
			| ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());

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

	void SceneHierarchyPanel::DrawVec3Control(const std::string& name, glm::vec3& values, float resetValue, float columnWidth)
	{
		glm::vec3 dragValues = values;
		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(name.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(name.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		// X
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.2f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.2f, 0.2f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("X", buttonSize)) // TODO: double click
			CommandHistory::AddCommand(new ChangeValueCommand(resetValue, values.x));
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		if (ImGui::DragFloat("##X", &dragValues.x, 0.1f, 0.0f, 0.0f, "%.2f"))
			CommandHistory::AddCommand(new ChangeValueCommand(dragValues.x, values.x));
		ImGui::PopItemWidth();
		ImGui::SameLine();

		// Y
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.8f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.9f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.8f, 0.2f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Y", buttonSize))
			CommandHistory::AddCommand(new ChangeValueCommand(resetValue, values.y));
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		if (ImGui::DragFloat("##Y", &dragValues.y, 0.1f, 0.0f, 0.0f, "%.2f"))
			CommandHistory::AddCommand(new ChangeValueCommand(dragValues.y, values.y));
		ImGui::PopItemWidth();
		ImGui::SameLine();

		// Z
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.2f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.2f, 0.9f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.2f, 0.8f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Z", buttonSize))
			CommandHistory::AddCommand(new ChangeValueCommand(resetValue, values.z));
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		if (ImGui::DragFloat("##Z", &dragValues.z, 0.1f, 0.0f, 0.0f, "%.2f"))
			CommandHistory::AddCommand(new ChangeValueCommand(dragValues.z, values.z));
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();
		ImGui::Columns(1);

		ImGui::PopID();
	}

	template<typename T, typename UIFunction>
	void SceneHierarchyPanel::DrawComponentUI(const std::string& name, Entity entity, UIFunction uiFunction)
	{
		const ImGuiTreeNodeFlags treeNodeFlags =
			ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed |
			ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap |
			ImGuiTreeNodeFlags_FramePadding;

		if (entity.HasComponent<T>())
		{

			auto& component = entity.GetComponent<T>();
			ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
			ImGui::PushStyleColor(ImGuiCol_Header, { 0.30f, 0.30f, 0.30f, 1.0f });
			float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			ImGui::Separator();
			bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, name.c_str());
			ImGui::PopStyleVar(1);
			ImGui::PopStyleColor();

			ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);
			if (ImGui::Button("+", ImVec2(lineHeight, lineHeight)))
				ImGui::OpenPopup("Component Settings");

			bool removeComponent = false;
			if (ImGui::BeginPopup("Component Settings"))
			{
				if (name != "Transform")
				{
					if (ImGui::MenuItem("Remove component"))
					{
						removeComponent = true;
					}
				}
				ImGui::EndPopup();
			}

			if (open)
			{
				uiFunction(component);
				ImGui::TreePop();
			}

			if (removeComponent)
				CommandHistory::AddCommand(new RemoveComponentCommand<T>(entity));
		}
	}

	void SceneHierarchyPanel::DrawComponents(Entity entity)
	{
		// --- Tag Component --------------------------------------------------
		if (entity.HasComponent<TagComponent>())
		{
			auto& tag = entity.GetComponent<TagComponent>().Tag;
			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			strcpy_s(buffer, sizeof(buffer), tag.c_str());
			ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue;

			if (ImGui::InputText("Tag", buffer, sizeof(buffer), flags))
				CommandHistory::AddCommand(new ChangeValueCommand(std::string(buffer), tag));
		}

		// --- Transform Component --------------------------------------------
		DrawComponentUI<TransformComponent>("Transform", entity, [this](auto& component)
			{
				DrawVec3Control("Translation", component.Translation);

				glm::vec3 rotation = glm::degrees(component.GetRotationEuler());
				DrawVec3Control("Rotation", rotation);
				component.SetRotationEuler(glm::radians(rotation));

				DrawVec3Control("Scale", component.Scale, 1.0f);
			});

		// --- Camera Component -----------------------------------------------
		DrawComponentUI<CameraComponent>("Camera", entity, [this](auto& component)
			{
				SceneCamera& camera = component.Camera;

				// Primary camera check
				bool primaryCheck = component.Primary;
				if (ImGui::Checkbox("Primary", &primaryCheck))
					CommandHistory::AddCommand(new ChangeValueCommand(primaryCheck, component.Primary));

				// Background color
				glm::vec4 backgroundColor = camera.GetBackgroundColor();
				if (ImGui::ColorEdit4("Background Color", glm::value_ptr(backgroundColor)))
					CommandHistory::AddCommand(new ChangeValueCommand(backgroundColor, camera.GetBackgroundColor()));

				// Projection mode
				const char* projectionTypeString[] = { "Orthographic", "Perspective" };
				const char* currentProjectionTypeString = projectionTypeString[(int)camera.GetProjectionType()];
				if (ImGui::BeginCombo("Projection", currentProjectionTypeString))
				{
					for (int i = 0; i < 2; i++)
					{
						bool isSelected = currentProjectionTypeString == projectionTypeString[i];
						if (ImGui::Selectable(projectionTypeString[i], isSelected))
							CommandHistory::AddCommand(new ChangeValueCommand((SceneCamera::ProjectionType)i, camera.GetProjectionType()));

						if (isSelected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}

				// Orthographic settings
				if (camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic)
				{
					// Size
					float orthoSize = camera.GetOrthographicSize();
					if (ImGui::DragFloat("Size", &orthoSize))
						CommandHistory::AddCommand(new ChangeValueCommand(orthoSize, camera.GetOrthographicSize()));

					// Near
					float nearClip = camera.GetOrthographicNearClip();
					if (ImGui::DragFloat("Near Clip", &nearClip))
						CommandHistory::AddCommand(new ChangeValueCommand(nearClip, camera.GetOrthographicNearClip()));

					// Far
					float farClip = camera.GetOrthographicFarClip();
					if (ImGui::DragFloat("Far Clip", &farClip))
						CommandHistory::AddCommand(new ChangeValueCommand(farClip, camera.GetOrthographicFarClip()));

					// Fixed Aspect Ratio
					bool fixedAspectRatio = component.FixedAspectRatio;
					if (ImGui::Checkbox("Fixed Aspect Ratio", &fixedAspectRatio))
						CommandHistory::AddCommand(new ChangeValueCommand(fixedAspectRatio, component.FixedAspectRatio));
				}

				// Perspective settings
				if (camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective)
				{
					// FOV
					float fov = glm::degrees(camera.GetPerspectiveFOV());
					if (ImGui::DragFloat("FOV", &fov))
						CommandHistory::AddCommand(new ChangeValueCommand(glm::radians(fov), camera.GetPerspectiveFOV()));

					// Near
					float nearClip = camera.GetPerspectiveNearClip();
					if (ImGui::DragFloat("Near Clip", &nearClip))
						CommandHistory::AddCommand(new ChangeValueCommand(nearClip, camera.GetPerspectiveNearClip()));

					// Far
					float farClip = camera.GetPerspectiveFarClip();
					if (ImGui::DragFloat("Far Clip", &farClip))
						CommandHistory::AddCommand(new ChangeValueCommand(farClip, camera.GetPerspectiveFarClip()));
				}
		});

		// --- SpriteRenderer Component ---------------------------------------
		DrawComponentUI<SpriteRendererComponent>("Sprite Renderer", entity, [this](auto& component)
		{
			glm::vec4 color = component.Color;
			if (ImGui::ColorEdit4("Color", glm::value_ptr(color)))
				CommandHistory::AddCommand(new ChangeValueCommand(color, component.Color));
		});
	}	
}