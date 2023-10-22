#include "PropertiesPanel.h"

#include <filesystem>

#include <ImGui/imgui.h>
#include <ImGui/imgui_internal.h>
#include <glm/gtc/type_ptr.hpp>

#include "Command/Command.h"
#include "Command/CommandHistory.h"
#include "Command/EntityCommands.h"
#include "Command/ValueCommands.h"
#include "Widgets/Widgets.h"

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
		
		// Create textures
		m_ShowMoreButton = Texture2D::Create("resources/icons/ShowMoreButton.png");
		m_FolderIcon = Texture2D::Create("resources/icons/FolderIcon.png");

		m_ScriptClasses = ScriptEngine::GetClassNames();
	}

	void PropertiesPanel::OnImGuiRender()
	{
		// Panel styling
		float windowWidth = ImGui::GetWindowSize().x;
		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoScrollbar;
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 10.0f, 10.0f });
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 4.0f, 2.0f });
		ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, 10.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, { 0.0f, 0.5f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, LocusColors::Orange);
		ImGui::Begin(" Properties ", false, windowFlags);

		m_LabelWidth = 60.0f * ( 1.0f + ((ImGui::GetWindowSize().x / windowWidth) * 5.0f));

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

	// Draws the component to the panel. Takes in a function for component specific data to display. 
	// Styling is all done in this function so all components have the same general styling.
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
			// Component dropdown when clicking on three dots on the right.
			if (ImGui::BeginPopup("Component Settings"))
			{
				if (name != "Transform")
				{
					if (ImGui::MenuItem("Remove Component"))
						removeComponent = true;
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
			// Tag
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

			// Enabled
			bool enabled = entity.GetComponent<TagComponent>().Enabled;
			if (ImGui::Checkbox("##Enabled", &enabled))
				entity.GetComponent<TagComponent>().Enabled = enabled;

			if (!ImGui::TempInputIsActive(ImGui::GetActiveID()))
				Application::Get().GetImGuiLayer()->BlockEvents(true);
			else
				Application::Get().GetImGuiLayer()->BlockEvents(false);

			// Groups (Not yet implemented)
			Widgets::DrawControlLabel("Group", { 0.0f, 0.0f });
			ImGui::SameLine();
			ImGui::Button("Enemy (temp)", { 150.0f, 0.0f });
		}

		// --- Transform Component --------------------------------------------
		DrawComponentUI<TransformComponent>("Transform", entity, [this](auto& component)
			{
				// Position
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 5.0f, 0.0f });
				Widgets::DrawVec3Control("Position", component.LocalPosition, m_LabelWidth, 0.0f, 0.01f, "%.2f");
				
				// Rotation
				Widgets::DrawVec3Control("Rotation", component.LocalRotation, m_LabelWidth, 0.0f, 0.01f, "%.2f");
				component.SetLocalRotation(component.LocalRotation);
				ImGui::PopStyleVar();

				// Scale
				// Bottom spacing gets removed if applying styling to all three controls. 
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 5.0f, 5.0f });
				Widgets::DrawVec3Control("Scale", component.LocalScale, m_LabelWidth, 1.0f, 0.01f, "%.2f");
				ImGui::PopStyleVar();

			});

		// --- Camera Component -----------------------------------------------
		DrawComponentUI<CameraComponent>("Camera", entity, [this](auto& component)
			{
				SceneCamera& camera = component.Camera;

				// Primary camera check
				Widgets::DrawBoolControl("Primary", m_LabelWidth, component.Primary);

				// Background color
				Widgets::DrawColorControl("Background Color", camera.GetBackgroundColor(), m_LabelWidth);

				// Projection mode dropdown
				Widgets::DrawControlLabel("Projection", { m_LabelWidth, 0.0f });
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
					Widgets::DrawValueControl("Size", m_LabelWidth, camera.GetOrthographicSize(), 5.0f);
					// Near
					Widgets::DrawValueControl("Near Clip", m_LabelWidth, camera.GetOrthographicNearClip(), -1.0f, 0.1f, "%.1f");
					// Far
					Widgets::DrawValueControl("Far Clip", m_LabelWidth, camera.GetOrthographicFarClip(), 1000.0f, 0.1f, "%.1f");
					// Fixed Aspect Ratio
					Widgets::DrawBoolControl("Fixed Aspect Ratio", m_LabelWidth, component.FixedAspectRatio);
				}

				// Perspective settings
				if (camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective)
				{
					// FOV TODO: Fix
					float fov = glm::degrees(camera.GetPerspectiveFOV());
					Widgets::DrawValueControl("FOV", m_LabelWidth, fov, 45.0f);
					camera.SetPerspectiveFOV(glm::radians(fov));
					// Near
					Widgets::DrawValueControl("Near Clip", m_LabelWidth, camera.GetPerspectiveNearClip(), -1.0f, 0.1f, "%.1f");
					// Far
					Widgets::DrawValueControl("Far Clip", m_LabelWidth, camera.GetPerspectiveFarClip(), 1000.0f, 0.1f, "%.1f");
				}
			});

		// --- SpriteRenderer Component ---------------------------------------
		DrawComponentUI<SpriteRendererComponent>("Sprite Renderer", entity, [this](auto& component)
			{
				// Sprite
				Widgets::DrawControlLabel("Sprite", { m_LabelWidth, 50.0f });
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
				Widgets::DrawColorControl("Color", component.Color, m_LabelWidth);
				// Tiling Factor
				Widgets::DrawValueControl("Tiling Factor", m_LabelWidth, component.TilingFactor);
			});

		// --- Circle Renderer Component --------------------------------------
		DrawComponentUI<CircleRendererComponent>("Circle Renderer", entity, [this](auto& component)
			{
				Widgets::DrawColorControl("Color", component.Color, m_LabelWidth);
				Widgets::DrawValueControl("Thickness", m_LabelWidth, component.Thickness, 1.0f, 0.01f, "%.3f", true, 0.0f, 1.0f);
				Widgets::DrawValueControl("Fade", m_LabelWidth, component.Fade, 0.005f, 0.01f, "%.3f", true, 0.0001f, FLT_MAX);
			});

		// --- Rigidbody2D Component ------------------------------------------
		DrawComponentUI<Rigidbody2DComponent>("Rigidbody 2D", entity, [this](auto& component)
			{
				// Body type dropdown
				Widgets::DrawControlLabel("Body Type", { m_LabelWidth, 0.0f });
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
				Widgets::DrawValueControl("Mass", m_LabelWidth, component.Mass);
				// Gravity Scale
				Widgets::DrawValueControl("Gravity Scale", m_LabelWidth, component.GravityScale);
				// Linear Drag
				Widgets::DrawValueControl("Linear Damping", m_LabelWidth, component.LinearDamping, 0.2f);
				// Angular Drag
				Widgets::DrawValueControl("Angular Damping", m_LabelWidth, component.AngularDamping, 0.2f);

				// Fixed Rotation
				Widgets::DrawBoolControl("Fixed Rotation", m_LabelWidth, component.FixedRotation);

				// Friction
				Widgets::DrawValueControl("Friction", m_LabelWidth, component.Friction, 0.2f);
				// Restitution
				Widgets::DrawValueControl("Restitution", m_LabelWidth, component.Restitution, 0.0f);
				// Restitution Threshold
				Widgets::DrawValueControl("Restitution Threshold", m_LabelWidth, component.RestitutionThreshold, 0.0f);
			});

		// --- BoxCollider2D Component ----------------------------------------
		DrawComponentUI<BoxCollider2DComponent>("Box Collider 2D", entity, [this](auto& component)
			{
				// Collision Layer
				Widgets::DrawValueControl("Collision Layer", m_LabelWidth, component.CollisionLayer);
				// Size
				Widgets::DrawVec2Control("Size", component.Size, m_LabelWidth, 1.0f);
				// Offset
				Widgets::DrawVec2Control("Offset", component.Offset, m_LabelWidth);
			});

		// --- CircleCollider2D Component -------------------------------------
		DrawComponentUI<CircleCollider2DComponent>("Circle Collider 2D", entity, [this](auto& component)
			{
				// Collision Layer
				Widgets::DrawValueControl("Collision Layer", m_LabelWidth, component.CollisionLayer);
				// Radius
				Widgets::DrawValueControl("Radius", m_LabelWidth, component.Radius, 0.5f);
				// Offset
				Widgets::DrawVec2Control("Offset", component.Offset, m_LabelWidth);
			});

		// --- Script ---------------------------------------------------------
		DrawComponentUI<ScriptComponent>("Script", entity, [this, entity](auto& component) mutable
			{
				// Script Class dropdown
				Widgets::DrawControlLabel("Script Class", { m_LabelWidth, 0.0f });
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
					DrawScriptFields(entity, component);
			});
	}

	void PropertiesPanel::DrawScriptFields(Entity entity, ScriptComponent& component)
	{
		// This function is very repetitive but works for now. 

		Ref<ScriptClass> scriptClass = ScriptEngine::GetScriptClass(component.ScriptClass);
		Ref<ScriptInstance> instance = ScriptEngine::GetScriptInstance(entity.GetUUID());

		if (scriptClass)
		{
			const auto& fields = scriptClass->GetPublicFields();
			for (const auto& [name, field] : fields)
			{
				std::string label = "##" + name;
				if (instance) // Runtime controls
				{
					// TODO: Implement char, string vec, and entity controls.
					if (field.Type == FieldType::SystemSingle)
						Widgets::DrawFieldValueControl<float>(name, m_LabelWidth, instance->GetFieldValue<float>(name), scriptClass->GetFieldValue<float>(name), instance);
					else if (field.Type == FieldType::SystemDouble)
						Widgets::DrawFieldValueControl<double>(name, m_LabelWidth, instance->GetFieldValue<double>(name), scriptClass->GetFieldValue<double>(name), instance);
					else if (field.Type == FieldType::SystemShort)
						Widgets::DrawFieldValueControl<int16_t>(name, m_LabelWidth, instance->GetFieldValue<int16_t>(name), scriptClass->GetFieldValue<int16_t>(name), instance);
					else if (field.Type == FieldType::SystemInt)
						Widgets::DrawFieldValueControl<int>(name, m_LabelWidth, instance->GetFieldValue<int>(name), scriptClass->GetFieldValue<int>(name), instance);
					else if (field.Type == FieldType::SystemLong)
						Widgets::DrawFieldValueControl<int64_t>(name, m_LabelWidth, instance->GetFieldValue<int64_t>(name), scriptClass->GetFieldValue<int64_t>(name), instance);
					else if (field.Type == FieldType::SystemUShort)
						Widgets::DrawFieldValueControl<uint16_t>(name, m_LabelWidth, instance->GetFieldValue<uint16_t>(name), scriptClass->GetFieldValue<uint16_t>(name), instance);
					else if (field.Type == FieldType::SystemUInt)
						Widgets::DrawFieldValueControl<uint32_t>(name, m_LabelWidth, instance->GetFieldValue<uint32_t>(name), scriptClass->GetFieldValue<uint32_t>(name), instance);
					else if (field.Type == FieldType::SystemULong)
						Widgets::DrawFieldValueControl<uint64_t>(name, m_LabelWidth, instance->GetFieldValue<uint64_t>(name), scriptClass->GetFieldValue<uint64_t>(name), instance);
					else if (field.Type == FieldType::SystemBoolean)
						Widgets::DrawFieldBoolControl(name, m_LabelWidth, instance->GetFieldValue<bool>(name), scriptClass->GetFieldValue<bool>(name), instance);
				}
				else // Editor controls
				{
					auto& fieldInstances = ScriptEngine::GetFieldInstances(entity.GetUUID());
					// Createa a ScriptClassFieldInstance if not contained in fieldInstances. Then set the default value defined in the C# script.
					if (fieldInstances.find(name) == fieldInstances.end())
					{
						ScriptClassFieldInstance& scriptField = fieldInstances[name];
						scriptField.Field = field;
						
						if (field.Type == FieldType::SystemSingle)
							scriptField.SetValue(scriptClass->GetFieldValue<float>(name));
						else if (field.Type == FieldType::SystemDouble)
							scriptField.SetValue(scriptClass->GetFieldValue<double>(name));
						else if (field.Type == FieldType::SystemShort)
							scriptField.SetValue(scriptClass->GetFieldValue<int16_t>(name));
						else if (field.Type == FieldType::SystemInt)
							scriptField.SetValue(scriptClass->GetFieldValue<int>(name));
						else if (field.Type == FieldType::SystemLong)
							scriptField.SetValue(scriptClass->GetFieldValue<int64_t>(name));
						else if (field.Type == FieldType::SystemUShort)
							scriptField.SetValue(scriptClass->GetFieldValue<uint16_t>(name));
						else if (field.Type == FieldType::SystemUInt)
							scriptField.SetValue(scriptClass->GetFieldValue<uint32_t>(name));
						else if (field.Type == FieldType::SystemULong)
							scriptField.SetValue(scriptClass->GetFieldValue<uint64_t>(name));
						else if (field.Type == FieldType::SystemBoolean)
							scriptField.SetValue(scriptClass->GetFieldValue<bool>(name));
					}
					else
					{
						// Draw field controls to editor.
						ScriptClassFieldInstance& scriptField = fieldInstances.at(name);

						if (field.Type == FieldType::SystemSingle)
							Widgets::DrawValueControl<float>(name, m_LabelWidth, *(float*)scriptField.m_Buffer, scriptClass->GetFieldValue<float>(name));
						else if (field.Type == FieldType::SystemDouble)
							Widgets::DrawValueControl<double>(name, m_LabelWidth, *(double*)scriptField.m_Buffer, scriptClass->GetFieldValue<double>(name));
						else if (field.Type == FieldType::SystemShort)
							Widgets::DrawValueControl<int16_t>(name, m_LabelWidth, *(int16_t*)scriptField.m_Buffer, scriptClass->GetFieldValue<int16_t>(name));
						else if (field.Type == FieldType::SystemInt)
							Widgets::DrawValueControl<int>(name, m_LabelWidth, *(int*)scriptField.m_Buffer, scriptClass->GetFieldValue<int>(name));
						else if (field.Type == FieldType::SystemLong)
							Widgets::DrawValueControl<int64_t>(name, m_LabelWidth, *(int64_t*)scriptField.m_Buffer, scriptClass->GetFieldValue<int64_t>(name));
						else if (field.Type == FieldType::SystemUShort)
							Widgets::DrawValueControl<uint16_t>(name, m_LabelWidth, *(uint16_t*)scriptField.m_Buffer, scriptClass->GetFieldValue<uint16_t>(name));
						else if (field.Type == FieldType::SystemUInt)
							Widgets::DrawValueControl<uint32_t>(name, m_LabelWidth, *(uint32_t*)scriptField.m_Buffer, scriptClass->GetFieldValue<uint32_t>(name));
						else if (field.Type == FieldType::SystemULong)
							Widgets::DrawValueControl<uint64_t>(name, m_LabelWidth, *(uint64_t*)scriptField.m_Buffer, scriptClass->GetFieldValue<uint64_t>(name));
						else if (field.Type == FieldType::SystemBoolean)
							Widgets::DrawBoolControl(name, m_LabelWidth, *(bool*)scriptField.m_Buffer, scriptClass->GetFieldValue<bool>(name));
					}
				}
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