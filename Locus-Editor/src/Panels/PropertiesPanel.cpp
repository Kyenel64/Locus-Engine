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
		m_FolderIcon = Texture2D::Create("resources/icons/FolderIcon.png");
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
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 5.0f, 0.0f });
				DrawVec3Control("Position", component.Translation);

				DrawVec3Control("Rotation", component.GetRotationEuler());
				component.SetRotationEuler(component.GetRotationEuler());
				ImGui::PopStyleVar();

				// Bottom spacing is removed if pushing item spacing to all three controls. 
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 5.0f, 5.0f });
				DrawVec3Control("Scale", component.Scale, 1.0f);
				ImGui::PopStyleVar();

			});

		// --- Camera Component -----------------------------------------------
		DrawComponentUI<CameraComponent>("Camera", entity, [this](auto& component)
			{
				SceneCamera& camera = component.Camera;

				// Primary camera check
				DrawControlLabel("Primary");
				ImGui::SameLine();
				bool primaryCheck = component.Primary;
				if (ImGui::Checkbox("##Primary", &primaryCheck))
					CommandHistory::AddCommand(new ChangeValueCommand(primaryCheck, component.Primary));

				// Background color
				glm::vec4 backgroundColor = camera.GetBackgroundColor();
				DrawColorControl("Background Color", camera.GetBackgroundColor());

				// Projection mode
				DrawControlLabel("Projection");
				ImGui::SameLine();
				const char* projectionTypeString[] = { "Orthographic", "Perspective" };
				const char* currentProjectionTypeString = projectionTypeString[(int)camera.GetProjectionType()];
				ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
				if (ImGui::BeginCombo("##Projection", currentProjectionTypeString))
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
				ImGui::PopItemWidth();

				// Orthographic settings
				if (camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic)
				{
					// Size
					DrawFloatControl("Size", camera.GetOrthographicSize());
					// Near
					DrawFloatControl("Near Clip", camera.GetOrthographicNearClip());
					// Far
					DrawFloatControl("Far Clip", camera.GetOrthographicFarClip());

					// Fixed Aspect Ratio
					DrawControlLabel("Fixed Aspect Ratio");
					ImGui::SameLine();
					bool isFixed = component.FixedAspectRatio;
					if (ImGui::Checkbox("##FixedAspectRatio", &isFixed))
						CommandHistory::AddCommand(new ChangeValueCommand(isFixed, component.FixedAspectRatio));
				}

				// Perspective settings
				if (camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective)
				{
					// FOV
					DrawFloatControl("FOV", camera.GetPerspectiveFOV());
					// Near
					DrawFloatControl("Near Clip", camera.GetPerspectiveNearClip());
					// Far
					DrawFloatControl("Far Clip", camera.GetPerspectiveFarClip());
				}
			});

		// --- SpriteRenderer Component ---------------------------------------
		DrawComponentUI<SpriteRendererComponent>("Sprite Renderer", entity, [this](auto& component)
			{
				// Sprite
				DrawControlLabel("Sprite", { 110.0f, 50.0f });
				ImGui::SameLine();

				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, LocusColors::Grey);
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, LocusColors::Grey);
				if (component.Texture == nullptr)
					ImGui::Button("##EmptyTexture", { 50.0f, 50.0f });
				else
					ImGui::ImageButton((ImTextureID)(uint64_t)component.Texture->GetRendererID(), { 50.0f, 50.0f }, { 1, 1 }, { 0, 0 });
				ImGui::PopStyleColor(2);
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

				ImGui::SameLine();

				ImGui::SetCursorPosY(ImGui::GetCursorPos().y + 25.0f - ImGui::GetFontSize() * 0.5f);
				if (component.Texture == nullptr)
					ImGui::Button("##EmptyTexturePath", { -1.0f, 0.0f });
				else
					ImGui::Button(component.Texture->GetTextureName().c_str(), { -1.0f, 0.0f });
				ImGui::SameLine(ImGui::GetContentRegionAvail().x - 5.0f);
				ImGui::SetCursorPosY(ImGui::GetCursorPos().y + 25.0f - ImGui::GetFontSize() * 0.5f);
				ImGui::PushStyleColor(ImGuiCol_Button, LocusColors::Transparent);
				ImGui::ImageButton((ImTextureID)(uint64_t)m_FolderIcon->GetRendererID(), { 16.0f, 16.0f }, { 1, 1 }, { 0, 0 });
				ImGui::PopStyleColor();

				// Color
				DrawColorControl("Color", component.Color);
				// Tiling Factor
				DrawFloatControl("Tiling Factor", component.TilingFactor);
			});

		// --- Rigidbody2D Component ------------------------------------------
		DrawComponentUI<Rigidbody2DComponent>("Rigidbody 2D", entity, [this](auto& component)
			{
				// Body type
				DrawControlLabel("Body Type");
				ImGui::SameLine();
				const char* RigidbodyTypeString[] = { "Static", "Dynamic", "Kinematic" };
				const char* currentRigidbodyTypeString = RigidbodyTypeString[(int)component.BodyType];
				ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
				if (ImGui::BeginCombo("##Body Type", currentRigidbodyTypeString))
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
				ImGui::PopItemWidth();

				// Mass
				DrawFloatControl("Mass", component.Mass);
				// Gravity Scale
				DrawFloatControl("Gravity Scale", component.GravityScale);
				// Linear Drag
				DrawFloatControl("Linear Drag", component.LinearDrag);
				// Angular Drag
				DrawFloatControl("Angular Drag", component.AngularDrag);

				// Fixed Rotation
				DrawControlLabel("Fixed Rotation");
				ImGui::SameLine();
				bool fixedRotation = component.FixedRotation;
				if (ImGui::Checkbox("##Fixed Rotation", &fixedRotation))
					CommandHistory::AddCommand(new ChangeValueCommand(fixedRotation, component.FixedRotation));

				// Friction
				DrawFloatControl("Friction", component.Friction);
				// Restitution
				DrawFloatControl("Restitution", component.Restitution);
				// Restitution Threshold
				DrawFloatControl("Restitution Threshold", component.RestitutionThreshold);
			});

		DrawComponentUI<BoxCollider2DComponent>("Box Collider 2D", entity, [this](auto& component)
			{
				// Collision Layer
				DrawUInt16Control("Collision Layer", component.CollisionLayer);
				// Size
				DrawVec2Control("Size", component.Size, 1.0f);
				// Offset
				DrawVec2Control("Offset", component.Offset);
			});
	}

	void PropertiesPanel::DrawControlLabel(const std::string& name, const glm::vec2& size)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, LocusColors::Transparent);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, LocusColors::Transparent);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, LocusColors::Transparent);
		ImGui::Button(name.c_str(), { size.x, size.y });
		ImGui::PopStyleColor(3);
	}

	void PropertiesPanel::DrawFloatControl(const std::string& name, float& changeValue, float resetValue)
	{
		DrawControlLabel(name);
		ImGui::SameLine();

		if (ImGui::IsItemHovered())
		{
			ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
			if (ImGui::IsMouseDoubleClicked(0))
				CommandHistory::AddCommand(new ChangeValueCommand(resetValue, changeValue));
		}

		float dragVal = changeValue;
		std::string label = "##" + name;
		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
		if (ImGui::DragFloat(label.c_str(), &dragVal))
			CommandHistory::AddCommand(new ChangeValueCommand(dragVal, changeValue));
		ImGui::PopItemWidth();
		if (ImGui::IsItemHovered())
			ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
	}

	void PropertiesPanel::DrawIntControl(const std::string& name, int& changeValue, int resetValue)
	{
		DrawControlLabel(name);
		ImGui::SameLine();

		if (ImGui::IsItemHovered())
		{
			ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
			if (ImGui::IsMouseDoubleClicked(0))
				CommandHistory::AddCommand(new ChangeValueCommand(resetValue, changeValue));
		}

		int dragVal = changeValue;
		std::string label = "##" + name;
		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
		if (ImGui::DragInt(label.c_str(), &dragVal, 1.0f, 0))
			CommandHistory::AddCommand(new ChangeValueCommand(dragVal, changeValue));
		ImGui::PopItemWidth();
		if (ImGui::IsItemHovered())
			ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
	}

	void PropertiesPanel::DrawUInt16Control(const std::string& name, uint16_t& changeValue, uint16_t resetValue)
	{
		DrawControlLabel(name);
		ImGui::SameLine();

		if (ImGui::IsItemHovered())
		{
			ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
			if (ImGui::IsMouseDoubleClicked(0))
				CommandHistory::AddCommand(new ChangeValueCommand(resetValue, changeValue));
		}

		int dragVal = static_cast<int>(changeValue);
		std::string label = "##" + name;
		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
		if (ImGui::DragInt(label.c_str(), &dragVal, 1.0f, 0))
			CommandHistory::AddCommand(new ChangeValueCommand(static_cast<uint16_t>(dragVal), changeValue));
		ImGui::PopItemWidth();
		if (ImGui::IsItemHovered())
			ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
	}

	void PropertiesPanel::DrawColorControl(const std::string& name, glm::vec4& colorValue)
	{
		DrawControlLabel("Color");
		ImGui::SameLine();

		glm::vec4 color = colorValue;
		static glm::vec4 backupColor;
		ImGui::PushStyleColor(ImGuiCol_Button, ToImVec4(color));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ToImVec4(color));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ToImVec4(color));
		if (ImGui::Button("##Color", { ImGui::GetContentRegionAvail().x, 0.0f }))
		{
			ImGui::OpenPopup("mypicker");
			backupColor = color;
		}
		ImGui::PopStyleColor(3);
		if (ImGui::IsItemHovered())
		{
			ImGui::ColorTooltip("Color", glm::value_ptr(color), ImGuiColorEditFlags_None);
		}
		if (ImGui::BeginPopup("mypicker"))
		{
			ImGui::Text("Color");
			ImGui::Separator();
			if (ImGui::ColorPicker4("##picker", glm::value_ptr(color), ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview))
				CommandHistory::AddCommand(new ChangeValueCommand(color, colorValue));
			ImGui::SameLine();

			ImGui::BeginGroup(); // Lock X position
			ImGui::Text("Current");
			ImGui::ColorButton("##current", ToImVec4(color), ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_AlphaPreviewHalf, ImVec2(60, 40));
			ImGui::Text("Previous");
			if (ImGui::ColorButton("##previous", ToImVec4(backupColor), ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_AlphaPreviewHalf, ImVec2(60, 40)))
				color = backupColor;
			ImGui::EndGroup();
			ImGui::EndPopup();
		}
	}
	
	void PropertiesPanel::DrawVec2Control(const std::string& name, glm::vec2& values, float resetValue)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, LocusColors::Transparent);
		ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, { 0.0f, 1.0f });

		glm::vec2 dragValues = values;
		if (name == "Rotation")
			dragValues = glm::degrees(dragValues);

		DrawControlLabel(name.c_str());

		ImGui::SameLine();

		ImGui::BeginTable("Vec3Control", 2);

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0.0f, 0.0f });
		// X
		ImGui::TableNextColumn();
		ImGui::Button("X", { 0.0f, 0.0f });
		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
			CommandHistory::AddCommand(new ChangeValueCommand(resetValue, values.x));
		ImGui::SameLine();
		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
		if (ImGui::DragFloat("##X", &dragValues.x, 0.1f, 0.0f, 0.0f, "%.2f"))
		{
			if (name == "Rotation")
				CommandHistory::AddCommand(new ChangeValueCommand(glm::radians(dragValues.x), values.x));
			else
				CommandHistory::AddCommand(new ChangeValueCommand(dragValues.x, values.x));
		}
		if (ImGui::IsItemHovered())
			ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
		ImGui::PopItemWidth();

		// Y
		ImGui::TableNextColumn();
		ImGui::Button("Y", { 0.0f, 0.0f });
		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
			CommandHistory::AddCommand(new ChangeValueCommand(resetValue, values.y));
		ImGui::SameLine();
		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
		if (ImGui::DragFloat("##Y", &dragValues.y, 0.1f, 0.0f, 0.0f, "%.2f"))
		{
			if (name == "Rotation")
				CommandHistory::AddCommand(new ChangeValueCommand(glm::radians(dragValues.y), values.y));
			else
				CommandHistory::AddCommand(new ChangeValueCommand(dragValues.y, values.y));
		}
		if (ImGui::IsItemHovered())
			ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::EndTable();
		ImGui::PopStyleColor();
		ImGui::PopStyleVar();
	}

	void PropertiesPanel::DrawVec3Control(const std::string& name, glm::vec3& values, float resetValue)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, LocusColors::Transparent);
		ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, { 0.0f, 1.0f });

		glm::vec3 dragValues = values;
		if (name == "Rotation")
			dragValues = glm::degrees(dragValues);

		DrawControlLabel(name.c_str());

		ImGui::SameLine();

		ImGui::BeginTable("Vec3Control", 3);

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0.0f, 0.0f });
		// X
		ImGui::TableNextColumn();
		ImGui::Button("X", { 0.0f, 0.0f });
		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
			CommandHistory::AddCommand(new ChangeValueCommand(resetValue, values.x));
		ImGui::SameLine();
		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
		if(ImGui::DragFloat("##X", &dragValues.x, 0.1f, 0.0f, 0.0f, "%.2f"))
		{
			if (name == "Rotation")
				CommandHistory::AddCommand(new ChangeValueCommand(glm::radians(dragValues.x), values.x));
			else
				CommandHistory::AddCommand(new ChangeValueCommand(dragValues.x, values.x));
		}
		if (ImGui::IsItemHovered())
			ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
		ImGui::PopItemWidth();

		// Y
		ImGui::TableNextColumn();
		ImGui::Button("Y", { 0.0f, 0.0f });
		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
			CommandHistory::AddCommand(new ChangeValueCommand(resetValue, values.y));
		ImGui::SameLine();
		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
		if (ImGui::DragFloat("##Y", &dragValues.y, 0.1f, 0.0f, 0.0f, "%.2f"))
		{
			if (name == "Rotation")
				CommandHistory::AddCommand(new ChangeValueCommand(glm::radians(dragValues.y), values.y));
			else
				CommandHistory::AddCommand(new ChangeValueCommand(dragValues.y, values.y));
		}
		if (ImGui::IsItemHovered())
			ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
		ImGui::PopItemWidth();

		// Z
		ImGui::TableNextColumn();
		ImGui::Button("Z", { 0.0f, 0.0f });
		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
			CommandHistory::AddCommand(new ChangeValueCommand(resetValue, values.z));
		ImGui::SameLine();
		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
		if (ImGui::DragFloat("##Z", &dragValues.z, 0.1f, 0.0f, 0.0f, "%.2f"))
		{
			if (name == "Rotation")
				CommandHistory::AddCommand(new ChangeValueCommand(glm::radians(dragValues.z), values.z));
			else
				CommandHistory::AddCommand(new ChangeValueCommand(dragValues.z, values.z));
		}
		if (ImGui::IsItemHovered())
			ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::EndTable();
		ImGui::PopStyleColor();
		ImGui::PopStyleVar();
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
}