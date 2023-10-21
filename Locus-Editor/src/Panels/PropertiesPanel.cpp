#include "PropertiesPanel.h"

#include <filesystem>

#include <ImGui/imgui.h>
#include <ImGui/imgui_internal.h>
#include <glm/gtc/type_ptr.hpp>

#include "Command/Command.h"
#include "Command/CommandHistory.h"
#include "Command/EntityCommands.h"
#include "Command/ValueCommands.h"

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

		m_ScriptClasses = ScriptEngine::GetClassNames(); // TODO: Move this to when hot reloading.
	}

	void PropertiesPanel::OnImGuiRender()
	{
		float windowWidth = ImGui::GetWindowSize().x;
		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoScrollbar;
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 10.0f, 10.0f });
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 4.0f, 2.0f });
		ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, 10.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, { 0.0f, 0.5f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, LocusColors::Orange);
		ImGui::Begin(" Properties ", false, windowFlags);

		m_LabelWidth =  60.0f * ( 1.0f + ((ImGui::GetWindowSize().x / windowWidth) * 5.0f));

		if (m_SelectedEntity.IsValid())
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

				if (ImGui::MenuItem("Circle Renderer"))
				{
					if (!m_SelectedEntity.HasComponent<CircleRendererComponent>())
						CommandHistory::AddCommand(new AddComponentCommand<CircleRendererComponent>(m_SelectedEntity));
					else
						LOCUS_CORE_WARN("This entity already has a Circle Renderer Component");
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

				if (ImGui::MenuItem("Circle Collider 2D"))
				{
					if (!m_SelectedEntity.HasComponent<CircleCollider2DComponent>())
						CommandHistory::AddCommand(new AddComponentCommand<CircleCollider2DComponent>(m_SelectedEntity));
					else
						LOCUS_CORE_WARN("This entity already has a CircleCollider2D Component");
					ImGui::CloseCurrentPopup();
				}

				if (ImGui::MenuItem("Script"))
				{
					if (!m_SelectedEntity.HasComponent<ScriptComponent>())
						CommandHistory::AddCommand(new AddComponentCommand<ScriptComponent>(m_SelectedEntity));
					else
						LOCUS_CORE_WARN("This entity already has a Script Component");
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
				// Position
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 5.0f, 0.0f });
				DrawVec3Control("Position", component.LocalPosition, 0.0f, 0.01f, 0.0f, 0.0f, "%.2f");
				
				// Rotation
				DrawVec3Control("Rotation", component.LocalRotation, 0.0f, 0.01f, 0.0f, 0.0f, "%.2f");
				component.SetLocalRotation(component.LocalRotation);
				ImGui::PopStyleVar();

				// Scale
				// Bottom spacing is removed if pushing item spacing to all three controls. 
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 5.0f, 5.0f });
				DrawVec3Control("Scale", component.LocalScale, 1.0f, 0.01f, 0.0f, 0.0f, "%.2f");
				ImGui::PopStyleVar();

			});

		// --- Camera Component -----------------------------------------------
		DrawComponentUI<CameraComponent>("Camera", entity, [this](auto& component)
			{
				SceneCamera& camera = component.Camera;

				// Primary camera check
				DrawControlLabel("Primary", { m_LabelWidth, 0.0f });
				ImGui::SameLine();
				bool primaryCheck = component.Primary;
				if (ImGui::Checkbox("##Primary", &primaryCheck))
					CommandHistory::AddCommand(new ChangeValueCommand(primaryCheck, component.Primary));

				// Background color
				glm::vec4 backgroundColor = camera.GetBackgroundColor();
				DrawColorControl("Background Color", camera.GetBackgroundColor());

				// Projection mode
				DrawControlLabel("Projection", { m_LabelWidth, 0.0f });
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
					DrawFloatControl("Size", camera.GetOrthographicSize(), 5.0f);
					// Near
					DrawFloatControl("Near Clip", camera.GetOrthographicNearClip(), -1.0f, 0.1f, 0.0f, 0.0f, "%.1f");
					// Far
					DrawFloatControl("Far Clip", camera.GetOrthographicFarClip(), 1000.0f, 0.1f, 0.0f, 0.0f, "%.1f");

					// Fixed Aspect Ratio
					DrawControlLabel("Fixed Aspect Ratio", { m_LabelWidth, 0.0f });
					ImGui::SameLine();
					bool isFixed = component.FixedAspectRatio;
					if (ImGui::Checkbox("##FixedAspectRatio", &isFixed))
						CommandHistory::AddCommand(new ChangeValueCommand(isFixed, component.FixedAspectRatio));
				}

				// Perspective settings
				if (camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective)
				{
					// FOV
					float fov = glm::degrees(camera.GetPerspectiveFOV());
					DrawFloatControl("FOV", fov, 45.0f);
					camera.SetPerspectiveFOV(glm::radians(fov));
					// Near
					DrawFloatControl("Near Clip", camera.GetPerspectiveNearClip(), -1.0f, 0.1f, 0.0f, 0.0f, "%.1f");
					// Far
					DrawFloatControl("Far Clip", camera.GetPerspectiveFarClip(), 1000.0f, 0.1f, 0.0f, 0.0f, "%.1f");
				}
			});

		// --- SpriteRenderer Component ---------------------------------------
		DrawComponentUI<SpriteRendererComponent>("Sprite Renderer", entity, [this](auto& component)
			{
				// Sprite
				DrawControlLabel("Sprite", { m_LabelWidth, 50.0f });
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

		// --- Circle Renderer Component --------------------------------------
		DrawComponentUI<CircleRendererComponent>("Circle Renderer", entity, [this](auto& component)
			{
				DrawColorControl("Color", component.Color);
				DrawFloatControl("Thickness", component.Thickness, 1.0f, 0.01f, 0.0f, 1.0f);
				DrawFloatControl("Fade", component.Fade, 0.005f, 0.01f, 0.0001f, FLT_MAX);
			});

		// --- Rigidbody2D Component ------------------------------------------
		DrawComponentUI<Rigidbody2DComponent>("Rigidbody 2D", entity, [this](auto& component)
			{
				// Body type
				DrawControlLabel("Body Type", { m_LabelWidth, 0.0f });
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
				DrawFloatControl("Linear Damping", component.LinearDamping, 0.2f);
				// Angular Drag
				DrawFloatControl("Angular Damping", component.AngularDamping, 0.2f);

				// Fixed Rotation
				DrawControlLabel("Fixed Rotation", { m_LabelWidth, 0.0f });
				ImGui::SameLine();
				bool fixedRotation = component.FixedRotation;
				if (ImGui::Checkbox("##Fixed Rotation", &fixedRotation))
					CommandHistory::AddCommand(new ChangeValueCommand(fixedRotation, component.FixedRotation));

				// Friction
				DrawFloatControl("Friction", component.Friction, 0.2f);
				// Restitution
				DrawFloatControl("Restitution", component.Restitution, 0.0f);
				// Restitution Threshold
				DrawFloatControl("Restitution Threshold", component.RestitutionThreshold, 0.0f);
			});

		// --- BoxCollider2D Component ----------------------------------------
		DrawComponentUI<BoxCollider2DComponent>("Box Collider 2D", entity, [this](auto& component)
			{
				// Collision Layer
				DrawIntControl("Collision Layer", component.CollisionLayer);
				// Size
				DrawVec2Control("Size", component.Size, 1.0f);
				// Offset
				DrawVec2Control("Offset", component.Offset);
			});

		// --- CircleCollider2D Component -------------------------------------
		DrawComponentUI<CircleCollider2DComponent>("Circle Collider 2D", entity, [this](auto& component)
			{
				// Collision Layer
				DrawIntControl("Collision Layer", component.CollisionLayer);
				// Radius
				DrawFloatControl("Radius", component.Radius, 0.5f);
				// Offset
				DrawVec2Control("Offset", component.Offset);
			});

		// --- Script ---------------------------------------------------------
		DrawComponentUI<ScriptComponent>("Script", entity, [this, entity](auto& component) mutable
			{
				// Script Class
				DrawControlLabel("Script Class", { m_LabelWidth, 0.0f });
				ImGui::SameLine();
				std::string curClass = component.ScriptClass;
				ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
				if (ImGui::BeginCombo("##Script Class", curClass.c_str()))
				{
					static ImGuiTextFilter filter;
					filter.Draw("##Search");
					// First class is <Module> which we don't use.
					for (int i = 1; i < m_ScriptClasses.size(); i++)
					{
						if (filter.PassFilter(m_ScriptClasses[i].c_str()))
						{
							bool isSelected = curClass == m_ScriptClasses[i];
							if (ImGui::Selectable(m_ScriptClasses[i].c_str(), isSelected))
							{
								CommandHistory::AddCommand(new ChangeValueCommand(m_ScriptClasses[i], component.ScriptClass));
								ScriptEngine::GetFieldInstances(entity.GetUUID()).clear(); // Could cause error if undoing
							}
							if (isSelected)
								ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}
				ImGui::PopItemWidth();

				// Public Fields
				if (Application::Get().IsRunning())
				{
					Ref<ScriptClass> scriptClass = ScriptEngine::GetScriptClass(component.ScriptClass);
					Ref<ScriptInstance> instance = ScriptEngine::GetScriptInstance(entity.GetUUID());
					if (scriptClass)
					{
						const auto& fields = scriptClass->GetPublicFields();
						for (const auto& [name, field] : fields)
						{
							switch (field.Type)
							{
								// TODO: Add the rest of data types
								case FieldType::SystemSingle:  DrawFieldControl<float>(entity, name, field, scriptClass, instance); break;
								case FieldType::SystemInt:     DrawFieldControl<int>(entity, name, field, scriptClass, instance); break;
								case FieldType::SystemBoolean: DrawFieldControl<bool>(entity, name, field, scriptClass, instance); break;
							}
						}
					}
				}
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

	void PropertiesPanel::DrawFloatControl(const std::string& name, float& changeValue, float resetValue, float speed, float min, float max, const char* format)
	{
		DrawControlLabel(name, { m_LabelWidth, 0.0f });
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
		if (ImGui::DragFloat(label.c_str(), &dragVal, speed, min, max, format))
			CommandHistory::AddCommand(new ChangeValueCommand(dragVal, changeValue));
		ImGui::PopItemWidth();
		if (ImGui::IsItemHovered())
			ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
	}

	template<typename T>
	void PropertiesPanel::DrawIntControl(const std::string& name, T& changeValue, T resetValue, float speed, int min, int max, const char* format)
	{
		DrawControlLabel(name, { m_LabelWidth, 0.0f });
		ImGui::SameLine();

		if (ImGui::IsItemHovered())
		{
			ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
			if (ImGui::IsMouseDoubleClicked(0))
				CommandHistory::AddCommand(new ChangeValueCommand(resetValue, changeValue));
		}

		std::string label = "##" + name;
		int dragVal = static_cast<int>(changeValue);

		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
		if (ImGui::DragInt(label.c_str(), &dragVal, 1.0f, 0))
			CommandHistory::AddCommand(new ChangeValueCommand<T>(dragVal, (T&)changeValue));
		ImGui::PopItemWidth();

		if (ImGui::IsItemHovered())
			ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
	}

	void PropertiesPanel::DrawBoolControl(const std::string& name, bool& changeValue)
	{
		DrawControlLabel(name, { m_LabelWidth, 0.0f });
		ImGui::SameLine();

		if (ImGui::IsItemHovered())
		{
			ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
			if (ImGui::IsMouseDoubleClicked(0))
				CommandHistory::AddCommand(new ChangeValueCommand(false, changeValue));
		}

		bool val = static_cast<bool>(changeValue);
		std::string label = "##" + name;
		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
		if (ImGui::Checkbox(label.c_str(), &val))
			CommandHistory::AddCommand(new ChangeValueCommand(val, changeValue));
		ImGui::PopItemWidth();
	}

	void PropertiesPanel::DrawColorControl(const std::string& name, glm::vec4& colorValue)
	{
		DrawControlLabel("Color", { m_LabelWidth, 0.0f });
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
	
	void PropertiesPanel::DrawVec2Control(const std::string& name, glm::vec2& values, float resetValue, float speed, float min, float max, const char* format)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, LocusColors::Transparent);
		ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, { 0.0f, 1.0f });

		glm::vec2 dragValues = values;
		if (name == "Rotation")
			dragValues = glm::degrees(dragValues);

		DrawControlLabel(name.c_str(), { m_LabelWidth, 0.0f });

		ImGui::SameLine();

		ImGui::BeginTable("Vec3Control", 2);

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0.0f, 0.0f });
		// X
		ImGui::TableNextColumn();
		ImGui::Button("X", { 0.0f, 0.0f });
		if (ImGui::IsItemHovered())
		{
			ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
			if (ImGui::IsMouseDoubleClicked(0))
				CommandHistory::AddCommand(new ChangeValueCommand(resetValue, values.x));
		}
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
		if (ImGui::IsItemHovered())
		{
			ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
			if (ImGui::IsMouseDoubleClicked(0))
				CommandHistory::AddCommand(new ChangeValueCommand(resetValue, values.y));
		}
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

	bool PropertiesPanel::DrawVec3Control(const std::string& name, glm::vec3& values, float resetValue, float speed, float min, float max, const char* format)
	{
		bool isChanged = false;
		ImGui::PushStyleColor(ImGuiCol_Button, LocusColors::Transparent);
		ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, { 0.0f, 1.0f });

		glm::vec3 dragValues = values;
		if (name == "Rotation")
			dragValues = glm::degrees(dragValues);

		DrawControlLabel(name.c_str(), { m_LabelWidth, 0.0f });

		ImGui::SameLine();

		ImGui::BeginTable("Vec3Control", 3);

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0.0f, 0.0f });
		// X
		ImGui::TableNextColumn();
		ImGui::Button("X", { 0.0f, 0.0f });
		if (ImGui::IsItemHovered())
		{
			ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
			if (ImGui::IsMouseDoubleClicked(0))
			{
				CommandHistory::AddCommand(new ChangeValueCommand(resetValue, values.x));
				isChanged = true;
			}
		}
		ImGui::SameLine();
		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
		if(ImGui::DragFloat("##X", &dragValues.x, speed, min, max, format))
		{
			if (name == "Rotation")
			{
				CommandHistory::AddCommand(new ChangeValueCommand(glm::radians(dragValues.x), values.x));
				isChanged = true;
			}
			else
			{
				CommandHistory::AddCommand(new ChangeValueCommand(dragValues.x, values.x));
				isChanged = true;
			}
		}
		if (ImGui::IsItemHovered())
			ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
		ImGui::PopItemWidth();

		// Y
		ImGui::TableNextColumn();
		ImGui::Button("Y", { 0.0f, 0.0f });
		if (ImGui::IsItemHovered())
		{
			ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
			if (ImGui::IsMouseDoubleClicked(0))
			{
				CommandHistory::AddCommand(new ChangeValueCommand(resetValue, values.y));
				isChanged = true;
			}
		}
		ImGui::SameLine();
		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
		if (ImGui::DragFloat("##Y", &dragValues.y, speed, min, max, format))
		{
			if (name == "Rotation")
			{
				CommandHistory::AddCommand(new ChangeValueCommand(glm::radians(dragValues.y), values.y));
				isChanged = true;
			}
			else
			{
				CommandHistory::AddCommand(new ChangeValueCommand(dragValues.y, values.y));
				isChanged = true;
			}
		}
		if (ImGui::IsItemHovered())
			ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
		ImGui::PopItemWidth();

		// Z
		ImGui::TableNextColumn();
		ImGui::Button("Z", { 0.0f, 0.0f });
		if (ImGui::IsItemHovered())
		{
			ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
			if (ImGui::IsMouseDoubleClicked(0))
			{
				CommandHistory::AddCommand(new ChangeValueCommand(resetValue, values.z));
				isChanged = true;
			}
		}
		ImGui::SameLine();
		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
		if (ImGui::DragFloat("##Z", &dragValues.z, speed, min, max, format))
		{
			if (name == "Rotation")
			{
				CommandHistory::AddCommand(new ChangeValueCommand(glm::radians(dragValues.z), values.z));
				isChanged = true;
			}
			else
			{
				CommandHistory::AddCommand(new ChangeValueCommand(dragValues.z, values.z));
				isChanged = true;
			}
		}
		if (ImGui::IsItemHovered())
			ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::EndTable();
		ImGui::PopStyleColor();
		ImGui::PopStyleVar();

		if (Input::IsKeyPressed(Key::Z) && Input::IsKeyPressed(Key::LeftControl))
			isChanged = true;
		return isChanged;
	}

	template<typename T>
	void PropertiesPanel::DrawFieldControl(Entity entity, const std::string& name, const ScriptClassField& field, Ref<ScriptClass> scriptClass, Ref<ScriptInstance> instance)
	{
		// During scene runtime
		if (instance)
		{
			DrawControlLabel(name, { m_LabelWidth, 0.0f });
			T data = instance->GetFieldValue<T>(name);
			std::string label = "##" + name;

			if (typeid(T) == typeid(float))
			{
				if (ImGui::DragFloat(label.c_str(), &(float)data))
					instance->SetFieldValue(name, data); // TODO: Use command pattern
				
			}
			if (typeid(T) == typeid(int))
			{
				if (ImGui::DragInt(label.c_str(), &(int)data))
					instance->SetFieldValue(name, data);
			}
			if (typeid(T) == typeid(bool))
			{
				bool checked = &data;
				if (ImGui::Checkbox(label.c_str(), &checked))
					instance->SetFieldValue(name, checked);
			}
		}
		// During editor
		else
		{
			const auto& fields = scriptClass->GetPublicFields();
			auto& fieldInstances = ScriptEngine::GetFieldInstances(entity.GetUUID());

			if (fieldInstances.find(name) == fieldInstances.end())
			{
				ScriptClassFieldInstance& scriptField = fieldInstances[name];
				scriptField.Field = field;
				T data = scriptClass->GetFieldValue<T>(name);
				scriptField.SetValue(data);
			}
			else
			{
				ScriptClassFieldInstance& scriptField = fieldInstances[name];
				if (typeid(T) == typeid(float))
					DrawFloatControl(name, *(float*)scriptField.m_Buffer, scriptClass->GetFieldValue<float>(name));
				if (typeid(T) == typeid(int))
					DrawIntControl(name, *(int*)scriptField.m_Buffer, scriptClass->GetFieldValue<int>(name));
				if (typeid(T) == typeid(bool))
					DrawBoolControl(name, *(bool*)scriptField.m_Buffer);
			}
		}
	}

	template<typename T>
	void PropertiesPanel::CopyComponentToClipboard(Entity selectedEntity)
	{
		if (typeid(T) == typeid(TransformComponent))
		{
			m_ClipboardComponent.Transform = CreateRef<TransformComponent>(m_SelectedEntity.GetComponent<TransformComponent>());
			m_ClipboardComponentType = ComponentType::Transform;
		}
		if (typeid(T) == typeid(SpriteRendererComponent))
		{
			m_ClipboardComponent.SpriteRenderer = CreateRef<SpriteRendererComponent>(m_SelectedEntity.GetComponent<SpriteRendererComponent>());
			m_ClipboardComponentType = ComponentType::SpriteRenderer;
		}
		if (typeid(T) == typeid(CircleRendererComponent))
		{
			m_ClipboardComponent.CircleRenderer = CreateRef<CircleRendererComponent>(m_SelectedEntity.GetComponent<CircleRendererComponent>());
			m_ClipboardComponentType = ComponentType::CircleRenderer;
		}
		if (typeid(T) == typeid(CameraComponent))
		{
			m_ClipboardComponent.Camera = CreateRef<CameraComponent>(m_SelectedEntity.GetComponent<CameraComponent>());
			m_ClipboardComponentType = ComponentType::Camera;
		}
		if (typeid(T) == typeid(Rigidbody2DComponent))
		{
			m_ClipboardComponent.Rigidbody2D = CreateRef<Rigidbody2DComponent>(m_SelectedEntity.GetComponent<Rigidbody2DComponent>());
			m_ClipboardComponentType = ComponentType::Rigidbody2D;
		}
		if (typeid(T) == typeid(BoxCollider2DComponent))
		{
			m_ClipboardComponent.BoxCollider2D = CreateRef<BoxCollider2DComponent>(m_SelectedEntity.GetComponent<BoxCollider2DComponent>());
			m_ClipboardComponentType = ComponentType::BoxCollider2D;
		}
		if (typeid(T) == typeid(CircleCollider2DComponent))
		{
			m_ClipboardComponent.CircleCollider2D = CreateRef<CircleCollider2DComponent>(m_SelectedEntity.GetComponent<CircleCollider2DComponent>());
			m_ClipboardComponentType = ComponentType::CircleCollider2D;
		}
		if (typeid(T) == typeid(NativeScriptComponent))
		{
			m_ClipboardComponent.NativeScript = CreateRef<NativeScriptComponent>(m_SelectedEntity.GetComponent<NativeScriptComponent>());
			m_ClipboardComponentType = ComponentType::NativeScript;
		}
	}

	void PropertiesPanel::PasteComponent(Entity selectedEntity)
	{
		switch (m_ClipboardComponentType)
		{
		case Locus::ComponentType::None:
			break;
		case Locus::ComponentType::Transform: selectedEntity.AddOrReplaceComponent<TransformComponent>(*m_ClipboardComponent.Transform);
			break;
		case Locus::ComponentType::SpriteRenderer: selectedEntity.AddOrReplaceComponent<SpriteRendererComponent>(*m_ClipboardComponent.SpriteRenderer);
			break;
		case Locus::ComponentType::CircleRenderer: selectedEntity.AddOrReplaceComponent<CircleRendererComponent>(*m_ClipboardComponent.CircleRenderer);
			break;
		case Locus::ComponentType::Camera: selectedEntity.AddOrReplaceComponent<CameraComponent>(*m_ClipboardComponent.Camera);
			break;
		case Locus::ComponentType::Rigidbody2D: selectedEntity.AddOrReplaceComponent<Rigidbody2DComponent>(*m_ClipboardComponent.Rigidbody2D);
			break;
		case Locus::ComponentType::BoxCollider2D: selectedEntity.AddOrReplaceComponent<BoxCollider2DComponent>(*m_ClipboardComponent.BoxCollider2D);
			break;
		case Locus::ComponentType::CircleCollider2D: selectedEntity.AddOrReplaceComponent<CircleCollider2DComponent>(*m_ClipboardComponent.CircleCollider2D);
			break;
		case Locus::ComponentType::NativeScript: selectedEntity.AddOrReplaceComponent<NativeScriptComponent>(*m_ClipboardComponent.NativeScript);
			break;
		default:
			break;
		}
	}
}