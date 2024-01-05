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
	extern Entity g_SelectedEntity;

	PropertiesPanel::PropertiesPanel()
	{
		m_ProjectDirectory = Application::Get().GetProjectPath() / "Assets";
	}

	void PropertiesPanel::SetScene(const Ref<Scene>& context)
	{
		m_ActiveScene = context;

		// Create textures
		m_MenuIcon = Texture2D::Create("resources/icons/MenuIcon.png");
		m_FolderIcon = Texture2D::Create("resources/icons/FolderIcon.png");

		m_ScriptClasses = ScriptEngine::GetClassNames();
	}

	void PropertiesPanel::OnImGuiRender()
	{
		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_TabBarAlignLeft | ImGuiWindowFlags_DockedWindowBorder;
		ImGui::Begin("Properties", false, windowFlags);

		if (g_SelectedEntity.IsValid())
		{
			// --- Draw properties for all active components ---
			DrawComponents(g_SelectedEntity);


			// --- Draw Add Component button ---
			ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x * 0.5f - (ImGui::CalcTextSize("Add Component").x * 0.5f + ImGui::GetStyle().FramePadding.x));
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.0f);
			if (ImGui::Button("Add Component"))
				ImGui::OpenPopup("AddComponent");

			if (ImGui::BeginPopup("AddComponent"))
			{
				if (ImGui::MenuItem("Camera"))
				{
					if (!g_SelectedEntity.HasComponent<CameraComponent>())
						CommandHistory::AddCommand(new AddComponentCommand<CameraComponent>(m_ActiveScene, g_SelectedEntity));
					else
						LOCUS_CORE_WARN("This entity already has a Camera Component");
					ImGui::CloseCurrentPopup();
				}

				if (ImGui::MenuItem("Sprite Renderer"))
				{
					if (!g_SelectedEntity.HasComponent<SpriteRendererComponent>())
						CommandHistory::AddCommand(new AddComponentCommand<SpriteRendererComponent>(m_ActiveScene, g_SelectedEntity));
					else
						LOCUS_CORE_WARN("This entity already has a Sprite Renderer Component");
					ImGui::CloseCurrentPopup();
				}

				if (ImGui::MenuItem("Circle Renderer"))
				{
					if (!g_SelectedEntity.HasComponent<CircleRendererComponent>())
						CommandHistory::AddCommand(new AddComponentCommand<CircleRendererComponent>(m_ActiveScene, g_SelectedEntity));
					else
						LOCUS_CORE_WARN("This entity already has a Circle Renderer Component");
					ImGui::CloseCurrentPopup();
				}

				if (ImGui::MenuItem("Cube Renderer"))
				{
					if (!g_SelectedEntity.HasComponent<CubeRendererComponent>())
						CommandHistory::AddCommand(new AddComponentCommand<CubeRendererComponent>(m_ActiveScene, g_SelectedEntity));
					else
						LOCUS_CORE_WARN("This entity already has a Cube Renderer Component");
					ImGui::CloseCurrentPopup();
				}

				if (ImGui::MenuItem("Mesh Renderer"))
				{
					if (!g_SelectedEntity.HasComponent<MeshRendererComponent>())
						CommandHistory::AddCommand(new AddComponentCommand<MeshRendererComponent>(m_ActiveScene, g_SelectedEntity));
					else
						LOCUS_CORE_WARN("This entity already has a Mesh Renderer Component");
					ImGui::CloseCurrentPopup();
				}

				if (ImGui::MenuItem("Point Light"))
				{
					if (!g_SelectedEntity.HasComponent<PointLightComponent>())
						CommandHistory::AddCommand(new AddComponentCommand<PointLightComponent>(m_ActiveScene, g_SelectedEntity));
					else
						LOCUS_CORE_WARN("This entity already has a Point Light Component");
					ImGui::CloseCurrentPopup();
				}

				if (ImGui::MenuItem("Directional Light"))
				{
					if (!g_SelectedEntity.HasComponent<DirectionalLightComponent>())
						CommandHistory::AddCommand(new AddComponentCommand<DirectionalLightComponent>(m_ActiveScene, g_SelectedEntity));
					else
						LOCUS_CORE_WARN("This entity already has a Directional Light Component");
					ImGui::CloseCurrentPopup();
				}

				if (ImGui::MenuItem("Spot Light"))
				{
					if (!g_SelectedEntity.HasComponent<SpotLightComponent>())
						CommandHistory::AddCommand(new AddComponentCommand<SpotLightComponent>(m_ActiveScene, g_SelectedEntity));
					else
						LOCUS_CORE_WARN("This entity already has a Spot Light Component");
					ImGui::CloseCurrentPopup();
				}

				if (ImGui::MenuItem("Rigidbody 2D"))
				{
					if (!g_SelectedEntity.HasComponent<Rigidbody2DComponent>())
						CommandHistory::AddCommand(new AddComponentCommand<Rigidbody2DComponent>(m_ActiveScene, g_SelectedEntity));
					else
						LOCUS_CORE_WARN("This entity already has a Rigidbody2D Component");
					ImGui::CloseCurrentPopup();
				}

				if (ImGui::MenuItem("Box Collider 2D"))
				{
					if (!g_SelectedEntity.HasComponent<BoxCollider2DComponent>())
						CommandHistory::AddCommand(new AddComponentCommand<BoxCollider2DComponent>(m_ActiveScene, g_SelectedEntity));
					else
						LOCUS_CORE_WARN("This entity already has a BoxCollider2D Component");
					ImGui::CloseCurrentPopup();
				}

				if (ImGui::MenuItem("Circle Collider 2D"))
				{
					if (!g_SelectedEntity.HasComponent<CircleCollider2DComponent>())
						CommandHistory::AddCommand(new AddComponentCommand<CircleCollider2DComponent>(m_ActiveScene, g_SelectedEntity));
					else
						LOCUS_CORE_WARN("This entity already has a CircleCollider2D Component");
					ImGui::CloseCurrentPopup();
				}

				if (ImGui::MenuItem("Script"))
				{
					if (!g_SelectedEntity.HasComponent<ScriptComponent>())
						CommandHistory::AddCommand(new AddComponentCommand<ScriptComponent>(m_ActiveScene, g_SelectedEntity));
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

		// --- Popup when clicking empty area ---
		if (ImGui::BeginPopupContextWindow(0, 1 | ImGuiPopupFlags_NoOpenOverItems))
		{
			if (m_ClipboardComponentType != ComponentType::None && g_SelectedEntity)
				if (ImGui::MenuItem("Paste Component"))
					PasteComponent(g_SelectedEntity);
			ImGui::EndPopup();
		}

		ImGui::End();
	}

	// Draws the component to the panel. Takes in a function for component specific data to display. 
	// Styling is all done in this function so all components have uniform styling.
	template<typename T, typename UIFunction>
	void PropertiesPanel::DrawComponentUI(const std::string& name, Entity entity, UIFunction uiFunction)
	{
		const ImGuiTreeNodeFlags treeNodeFlags =
			ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed |
			ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap |
			ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_HideArrow;

		if (entity.HasComponent<T>())
		{
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { ImGui::GetStyle().ItemSpacing.x, 3.0f });
			auto& component = entity.GetComponent<T>();

			bool removeComponent = false;

			ImGui::Separator();

			// TreeNode
			ImGui::PushStyleColor(ImGuiCol_Header, LocusColors::Transparent);
			ImGui::PushStyleColor(ImGuiCol_HeaderHovered, LocusColors::Transparent);
			ImGui::PushStyleColor(ImGuiCol_HeaderActive, LocusColors::Transparent);
			ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 3.0f, ImGui::GetStyle().FramePadding.y });
			ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
			bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, name.c_str());
			ImGui::PopFont();
			ImGui::PopStyleVar();
			ImGui::PopStyleColor(3);
			float menuButtonSize = ImGui::GetItemRectSize().y;

			ImGui::SameLine(ImGui::GetItemRectSize().x - menuButtonSize);

			// TreeNode Menu button
			ImGui::PushStyleColor(ImGuiCol_Button, LocusColors::Transparent);
			if (ImGui::ImageButton((ImTextureID)(uint64_t)m_MenuIcon->GetRendererID(), { menuButtonSize, menuButtonSize }, ImVec2(0, 1), ImVec2(1, 0), 0))
				ImGui::OpenPopup("Component Settings");
			ImGui::PopStyleColor();
			ImGui::PopStyleVar();

			// TreeNode Menu popup
			if (ImGui::BeginPopup("Component Settings"))
			{
				if (name != "Transform")
				{
					if (ImGui::MenuItem("Remove Component"))
						removeComponent = true;
				}
				if (ImGui::MenuItem("Copy Component"))
					CopyComponentToClipboard<T>(g_SelectedEntity);
				ImGui::EndPopup();
			}

			if (open)
			{
				uiFunction(component);
				ImGui::TreePop();
			}

			if (removeComponent)
				CommandHistory::AddCommand(new RemoveComponentCommand<T>(m_ActiveScene, entity));

			ImGui::PopStyleVar();
		}
	}

	void PropertiesPanel::DrawComponents(Entity entity)
	{
		// --- Tag Component --------------------------------------------------
		if (entity.HasComponent<TagComponent>())
		{
			// Tag
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0.0f, ImGui::GetStyle().ItemSpacing.x });
			auto& tag = entity.GetComponent<TagComponent>().Tag;
			Widgets::DrawStringControl("Tag", tag, "Empty Entity", 0, ImGui::GetWindowWidth() * 0.5f - ImGui::CalcTextSize("Tag").x - ImGui::GetStyle().FramePadding.x * 2.0f);

			ImGui::SameLine();

			// Groups TODO: replace this with dropdown
			auto& group = entity.GetComponent<TagComponent>().Group;
			Widgets::DrawStringControl("Group", group, "Default", 0);
			ImGui::PopStyleVar();
		}

		// --- Transform Component --------------------------------------------
		DrawComponentUI<TransformComponent>("Transform", entity, [this](auto& component)
			{
				// Position
				Widgets::DrawVec3Control("Position", component.LocalPosition, { 0.0f, 0.0f, 0.0f }, 0.01f, "%.2f");

				// Rotation
				Widgets::DrawVec3Control("Rotation", component.LocalRotation, { 0.0f, 0.0f, 0.0f }, 0.01f, "%.2f");
				component.SetLocalRotation(component.LocalRotation);

				// Scale
				Widgets::DrawVec3Control("Scale", component.LocalScale, { 1.0f, 1.0f, 1.0f }, 0.01f, "%.2f");
			});

		// --- Camera Component -----------------------------------------------
		DrawComponentUI<CameraComponent>("Camera", entity, [this](auto& component)
			{
				SceneCamera& camera = component.Camera;

				// Primary camera check
				Widgets::DrawBoolControl("Primary", component.Primary);

				// Background color
				Widgets::DrawColorControl("Background Color", camera.GetBackgroundColor(), { 0.25f, 0.5f, 0.5f, 1.0f });

				// Projection mode dropdown
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0.0f, ImGui::GetStyle().ItemSpacing.y });

				Widgets::DrawControlLabel("Projection", { ImGui::GetContentRegionAvail().x * 0.5f, 0.0f });
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
				ImGui::PopStyleVar();

				// Orthographic settings
				if (camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic)
				{
					// Size
					Widgets::DrawValueControl("Size", camera.GetOrthographicSize(), 5.0f);
					// Near
					Widgets::DrawValueControl("Near Clip", camera.GetOrthographicNearClip(), -1.0f, 0.1f, "%.1f");
					// Far
					Widgets::DrawValueControl("Far Clip", camera.GetOrthographicFarClip(), 1000.0f, 0.1f, "%.1f");
					// Fixed Aspect Ratio
					Widgets::DrawBoolControl("Fixed Aspect Ratio", component.FixedAspectRatio);
				}

				// Perspective settings
				if (camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective)
				{
					// FOV
					Widgets::DrawValueControl("FOV", camera.GetPerspectiveFOV(), 45.0f);
					// Near
					Widgets::DrawValueControl("Near Clip", camera.GetPerspectiveNearClip(), -1.0f, 0.1f, "%.1f");
					// Far
					Widgets::DrawValueControl("Far Clip", camera.GetPerspectiveFarClip(), 1000.0f, 0.1f, "%.1f");
				}
			});

		// --- SpriteRenderer Component ---------------------------------------
		DrawComponentUI<SpriteRendererComponent>("Sprite Renderer", entity, [this](auto& component)
			{
				// Sprite
				Widgets::DrawTextureDropdown("Sprite", component.Texture);
				// Color
				Widgets::DrawColorControl("Color", component.Color, { 1.0f, 1.0f, 1.0f, 1.0f});
				// Tiling Factor
				Widgets::DrawValueControl("Tiling Factor", component.TilingFactor);
			});

		// --- Circle Renderer Component --------------------------------------
		DrawComponentUI<CircleRendererComponent>("Circle Renderer", entity, [this](auto& component)
			{
				Widgets::DrawColorControl("Color", component.Color, { 1.0f, 1.0f, 1.0f, 1.0f });

				Widgets::DrawValueControl("Thickness", component.Thickness, 1.0f, 0.01f, "%.3f", -1.0f, -1.0f, 0.0f, 1.0f);

				Widgets::DrawValueControl("Fade", component.Fade, 0.005f, 0.01f, "%.3f", -1.0f, -1.0f, 0.0001f, FLT_MAX);
			});

		// --- Cube Renderer Component ----------------------------------------
		DrawComponentUI<CubeRendererComponent>("Cube Renderer", entity, [this](auto& component) 
			{
				Widgets::DrawMaterialDropdown("Material", component.Material);
			});

		DrawComponentUI<MeshRendererComponent>("Mesh Renderer", entity, [this](auto& component)
			{
				Widgets::DrawMaterialDropdown("Material", component.Material);
			});

		// --- Point Light Component ----------------------------------------
		DrawComponentUI<PointLightComponent>("Point Light", entity, [this](auto& component)
			{
				Widgets::DrawColorControl("Color", component.Color, { 1.0f, 1.0f, 1.0f, 1.0f });

				Widgets::DrawValueControl("Intensity", component.Intensity, 1.0f, 0.1f, nullptr, -1.0f, -1.0f, 0.0f, FLT_MAX);
			});

		// --- Directional Light Component ----------------------------------------
		DrawComponentUI<DirectionalLightComponent>("Directional Light", entity, [this](auto& component)
			{
				Widgets::DrawColorControl("Color", component.Color, { 1.0f, 1.0f, 1.0f, 1.0f });

				Widgets::DrawValueControl("Intensity", component.Intensity, 1.0f, 0.1f, nullptr, -1.0f, -1.0f, 0.0f, FLT_MAX);
			});

		// --- Spot Light Component ----------------------------------------
		DrawComponentUI<SpotLightComponent>("Spot Light", entity, [this](auto& component)
			{
				Widgets::DrawColorControl("Color", component.Color, { 1.0f, 1.0f, 1.0f, 1.0f });

				Widgets::DrawValueControl("Intensity", component.Intensity, 1.0f, 0.1f, nullptr, -1.0f, -1.0f, 0.0f, FLT_MAX);

				Widgets::DrawValueControl("CutOff", component.CutOff, 10.0f, 0.1f, nullptr, -1.0f, -1.0f, 0.0f, FLT_MAX);

				Widgets::DrawValueControl("OuterCutOff", component.OuterCutOff, 20.0f, 0.1f, nullptr, -1.0f, -1.0f, 0.0f, FLT_MAX);
			});

		// --- Rigidbody2D Component ------------------------------------------
		DrawComponentUI<Rigidbody2DComponent>("Rigidbody 2D", entity, [this](auto& component)
			{
				// Body type dropdown
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0.0f, ImGui::GetStyle().ItemSpacing.y });

				Widgets::DrawControlLabel("Body Type", { ImGui::GetContentRegionAvail().x * 0.5f, 0.0f });
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
							CommandHistory::AddCommand(new ChangeValueCommand((Rigidbody2DType)i, component.BodyType));

						if (isSelected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}
				ImGui::PopItemWidth();
				ImGui::PopStyleVar();

				// Mass
				Widgets::DrawValueControl("Mass", component.Mass);
				// Gravity Scale
				Widgets::DrawValueControl("Gravity Scale", component.GravityScale);
				// Linear Drag
				Widgets::DrawValueControl("Linear Damping", component.LinearDamping, 0.2f);
				// Angular Drag
				Widgets::DrawValueControl("Angular Damping", component.AngularDamping, 0.2f);
				// Fixed Rotation
				Widgets::DrawBoolControl("Fixed Rotation", component.FixedRotation);

				if (component.BodyType == Rigidbody2DType::Dynamic)
					Widgets::DrawBoolControl("IsBullet", component.IsBullet);
			});

		// --- BoxCollider2D Component ----------------------------------------
		DrawComponentUI<BoxCollider2DComponent>("Box Collider 2D", entity, [this](auto& component)
			{
				// Friction
				Widgets::DrawValueControl("Friction", component.Friction, 0.2f);
				// Restitution
				Widgets::DrawValueControl("Restitution", component.Restitution, 0.0f);
				// Restitution Threshold
				Widgets::DrawValueControl("Restitution Threshold", component.RestitutionThreshold, 0.0f);
				// Collision Category
				Widgets::DrawCollisionGrid("Collision Category", component.CollisionCategory, 0x0001);
				// Collision Mask
				Widgets::DrawCollisionGrid("Collision Mask", component.CollisionMask, 0xFFFF);
				// Size
				Widgets::DrawVec2Control("Size", component.Size);
				// Offset
				Widgets::DrawVec2Control("Offset", component.Offset, { 0.0f, 0.0f });
			});

		// --- CircleCollider2D Component -------------------------------------
		DrawComponentUI<CircleCollider2DComponent>("Circle Collider 2D", entity, [this](auto& component)
			{
				// Friction
				Widgets::DrawValueControl("Friction", component.Friction, 0.2f);
				// Restitution
				Widgets::DrawValueControl("Restitution", component.Restitution, 0.0f);
				// Restitution Threshold
				Widgets::DrawValueControl("Restitution Threshold", component.RestitutionThreshold, 0.0f);
				// Collision Category
				Widgets::DrawCollisionGrid("Collision Category", component.CollisionCategory, 0x0001);
				// Collision Mask
				Widgets::DrawCollisionGrid("Collision Mask", component.CollisionMask, 0xFFFF);
				// Radius
				Widgets::DrawValueControl("Radius", component.Radius, 0.5f);
				// Offset
				Widgets::DrawVec2Control("Offset", component.Offset, { 0.0f, 0.0f });
			});

		// --- Script ---------------------------------------------------------
		DrawComponentUI<ScriptComponent>("Script", entity, [this, entity](auto& component) mutable
			{
				// Script Class dropdown
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0.0f, ImGui::GetStyle().ItemSpacing.y });

				Widgets::DrawControlLabel("Script Class", { ImGui::GetContentRegionAvail().x * 0.5f, 0.0f });

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
				ImGui::PopStyleVar();

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
					// TODO: Implement string, vec4, and entity controls.
					if (field.Type == FieldType::SystemSingle)
					{
						float changeVal = instance->GetFieldValue<float>(name);
						Widgets::DrawValueControl<float>(name, changeVal, scriptClass->GetFieldValue<float>(name), 0.1f, nullptr, -1.0f, -1.0f, 0.0f, 0.0f, instance);
					}
					else if (field.Type == FieldType::SystemDouble)
					{
						double changeVal = instance->GetFieldValue<double>(name);
						Widgets::DrawValueControl<double>(name, changeVal, scriptClass->GetFieldValue<double>(name), 0.1f, nullptr, -1.0f, -1.0f, 0.0f, 0.0f, instance);
					}
					else if (field.Type == FieldType::SystemShort)
					{
						int16_t changeVal = instance->GetFieldValue<int16_t>(name);
						Widgets::DrawValueControl<int16_t>(name, changeVal, scriptClass->GetFieldValue<int16_t>(name), 0.1f, nullptr, -1.0f, -1.0f, 0, 0, instance);
					}
					else if (field.Type == FieldType::SystemInt)
					{
						int changeVal = instance->GetFieldValue<int>(name);
						Widgets::DrawValueControl<int>(name, changeVal, scriptClass->GetFieldValue<int>(name), 0.1f, nullptr, -1.0f, -1.0f, 0, 0, instance);
					}
					else if (field.Type == FieldType::SystemLong)
					{
						int64_t changeVal = instance->GetFieldValue<int64_t>(name);
						Widgets::DrawValueControl<int64_t>(name, changeVal, scriptClass->GetFieldValue<int64_t>(name), 0.1f, nullptr, -1.0f, -1.0f, 0, 0, instance);
					}
					else if (field.Type == FieldType::SystemUShort)
					{
						uint16_t changeVal = instance->GetFieldValue<uint16_t>(name);
						Widgets::DrawValueControl<uint16_t>(name, changeVal, scriptClass->GetFieldValue<uint16_t>(name), 0.1f, nullptr, -1.0f, -1.0f, 0, 0, instance);
					}
					else if (field.Type == FieldType::SystemUInt)
					{
						uint32_t changeVal = instance->GetFieldValue<uint32_t>(name);
						Widgets::DrawValueControl<uint32_t>(name, changeVal, scriptClass->GetFieldValue<uint32_t>(name), 0.1f, nullptr, -1.0f, -1.0f, 0, 0, instance);
					}
					else if (field.Type == FieldType::SystemULong)
					{
						uint64_t changeVal = instance->GetFieldValue<uint64_t>(name);
						Widgets::DrawValueControl<uint64_t>(name, changeVal, scriptClass->GetFieldValue<uint64_t>(name), 0.1f, nullptr, -1.0f, -1.0f, 0, 0, instance);
					}
					else if (field.Type == FieldType::SystemBoolean)
					{
						bool changeVal = instance->GetFieldValue<bool>(name);
						Widgets::DrawBoolControl(name, changeVal, scriptClass->GetFieldValue<bool>(name), -1.0f, instance);
					}
					else if (field.Type == FieldType::SystemChar)
					{
						char changeVal = instance->GetFieldValue<char>(name);
						Widgets::DrawCharControl(name, changeVal, scriptClass->GetFieldValue<char>(name), -1.0f, -1.0f, instance);
					}
					else if (field.Type == FieldType::LocusVec2)
					{
						glm::vec2 changeVal = instance->GetFieldValue<glm::vec2>(name);
						Widgets::DrawVec2Control(name, changeVal, scriptClass->GetFieldValue<glm::vec2>(name), 0.1f, nullptr, -1.0f, -1.0f, glm::vec2(0.0f), glm::vec2(0.0f), instance);
					}
					else if (field.Type == FieldType::LocusVec3)
					{
						glm::vec3 changeVal = instance->GetFieldValue<glm::vec3>(name);
						Widgets::DrawVec3Control(name, changeVal, scriptClass->GetFieldValue<glm::vec3>(name), 0.1f, nullptr, -1.0f, -1.0f, glm::vec3(0.0f), glm::vec3(0.0f), instance);
					}
				}
				else // Editor controls
				{
					auto& fieldInstances = ScriptEngine::GetFieldInstances(entity.GetUUID());
					// Create a ScriptClassFieldInstance if not contained in fieldInstances. Then set the default value defined in the C# script.
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
						else if (field.Type == FieldType::SystemChar)
							scriptField.SetValue(scriptClass->GetFieldValue<char>(name));
						else if (field.Type == FieldType::LocusVec2)
							scriptField.SetValue(scriptClass->GetFieldValue<glm::vec2>(name));
						else if (field.Type == FieldType::LocusVec3)
							scriptField.SetValue(scriptClass->GetFieldValue<glm::vec3>(name));
					}
					else
					{
						// Draw field controls to editor.
						ScriptClassFieldInstance& scriptField = fieldInstances.at(name);

						if (field.Type == FieldType::SystemSingle)
							Widgets::DrawValueControl<float>(name, *(float*)scriptField.m_Buffer, scriptClass->GetFieldValue<float>(name));
						else if (field.Type == FieldType::SystemDouble)
							Widgets::DrawValueControl<double>(name, *(double*)scriptField.m_Buffer, scriptClass->GetFieldValue<double>(name));
						else if (field.Type == FieldType::SystemShort)
							Widgets::DrawValueControl<int16_t>(name, *(int16_t*)scriptField.m_Buffer, scriptClass->GetFieldValue<int16_t>(name));
						else if (field.Type == FieldType::SystemInt)
							Widgets::DrawValueControl<int>(name, *(int*)scriptField.m_Buffer, scriptClass->GetFieldValue<int>(name));
						else if (field.Type == FieldType::SystemLong)
							Widgets::DrawValueControl<int64_t>(name, *(int64_t*)scriptField.m_Buffer, scriptClass->GetFieldValue<int64_t>(name));
						else if (field.Type == FieldType::SystemUShort)
							Widgets::DrawValueControl<uint16_t>(name, *(uint16_t*)scriptField.m_Buffer, scriptClass->GetFieldValue<uint16_t>(name));
						else if (field.Type == FieldType::SystemUInt)
							Widgets::DrawValueControl<uint32_t>(name, *(uint32_t*)scriptField.m_Buffer, scriptClass->GetFieldValue<uint32_t>(name));
						else if (field.Type == FieldType::SystemULong)
							Widgets::DrawValueControl<uint64_t>(name, *(uint64_t*)scriptField.m_Buffer, scriptClass->GetFieldValue<uint64_t>(name));
						else if (field.Type == FieldType::SystemBoolean)
							Widgets::DrawBoolControl(name, *(bool*)scriptField.m_Buffer, scriptClass->GetFieldValue<bool>(name));
						else if (field.Type == FieldType::SystemChar)
							Widgets::DrawCharControl(name, *(char*)scriptField.m_Buffer, scriptClass->GetFieldValue<char>(name));
						else if (field.Type == FieldType::LocusVec2)
							Widgets::DrawVec2Control(name, *(glm::vec2*)scriptField.m_Buffer, scriptClass->GetFieldValue<glm::vec2>(name));
						else if (field.Type == FieldType::LocusVec3)
							Widgets::DrawVec3Control(name, *(glm::vec3*)scriptField.m_Buffer, scriptClass->GetFieldValue<glm::vec3>(name));
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
			m_ClipboardComponent.Transform = CreateRef<TransformComponent>(g_SelectedEntity.GetComponent<TransformComponent>());
			m_ClipboardComponentType = ComponentType::Transform;
		}
		if (typeid(T) == typeid(SpriteRendererComponent))
		{
			m_ClipboardComponent.SpriteRenderer = CreateRef<SpriteRendererComponent>(g_SelectedEntity.GetComponent<SpriteRendererComponent>());
			m_ClipboardComponentType = ComponentType::SpriteRenderer;
		}
		if (typeid(T) == typeid(CircleRendererComponent))
		{
			m_ClipboardComponent.CircleRenderer = CreateRef<CircleRendererComponent>(g_SelectedEntity.GetComponent<CircleRendererComponent>());
			m_ClipboardComponentType = ComponentType::CircleRenderer;
		}
		if (typeid(T) == typeid(CubeRendererComponent))
		{
			m_ClipboardComponent.CubeRenderer = CreateRef<CubeRendererComponent>(g_SelectedEntity.GetComponent<CubeRendererComponent>());
			m_ClipboardComponentType = ComponentType::CubeRenderer;
		}
		if (typeid(T) == typeid(PointLightComponent))
		{
			m_ClipboardComponent.PointLight = CreateRef<PointLightComponent>(g_SelectedEntity.GetComponent<PointLightComponent>());
			m_ClipboardComponentType = ComponentType::PointLight;
		}
		if (typeid(T) == typeid(CameraComponent))
		{
			m_ClipboardComponent.Camera = CreateRef<CameraComponent>(g_SelectedEntity.GetComponent<CameraComponent>());
			m_ClipboardComponentType = ComponentType::Camera;
		}
		if (typeid(T) == typeid(Rigidbody2DComponent))
		{
			m_ClipboardComponent.Rigidbody2D = CreateRef<Rigidbody2DComponent>(g_SelectedEntity.GetComponent<Rigidbody2DComponent>());
			m_ClipboardComponentType = ComponentType::Rigidbody2D;
		}
		if (typeid(T) == typeid(BoxCollider2DComponent))
		{
			m_ClipboardComponent.BoxCollider2D = CreateRef<BoxCollider2DComponent>(g_SelectedEntity.GetComponent<BoxCollider2DComponent>());
			m_ClipboardComponentType = ComponentType::BoxCollider2D;
		}
		if (typeid(T) == typeid(CircleCollider2DComponent))
		{
			m_ClipboardComponent.CircleCollider2D = CreateRef<CircleCollider2DComponent>(g_SelectedEntity.GetComponent<CircleCollider2DComponent>());
			m_ClipboardComponentType = ComponentType::CircleCollider2D;
		}
		if (typeid(T) == typeid(ScriptComponent))
		{
			m_ClipboardComponent.Script = CreateRef<ScriptComponent>(g_SelectedEntity.GetComponent<ScriptComponent>());
			m_ClipboardComponentType = ComponentType::Script;
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
		case Locus::ComponentType::CubeRenderer: selectedEntity.AddOrReplaceComponent<CubeRendererComponent>(*m_ClipboardComponent.CubeRenderer);
			break;
		case Locus::ComponentType::PointLight: selectedEntity.AddOrReplaceComponent<PointLightComponent>(*m_ClipboardComponent.PointLight);
			break;
		case Locus::ComponentType::Camera: selectedEntity.AddOrReplaceComponent<CameraComponent>(*m_ClipboardComponent.Camera);
			break;
		case Locus::ComponentType::Rigidbody2D: selectedEntity.AddOrReplaceComponent<Rigidbody2DComponent>(*m_ClipboardComponent.Rigidbody2D);
			break;
		case Locus::ComponentType::BoxCollider2D: selectedEntity.AddOrReplaceComponent<BoxCollider2DComponent>(*m_ClipboardComponent.BoxCollider2D);
			break;
		case Locus::ComponentType::CircleCollider2D: selectedEntity.AddOrReplaceComponent<CircleCollider2DComponent>(*m_ClipboardComponent.CircleCollider2D);
			break;
		case Locus::ComponentType::Script: selectedEntity.AddOrReplaceComponent<ScriptComponent>(*m_ClipboardComponent.Script);
			break;
		default:
			break;
		}
	}
}
