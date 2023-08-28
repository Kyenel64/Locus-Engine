#include "PropertiesPanel.h"

#include <filesystem>

#include <ImGui/imgui.h>
#include <ImGui/imgui_internal.h>
#include <glm/gtc/type_ptr.hpp>

#include "Command/Command.h"

namespace Locus
{
	extern const std::filesystem::path g_ProjectPath;

	PropertiesPanel::PropertiesPanel(const Ref<Scene>& context)
	{
		SetContext(context);
	}

	void PropertiesPanel::SetContext(const Ref<Scene>& context)
	{
		m_ActiveScene = context;
		m_SelectedEntity = {};
		m_ShowMoreButton = Texture2D::Create("resources/icons/ShowMoreButton.png");
	}

	void PropertiesPanel::OnImGuiRender()
	{
		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoScrollbar;
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 10.0f, 10.0f });
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 4.0f, 2.0f });
		ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, 10.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, { 0.0f, 0.5f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, LocusColors::Orange);
		ImGui::Begin(" Properties ", false, windowFlags);

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
						LOCUS_CORE_WARN("This entity already has a Camera Component");
					ImGui::CloseCurrentPopup();
				}

				if (ImGui::MenuItem("Sprite Renderer"))
				{
					if (!m_SelectedEntity.HasComponent<SpriteRendererComponent>())
						CommandHistory::AddCommand(new AddComponentCommand<SpriteRendererComponent>(m_SelectedEntity));
					else
						LOCUS_CORE_WARN("This entity already has a Sprite Renderer Component");
					ImGui::CloseCurrentPopup();
				}

				if (ImGui::MenuItem("Rigidbody 2D"))
				{
					if (!m_SelectedEntity.HasComponent<Rigidbody2DComponent>())
						CommandHistory::AddCommand(new AddComponentCommand<Rigidbody2DComponent>(m_SelectedEntity));
					else
						LOCUS_CORE_WARN("This entity already has a Rigidbody2D Component");
					ImGui::CloseCurrentPopup();
				}

				if (ImGui::MenuItem("Box Collider 2D"))
				{
					if (!m_SelectedEntity.HasComponent<BoxCollider2DComponent>())
						CommandHistory::AddCommand(new AddComponentCommand<BoxCollider2DComponent>(m_SelectedEntity));
					else
						LOCUS_CORE_WARN("This entity already has a BoxCollider2D Component");
					ImGui::CloseCurrentPopup();
				}

				ImGui::EndPopup();
			}
		}
		else
		{
			Application::Get().GetImGuiLayer()->BlockEvents(false);
		}

		if (ImGui::BeginPopupContextWindow(0, 1 | ImGuiPopupFlags_NoOpenOverItems))
		{
			if (m_ClipboardComponentType != ComponentType::None && m_SelectedEntity)
				if (ImGui::MenuItem("Paste Component"))
					PasteComponent(m_SelectedEntity);
			ImGui::EndPopup();
		}

		ImGui::End();
		ImGui::PopStyleVar(4);
		ImGui::PopStyleColor();
	}

	void PropertiesPanel::DrawVec3Control(const std::string& name, glm::vec3& values, float resetValue, float columnWidth)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, LocusColors::Transparent);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, LocusColors::Transparent);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, LocusColors::Transparent);
		ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, { 0.0f, 1.0f });

		glm::vec3 dragValues = values;
		if (name == "Rotation")
			dragValues = glm::degrees(dragValues);

		DrawControlLabel(name.c_str());

		ImGui::SameLine();

		ImGui::BeginTable("Vec3Control", 3);

		// X
		ImGui::TableNextColumn();
		if (ImGui::Button("X", { 0.0f, 0.0f }))
		{

		}
		ImGui::SameLine();
		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
		if(ImGui::DragFloat("##X", &dragValues.x, 0.1f, 0.0f, 0.0f, "%.2f"))
		{

			if (name == "Rotation")
				CommandHistory::AddCommand(new ChangeValueCommand(glm::radians(dragValues.x), values.x));
			else
				CommandHistory::AddCommand(new ChangeValueCommand(dragValues.x, values.x));
		}
		ImGui::PopItemWidth();

		// Y
		ImGui::TableNextColumn();
		if (ImGui::Button("Y", { 0.0f, 0.0f }))
		{

		}
		ImGui::SameLine();
		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
		if (ImGui::DragFloat("##Y", &dragValues.y, 0.1f, 0.0f, 0.0f, "%.2f"))
		{
			if (name == "Rotation")
				CommandHistory::AddCommand(new ChangeValueCommand(glm::radians(dragValues.x), values.x));
			else
				CommandHistory::AddCommand(new ChangeValueCommand(dragValues.x, values.x));
		}
		ImGui::PopItemWidth();

		// Z
		ImGui::TableNextColumn();
		if (ImGui::Button("Z", { 0.0f, 0.0f }))
		{

		}
		ImGui::SameLine();
		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
		if (ImGui::DragFloat("##Z", &dragValues.z, 0.1f, 0.0f, 0.0f, "%.2f"))
		{
			if (name == "Rotation")
				CommandHistory::AddCommand(new ChangeValueCommand(glm::radians(dragValues.x), values.x));
			else
				CommandHistory::AddCommand(new ChangeValueCommand(dragValues.x, values.x));
		}
		ImGui::PopItemWidth();

		ImGui::EndTable();
		ImGui::PopStyleColor(3);
		ImGui::PopStyleVar();
		
		//if (ImGui::Button("X", buttonSize)) // TODO: double click
		//	CommandHistory::AddCommand(new ChangeValueCommand(resetValue, values.x));

		//if (ImGui::DragFloat("##X", &dragValues.x, 0.1f, 0.0f, 0.0f, "%.2f"))
		//{
		//	if (name == "Rotation")
		//		CommandHistory::AddCommand(new ChangeValueCommand(glm::radians(dragValues.x), values.x));
		//	else
		//		CommandHistory::AddCommand(new ChangeValueCommand(dragValues.x, values.x));
		//}
	}

	template<typename T, typename UIFunction>
	void PropertiesPanel::DrawComponentUI(const std::string& name, Entity entity, UIFunction uiFunction)
	{
		const ImGuiTreeNodeFlags treeNodeFlags =
			ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed |
			ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap |
			ImGuiTreeNodeFlags_FramePadding;

		if (entity.HasComponent<T>())
		{

			auto& component = entity.GetComponent<T>();
			ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

			ImGui::PushStyleColor(ImGuiCol_Header, LocusColors::Tan);
			float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, name.c_str());
			ImGui::PopStyleColor();

			ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);
			ImGui::PushStyleColor(ImGuiCol_Button, LocusColors::Transparent);
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, LocusColors::Transparent);
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, LocusColors::LightGrey);
			if (ImGui::ImageButton((ImTextureID)(uint64_t)m_ShowMoreButton->GetRendererID(), ImVec2(lineHeight, lineHeight), ImVec2(0, 0), ImVec2(1, 1), 0))
				ImGui::OpenPopup("Component Settings");
			ImGui::PopStyleColor(3);

			bool removeComponent = false;
			if (ImGui::BeginPopup("Component Settings"))
			{
				if (name != "Transform")
				{
					if (ImGui::MenuItem("Remove Component"))
					{
						removeComponent = true;
					}
				}
				if (ImGui::MenuItem("Copy Component"))
					CopyComponentToClipboard<T>(m_SelectedEntity);
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

	void PropertiesPanel::DrawComponents(Entity entity)
	{
		// --- Tag Component --------------------------------------------------
		if (entity.HasComponent<TagComponent>())
		{
			auto& tag = entity.GetComponent<TagComponent>().Tag;
			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			strcpy_s(buffer, sizeof(buffer), tag.c_str());
			ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue;

			ImGui::PushItemWidth(ImGui::GetWindowSize().x - 50.0f);
			if (ImGui::InputText("##Tag", buffer, sizeof(buffer), flags))
				CommandHistory::AddCommand(new ChangeValueCommand(std::string(buffer), tag));
			ImGui::PopItemWidth();

			ImGui::SameLine();

			bool enabled = entity.GetComponent<TagComponent>().Enabled;
			if (ImGui::Checkbox("##Enabled", &enabled))
				entity.GetComponent<TagComponent>().Enabled = enabled;

			if (!ImGui::TempInputIsActive(ImGui::GetActiveID()))
				Application::Get().GetImGuiLayer()->BlockEvents(true);
			else
				Application::Get().GetImGuiLayer()->BlockEvents(false);

			DrawControlLabel("Group", { 0.0f, 0.0f });

			ImGui::SameLine();

			ImGui::Button("Enemy (temp)", { 150.0f, 0.0f });
		}

		// --- Transform Component --------------------------------------------
		DrawComponentUI<TransformComponent>("Transform", entity, [this](auto& component)
			{
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0.0f, 0.0f });
				DrawVec3Control("Position", component.Translation);

				DrawVec3Control("Rotation", component.GetRotationEuler());
				component.SetRotationEuler(component.GetRotationEuler());
				ImGui::PopStyleVar();

				// Bottom spacing is removed if pushing item spacing to all three controls. 
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0.0f, 5.0f });
				DrawVec3Control("Scale", component.Scale, 1.0f);
				ImGui::PopStyleVar();

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
					CommandHistory::AddCommand(new ChangeFunctionValueCommand(LOCUS_BIND_FN(camera.SetBackgroundColor, const glm::vec4&), backgroundColor, camera.GetBackgroundColor()));

				// Projection mode
				const char* projectionTypeString[] = { "Orthographic", "Perspective" };
				const char* currentProjectionTypeString = projectionTypeString[(int)camera.GetProjectionType()];
				if (ImGui::BeginCombo("Projection", currentProjectionTypeString))
				{
					for (int i = 0; i < 2; i++)
					{
						bool isSelected = currentProjectionTypeString == projectionTypeString[i];
						if (ImGui::Selectable(projectionTypeString[i], isSelected))
							CommandHistory::AddCommand(new ChangeFunctionValueCommand(LOCUS_BIND_FN(camera.SetProjectionType, SceneCamera::ProjectionType), (SceneCamera::ProjectionType)i, camera.GetProjectionType()));

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
						CommandHistory::AddCommand(new ChangeFunctionValueCommand(LOCUS_BIND_FN(camera.SetOrthographicSize, float), orthoSize, camera.GetOrthographicSize()));

					// Near
					float nearClip = camera.GetOrthographicNearClip();
					if (ImGui::DragFloat("Near Clip", &nearClip))
						CommandHistory::AddCommand(new ChangeFunctionValueCommand(LOCUS_BIND_FN(camera.SetOrthographicNearClip, float), nearClip, camera.GetOrthographicNearClip()));

					// Far
					float farClip = camera.GetOrthographicFarClip();
					if (ImGui::DragFloat("Far Clip", &farClip))
						CommandHistory::AddCommand(new ChangeFunctionValueCommand(LOCUS_BIND_FN(camera.SetOrthographicFarClip, float), farClip, camera.GetOrthographicFarClip()));

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
						CommandHistory::AddCommand(new ChangeFunctionValueCommand(LOCUS_BIND_FN(camera.SetPerspectiveFOV, float), glm::radians(fov), camera.GetPerspectiveFOV()));

					// Near
					float nearClip = camera.GetPerspectiveNearClip();
					if (ImGui::DragFloat("Near Clip", &nearClip))
						CommandHistory::AddCommand(new ChangeFunctionValueCommand(LOCUS_BIND_FN(camera.SetPerspectiveNearClip, float), nearClip, camera.GetPerspectiveNearClip()));

					// Far
					float farClip = camera.GetPerspectiveFarClip();
					if (ImGui::DragFloat("Far Clip", &farClip))
						CommandHistory::AddCommand(new ChangeFunctionValueCommand(LOCUS_BIND_FN(camera.SetPerspectiveFarClip, float), farClip, camera.GetPerspectiveFarClip()));
				}
			});

		// --- SpriteRenderer Component ---------------------------------------
		DrawComponentUI<SpriteRendererComponent>("Sprite Renderer", entity, [this](auto& component)
			{
				// Color
				glm::vec4 color = component.Color;
				if (ImGui::ColorEdit4("Color", glm::value_ptr(color)))
					CommandHistory::AddCommand(new ChangeValueCommand(color, component.Color));

				// Texture
				ImGui::Button("Texture", ImVec2(100.0f, 0.0f));
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("TEXTURE_ITEM_PATH"))
					{
						const wchar_t* path = (const wchar_t*)payload->Data;
						std::filesystem::path texturePath = std::filesystem::path(g_ProjectPath) / path;
						CommandHistory::AddCommand(new ChangeTextureCommand(Texture2D::Create(texturePath.string()), component.Texture, component.TexturePath));
					}
					ImGui::EndDragDropTarget();
				}

				// Texture Tiling
				float tilingFactor = component.TilingFactor;
				if (ImGui::DragFloat("Tiling", &tilingFactor))
					CommandHistory::AddCommand(new ChangeValueCommand(tilingFactor, component.TilingFactor));

			});

		// --- Rigidbody2D Component ------------------------------------------
		DrawComponentUI<Rigidbody2DComponent>("Rigidbody 2D", entity, [this](auto& component)
			{
				// Body type
				const char* RigidbodyTypeString[] = { "Static", "Dynamic", "Kinematic" };
				const char* currentRigidbodyTypeString = RigidbodyTypeString[(int)component.BodyType];
				if (ImGui::BeginCombo("Body Type", currentRigidbodyTypeString))
				{
					for (int i = 0; i < 2; i++)
					{
						bool isSelected = currentRigidbodyTypeString == RigidbodyTypeString[i];
						if (ImGui::Selectable(RigidbodyTypeString[i], isSelected))
							CommandHistory::AddCommand(new ChangeValueCommand((Rigidbody2DComponent::Rigidbody2DType)i, component.BodyType));

						if (isSelected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}

				// Mass
				float mass = component.Mass;
				if (ImGui::DragFloat("Mass", &mass))
					CommandHistory::AddCommand(new ChangeValueCommand(mass, component.Mass));

				// Gravity Scale
				float gravityScale = component.GravityScale;
				if (ImGui::DragFloat("Gravity Scale", &gravityScale))
					CommandHistory::AddCommand(new ChangeValueCommand(gravityScale, component.GravityScale));

				// Linear Drag
				float linearDrag = component.LinearDrag;
				if (ImGui::DragFloat("Linear Drag", &linearDrag))
					CommandHistory::AddCommand(new ChangeValueCommand(linearDrag, component.LinearDrag));

				// Angular Drag
				float angularDrag = component.AngularDrag;
				if (ImGui::DragFloat("Angular Drag", &angularDrag))
					CommandHistory::AddCommand(new ChangeValueCommand(angularDrag, component.AngularDrag));

				// Fixed Rotation
				bool fixedRotation = component.FixedRotation;
				if (ImGui::Checkbox("Fixed Rotation", &fixedRotation))
					CommandHistory::AddCommand(new ChangeValueCommand(fixedRotation, component.FixedRotation));

				// Friction
				float friction = component.Friction;
				if (ImGui::DragFloat("Friction", &friction))
					CommandHistory::AddCommand(new ChangeValueCommand(friction, component.Friction));

				// Restitution
				float restitution = component.Restitution;
				if (ImGui::DragFloat("Restitution", &restitution))
					CommandHistory::AddCommand(new ChangeValueCommand(restitution, component.Restitution));

				// Restitution Threshold
				float restitutionThreshold = component.RestitutionThreshold;
				if (ImGui::DragFloat("Restitution Threshold", &restitutionThreshold))
					CommandHistory::AddCommand(new ChangeValueCommand(restitutionThreshold, component.RestitutionThreshold));
			});

		DrawComponentUI<BoxCollider2DComponent>("Box Collider 2D", entity, [this](auto& component)
			{
				// Collision Layer
				int collisionLayer = component.CollisionLayer;
				if (ImGui::DragInt("Collision Layer", &collisionLayer, 1.0f, 1, 16))
					CommandHistory::AddCommand(new ChangeValueCommand((uint16_t)collisionLayer, component.CollisionLayer));

				// Size
				float size[2] = { component.Size.x, component.Size.y };
				if (ImGui::DragFloat2("Size", (float*)&size, 0.1f))
				{
					glm::vec2 sizeVec = { size[0], size[1] };
					CommandHistory::AddCommand(new ChangeValueCommand(sizeVec, component.Size));
				}

				// Offset
				float offset[2] = { component.Offset.x, component.Offset.y };
				if (ImGui::DragFloat2("Offset", (float*)&offset, 0.1f))
				{
					glm::vec2 offsetVec = { offset[0], offset[1] };
					CommandHistory::AddCommand(new ChangeValueCommand(offsetVec, component.Offset));
				}
			});
	}

	template<typename T>
	void PropertiesPanel::CopyComponentToClipboard(Entity selectedEntity)
	{
		if (typeid(T) == typeid(TransformComponent))
		{
			m_ClipboardComponent.Transform = m_SelectedEntity.GetComponent<TransformComponent>();
			m_ClipboardComponentType = ComponentType::Transform;
		}
		if (typeid(T) == typeid(SpriteRendererComponent))
		{
			m_ClipboardComponent.SpriteRenderer = m_SelectedEntity.GetComponent<SpriteRendererComponent>();
			m_ClipboardComponentType = ComponentType::SpriteRenderer;
		}
		if (typeid(T) == typeid(CameraComponent))
		{
			m_ClipboardComponent.Camera = m_SelectedEntity.GetComponent<CameraComponent>();
			m_ClipboardComponentType = ComponentType::Camera;
		}
		if (typeid(T) == typeid(Rigidbody2DComponent))
		{
			m_ClipboardComponent.Rigidbody2D = m_SelectedEntity.GetComponent<Rigidbody2DComponent>();
			m_ClipboardComponentType = ComponentType::Rigidbody2D;
		}
		if (typeid(T) == typeid(BoxCollider2DComponent))
		{
			m_ClipboardComponent.BoxCollider2D = m_SelectedEntity.GetComponent<BoxCollider2DComponent>();
			m_ClipboardComponentType = ComponentType::BoxCollider2D;
		}
		if (typeid(T) == typeid(NativeScriptComponent))
		{
			m_ClipboardComponent.NativeScript = m_SelectedEntity.GetComponent<NativeScriptComponent>();
			m_ClipboardComponentType = ComponentType::NativeScript;
		}
	}

	void PropertiesPanel::PasteComponent(Entity selectedEntity)
	{
		switch (m_ClipboardComponentType)
		{
		case Locus::ComponentType::None:
			break;
		case Locus::ComponentType::Transform: selectedEntity.AddOrReplaceComponent<TransformComponent>(m_ClipboardComponent.Transform);
			break;
		case Locus::ComponentType::SpriteRenderer: selectedEntity.AddOrReplaceComponent<SpriteRendererComponent>(m_ClipboardComponent.SpriteRenderer);
			break;
		case Locus::ComponentType::Camera: selectedEntity.AddOrReplaceComponent<CameraComponent>(m_ClipboardComponent.Camera);
			break;
		case Locus::ComponentType::Rigidbody2D: selectedEntity.AddOrReplaceComponent<Rigidbody2DComponent>(m_ClipboardComponent.Rigidbody2D);
			break;
		case Locus::ComponentType::BoxCollider2D: selectedEntity.AddOrReplaceComponent<BoxCollider2DComponent>(m_ClipboardComponent.BoxCollider2D);
			break;
		case Locus::ComponentType::NativeScript: selectedEntity.AddOrReplaceComponent<NativeScriptComponent>(m_ClipboardComponent.NativeScript);
			break;
		default:
			break;
		}
	}

	void PropertiesPanel::DrawControlLabel(const std::string& name, const glm::vec2& size)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, LocusColors::Transparent);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, LocusColors::Transparent);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, LocusColors::Transparent);
		ImGui::Button(name.c_str(), { size.x, size.y });
		ImGui::PopStyleColor(3);
	}

	void PropertiesPanel::DrawFloatControl(const std::string& name, float& value)
	{

	}
}