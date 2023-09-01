#include "LocusEditorLayer.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>
#include <ImGuizmo.h>

namespace Locus
{
	extern const std::filesystem::path g_ProjectPath;

	LocusEditorLayer::LocusEditorLayer()
		: Layer("LocusEditorLayer")
	{
		m_PlayButton = Texture2D::Create("resources/icons/PlayButton.png");
		m_StopButton = Texture2D::Create("resources/icons/StopButton.png");
		m_PauseButton = Texture2D::Create("resources/icons/PauseButton.png");
	}

	LocusEditorLayer::~LocusEditorLayer()
	{
		CommandHistory::Shutdown();
	}

	void LocusEditorLayer::OnAttach()
	{
		LOCUS_PROFILE_FUNCTION();

		// Textures
		//m_CheckerboardTexture = Texture2D::Create("assets/textures/Checkerboard.png");
		//m_SpriteSheet = Texture2D::Create("assets/textures/TX_Tileset_Grass.png");
		//m_Grass = SubTexture2D::CreateFromCoords(m_SpriteSheet, { 1, 0 }, { 32, 32 }, { 1, 2 });

		// Framebuffer
		FramebufferSpecification framebufferSpecs;
		framebufferSpecs.Attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INT, FramebufferTextureFormat::Depth };
		framebufferSpecs.Width = 1920;
		framebufferSpecs.Height = 1080;
		m_Framebuffer = Framebuffer::Create(framebufferSpecs);

		// Scene
		m_EditorScene = CreateRef<Scene>();
		m_ActiveScene = m_EditorScene;

		// Open startup project given through args
		auto commandLineArgs = Application::Get().GetCommandLineArgs();
		if (commandLineArgs.Count > 1)
		{
			auto sceneFilePath = commandLineArgs[1];
			SceneSerializer serializer(m_ActiveScene);
			serializer.Deserialize(sceneFilePath);
			m_SavePath = sceneFilePath;
		}

		/*class CameraControls : public ScriptableEntity
		{
		public:
			virtual void OnCreate() override
			{

			}

			virtual void OnDestroy() override
			{

			}

			virtual void OnUpdate(Timestep deltaTime) override
			{
				auto& translation = GetComponent<TransformComponent>().Translation;
				float speed = 5.0f;

				if (Input::IsKeyPressed(Key::A))
					translation.x -= speed * deltaTime;
				if (Input::IsKeyPressed(Key::D))
					translation.x += speed * deltaTime;
				if (Input::IsKeyPressed(Key::W))
					translation.y += speed * deltaTime;
				if (Input::IsKeyPressed(Key::S))
					translation.y -= speed * deltaTime;
			}
		};*/

		// Panels
		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
		m_PropertiesPanel.SetContext(m_ActiveScene);

		// Editor Camera
		m_EditorCamera = EditorCamera(30.0f, 1920.0f / 1080.0f, 0.1f, 1000.0f);

		// TODO: set values on load
		m_ViewportHeight = 600.0f;
		m_HierarchyHeight = 400.0f;
		m_CenterSplitterPos = 1500.0f;
	}

	void LocusEditorLayer::OnDetach()
	{
		LOCUS_PROFILE_FUNCTION();
	}

	void LocusEditorLayer::OnUpdate(Timestep deltaTime)
	{
		// Profiling
		LOCUS_PROFILE_FUNCTION();
		Renderer2D::StatsStartFrame();

		// Resize
		if (FramebufferSpecification spec = m_Framebuffer->GetSpecification();
			m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f &&
			(spec.Width != m_ViewportSize.x || spec.Height != m_ViewportSize.y))
		{
			m_Framebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_EditorCamera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
			m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		}

		Renderer2D::ResetStats();
		m_Framebuffer->Bind();
		
		// Clears
		m_Framebuffer->ClearAttachmentInt(1, -1);

		switch (m_SceneState)
		{
			case SceneState::Edit:
			{
				RenderCommand::SetClearColor(m_EditorCamera.GetBackgroundColor());
				RenderCommand::Clear();
				m_ActiveScene->OnUpdateEditor(deltaTime, m_EditorCamera);
				m_EditorCamera.OnUpdate(deltaTime); // These are camera specific update commands. Actual rendering is in scene object.
				break;
			}
			case SceneState::Pause:
			{
				break;
			}
			case SceneState::Play:
			{
				m_ActiveScene->OnUpdateRuntime(deltaTime);
				break;
			}
		}

		// Read pixel
		auto [mx, my] = ImGui::GetMousePos();
		mx -= m_ViewportBounds[0].x;
		my -= m_ViewportBounds[0].y;
		glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];
		my = viewportSize.y - my;
		int mouseX = (int)mx;
		int mouseY = (int)my;

		m_HoveredEntity = {};
		if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)viewportSize.x && mouseY < (int)viewportSize.y)
		{
			int pixelData = m_Framebuffer->ReadPixel(1, mouseX, mouseY); // TODO: This is really slow??
			m_HoveredEntity = pixelData == -1 ? Entity() : Entity((entt::entity)pixelData, m_ActiveScene.get());
		}
		m_SelectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
		m_PropertiesPanel.SetSelectedEntity(m_SelectedEntity);

		if (m_SelectedEntity == Entity::Null)
			ImGuizmo::Enable(false);
		else
			ImGuizmo::Enable(true);
		
		m_Framebuffer->Unbind();

		Renderer2D::StatsEndFrame();
	}

	void LocusEditorLayer::OnEvent(Event& e)
	{
		if (m_ViewportHovered)
			m_EditorCamera.OnEvent(e);

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<KeyPressedEvent>(LOCUS_BIND_EVENT_FN(LocusEditorLayer::OnKeyPressed));
		dispatcher.Dispatch<MouseButtonPressedEvent>(LOCUS_BIND_EVENT_FN(LocusEditorLayer::OnMouseButtonPressed));
	}

	void LocusEditorLayer::OnImGuiRender()
	{
		LOCUS_PROFILE_FUNCTION();

		// --- Dockspace ------------------------------------------------------
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoScrollbar |
			ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoScrollWithMouse;
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);

		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		ImGui::PushStyleColor(ImGuiCol_WindowBg, LocusColors::DarkGrey);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 13.0f, 13.0f });
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::Begin("Layout", false, window_flags);
		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor();

		ImGuiStyle& style = ImGui::GetStyle();
		ImGuiIO& io = ImGui::GetIO();

		DrawLayoutTable();

		//ImGui::ShowDemoWindow();

		// --- Menu Bar -------------------------------------------------------
		DrawToolbar();


		// --- Viewport window ------------------------------------------------
		ImGuiWindowFlags viewportFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoScrollbar;
		if (!Application::Get().GetIsSavedStatus())
			viewportFlags |= ImGuiWindowFlags_UnsavedDocument;
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });
		ImGui::Begin(" Viewport ", false, viewportFlags);
		ImGui::PopStyleVar();
		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };

		uint32_t textureID = m_Framebuffer->GetColorAttachmentRendererID();
		ImGui::Image((void*)(uintptr_t)textureID, ImVec2{ m_ViewportSize.x, m_ViewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

		m_ViewportBounds[0].x = ImGui::GetItemRectMin().x, m_ViewportBounds[0].y = ImGui::GetItemRectMin().y;
		m_ViewportBounds[1].x = ImGui::GetItemRectMax().x, m_ViewportBounds[1].y = ImGui::GetItemRectMax().y;
		m_ViewportHovered = ImGui::IsMouseHoveringRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
		m_ViewportFocused = ImGui::IsWindowFocused();

		ProcessViewportDragDrop();
		// viewport gizmo
		Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
		if (selectedEntity && m_GizmoType != -1)
			showGizmoUI();
		// viewport menu
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::MenuItem("P", "q"))
				m_GizmoType = -1;
			if (ImGui::MenuItem("T", "w"))
				m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
			if (ImGui::MenuItem("R", "e"))
				m_GizmoType = ImGuizmo::OPERATION::ROTATE;
			if (ImGui::MenuItem("S", "r"))
				m_GizmoType = ImGuizmo::OPERATION::SCALE;

			ImGui::EndMenuBar();
		}
		ImGui::End();


		// --- Content Browser ------------------------------------------------
		ImGui::SetNextWindowSize({ m_FrameSizes[1].x, m_FrameSizes[1].y });
		ImGui::SetNextWindowPos({ m_FramePositions[1].x, m_FramePositions[1].y });
		m_ContentBrowserPanel.OnImGuiRender();


		// --- Scene Hierarchy ------------------------------------------------
		ImGui::SetNextWindowSize({ m_FrameSizes[2].x, m_FrameSizes[2].y });
		ImGui::SetNextWindowPos({ m_FramePositions[2].x, m_FramePositions[2].y });
		m_SceneHierarchyPanel.OnImGuiRender();


		// --- Properties -----------------------------------------------------
		ImGui::SetNextWindowSize({ m_FrameSizes[3].x, m_FrameSizes[3].y });
		ImGui::SetNextWindowPos({ m_FramePositions[3].x, m_FramePositions[3].y });
		m_PropertiesPanel.OnImGuiRender();


		// --- Debug panel ---------------------------------------------------
		DrawDebugPanel();
	

		// --- Save Project Popup ---------------------------------------------
		if (Application::Get().GetSaveChangesPopupStatus())
			ImGui::OpenPopup("Save?");
		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
		if (ImGui::BeginPopupModal("Save?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("Save Current Project?");
			if (ImGui::Button("Save")) // TODO: Keep on popup modal when clicking cancel
			{
				SaveScene();
				Application::Get().Close();
			}
			if (ImGui::Button("Don't Save"))
			{
				Application::Get().Close();
			}
			if (ImGui::Button("Close"))
			{
				Application::Get().SetSaveChangesPopupStatus(false);
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}

		ImGui::End(); // End ImGui
	}

	bool LocusEditorLayer::OnKeyPressed(KeyPressedEvent& e)
	{

		bool control = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
		bool shift = Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift);
		switch (e.GetKeyCode())
		{
			// Scene
			case Key::N:
			{
				if (control)
					NewScene();
				break;
			}
			case Key::O:
			{
				if (control)
					OpenScene();
				break;
			}
			case Key::S:
			{
				if (control && shift)
					SaveSceneAs();
				else if (control)
					SaveScene();
				break;
			}

			case Key::C:
			{
				if (control)
				{
					if (m_SelectedEntity)
						m_ClipboardEntity = m_SelectedEntity;
				}
				break;
			}

			case Key::V:
			{
				if (control)
				{
					if (m_ClipboardEntity)
					{
						std::string& tag = m_ClipboardEntity.GetComponent<TagComponent>().Tag;
						CommandHistory::AddCommand(new DuplicateEntityCommand(m_ActiveScene, tag, m_ClipboardEntity));
					}
				}
				break;
			}

			case Key::D:
			{
				if (control)
				{
					if (m_SelectedEntity)
					{
						std::string& tag = m_SelectedEntity.GetComponent<TagComponent>().Tag;
						CommandHistory::AddCommand(new DuplicateEntityCommand(m_ActiveScene, tag, m_SelectedEntity));
					}
				}
				break;
			}

			case Key::Delete:
			{
				if (m_SelectedEntity)
					CommandHistory::AddCommand(new DestroyEntityCommand(m_ActiveScene, m_SelectedEntity));
				break;
			}

			// Command History
			case Key::Y:
			{
				if (control)
					CommandHistory::Redo();
				break;
			}
			case Key::Z:
			{
				if (control)
					CommandHistory::Undo();
				break;
			}

			// Gizmos
			case Key::Q:
				m_GizmoType = -1;
				break;
			case Key::W:
				m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
				break;
			case Key::E:
				m_GizmoType = ImGuizmo::OPERATION::ROTATE;
				break;
			case Key::R:
				m_GizmoType = ImGuizmo::OPERATION::SCALE;
				break;
		}

		return true;
	}

	bool LocusEditorLayer::OnMouseButtonPressed(MouseButtonPressedEvent& e)
	{
		if (e.GetMouseButton() == Mouse::ButtonLeft)
		{
			if (m_ViewportHovered && !ImGuizmo::IsOver())
				m_SceneHierarchyPanel.SetSelectedEntity(m_HoveredEntity);
		}

		return false;
	}

	void LocusEditorLayer::NewScene()
	{
		m_EditorScene = CreateRef<Scene>();
		m_EditorScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		m_ActiveScene = m_EditorScene;
		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
		m_PropertiesPanel.SetContext(m_ActiveScene);
		m_SavePath = std::string();
		Application::Get().SetIsSavedStatus(false);
		CommandHistory::Reset();
	}

	void LocusEditorLayer::OpenScene()
	{
		std::string path = FileDialogs::OpenFile("Locus Scene (*.locus)\0*.locus\0");
		if (!path.empty())
			OpenScene(path);
	}

	void LocusEditorLayer::OpenScene(const std::filesystem::path& path)
	{
		m_EditorScene = CreateRef<Scene>();
		m_EditorScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		m_ActiveScene = m_EditorScene;
		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
		m_PropertiesPanel.SetContext(m_ActiveScene);
		SceneSerializer serializer(m_EditorScene);
		serializer.Deserialize(path.string());
		m_SavePath = path.string();
		Application::Get().SetIsSavedStatus(true);
		CommandHistory::Reset();
	}

	void LocusEditorLayer::SaveSceneAs()
	{
		std::string path = FileDialogs::SaveFile("Locus Scene (*.locus)\0*.locus\0");
		if (!path.empty())
		{
			SceneSerializer serializer(m_ActiveScene);
			serializer.Serialize(path);
			m_SavePath = path;
			Application::Get().SetIsSavedStatus(true);
		}
	}

	void LocusEditorLayer::SaveScene()
	{
		if (m_SavePath.empty())
		{
			SaveSceneAs();
		}
		else
		{
			SceneSerializer serializer(m_ActiveScene);
			serializer.Serialize(m_SavePath);
			Application::Get().SetIsSavedStatus(true);
		}
	}

	void LocusEditorLayer::showGizmoUI()
	{
		ImGuizmo::SetOrthographic(false);
		ImGuizmo::SetDrawlist();
		ImGuizmo::SetRect(ImGui::GetItemRectMin().x, ImGui::GetItemRectMin().y, ImGui::GetItemRectSize().x, ImGui::GetItemRectSize().y);
		ImGuizmo::AllowAxisFlip(false);

		// Camera
		const glm::mat4& cameraProjection = m_EditorCamera.GetProjection();
		glm::mat4 cameraView = m_EditorCamera.GetViewMatrix();

		// Entity transform
		Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
		auto& tc = selectedEntity.GetComponent<TransformComponent>();
		glm::mat4& transform = tc.GetTransform();

		// Snapping
		bool snap = Input::IsKeyPressed(Key::LeftControl);
		float snapValue = 0.5f; // Snap 0.5m for translation & scale
		if (m_GizmoType == ImGuizmo::OPERATION::ROTATE)
			snapValue = 45.0f; // Snap to 45 degrees for rotation
		float snapValues[3] = { snapValue, snapValue, snapValue };

		ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
			(ImGuizmo::OPERATION)m_GizmoType, ImGuizmo::LOCAL, glm::value_ptr(transform),
			nullptr, snap ? snapValues : nullptr);

		glm::vec3 translation, scale;
		glm::quat rotation;
		//Math::DecomposeTransform(transform, scale, rotation, translation);
		glm::decompose(transform, scale, rotation, translation, glm::vec3(1.0f), glm::vec4(1.0f));

		if (ImGuizmo::IsUsing())
		{
			switch (m_GizmoType)
			{
				case ImGuizmo::TRANSLATE:
				{
					CommandHistory::AddCommand(new ChangeValueCommand(translation, tc.Translation));
					break;
				}
				case ImGuizmo::ROTATE:
				{
					// Do this in Euler in an attempt to preserve any full revolutions (> 360)
					glm::vec3 originalRotationEuler = tc.GetRotationEuler();

					// Map original rotation to range [-180, 180] which is what ImGuizmo gives us
					originalRotationEuler.x = fmodf(originalRotationEuler.x + glm::pi<float>(), glm::two_pi<float>()) - glm::pi<float>();
					originalRotationEuler.y = fmodf(originalRotationEuler.y + glm::pi<float>(), glm::two_pi<float>()) - glm::pi<float>();
					originalRotationEuler.z = fmodf(originalRotationEuler.z + glm::pi<float>(), glm::two_pi<float>()) - glm::pi<float>();

					glm::vec3 deltaRotationEuler = glm::eulerAngles(rotation) - originalRotationEuler;

					// Try to avoid drift due numeric precision
					if (fabs(deltaRotationEuler.x) < 0.001) deltaRotationEuler.x = 0.0f;
					if (fabs(deltaRotationEuler.y) < 0.001) deltaRotationEuler.y = 0.0f;
					if (fabs(deltaRotationEuler.z) < 0.001) deltaRotationEuler.z = 0.0f;

					glm::vec3 rotationEuler = tc.GetRotationEuler();
					CommandHistory::AddCommand(new ChangeValueCommand(rotationEuler + deltaRotationEuler, tc.GetRotationEuler()));
					break;
				}
				case ImGuizmo::SCALE:
				{
					CommandHistory::AddCommand(new ChangeValueCommand(scale, tc.Scale));
					break;
				}
			}
		}
	}

	void LocusEditorLayer::ProcessViewportDragDrop()
	{
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SCENE_ITEM_PATH"))
			{
				if (m_SceneState == SceneState::Play)
					OnSceneStop();
				const wchar_t* path = (const wchar_t*)payload->Data;
				OpenScene(std::filesystem::path(g_ProjectPath) / path);
			}
			ImGui::EndDragDropTarget();
		}
	}

	void LocusEditorLayer::OnScenePlay()
	{
		m_SceneState = SceneState::Play;
		m_ActiveScene = Scene::Copy(m_EditorScene);
		m_ActiveScene->OnRuntimeStart();
		m_SceneHierarchyPanel.SetContext(m_ActiveScene);

	}

	void LocusEditorLayer::OnSceneStop()
	{
		m_SceneState = SceneState::Edit;
		m_ActiveScene->OnRuntimeStop();

		m_ActiveScene = m_EditorScene;
		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
	}

	void LocusEditorLayer::OnScenePause()
	{
		m_SceneState = SceneState::Pause;
	}

	void LocusEditorLayer::DrawLayoutTable()
	{
		ImGuiIO io = ImGui::GetIO();

		// Docked window flags
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 5.0f, 5.0f });
		ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, { 0.0f, 0.0f });
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 0.0f, 4.0f });
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

		ImGuiWindowFlags childFlags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
		// --- Left Table ---
		if (ImGui::BeginTable("LayoutLeft", 1, ImGuiTableFlags_None, { m_CenterSplitterPos, -1.0f}))
		{
			ImGui::TableNextColumn();
			ImGui::BeginChild("Frame1", { -1.0f, m_ViewportHeight }, false, childFlags);
			ImGuiID dockspace_id = ImGui::GetID("Frame1Dockspace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

			m_FrameSizes[0].x = ImGui::GetWindowSize().x;
			m_FrameSizes[0].y = ImGui::GetWindowSize().y;
			m_FramePositions[0].x = ImGui::GetWindowPos().x;
			m_FramePositions[0].y = ImGui::GetWindowPos().y;

			ImGuiID frame1DockID = ImGui::GetID("Frame1Dock");
			ImGui::DockSpace(frame1DockID, { -1.0f, -1.0f });

			ImGui::EndChild();

			ImGui::Button("##Divider", { 80.0f, 3.0f });
			if (ImGui::IsItemHovered())
				ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNS);
			if (ImGui::IsItemActive())
			{
				m_ViewportHeight += io.MouseDelta.y;
			}

			ImGui::BeginChild("Frame2", { -1.0f, -1.0f }, false, childFlags);

			m_FrameSizes[1].x = ImGui::GetWindowSize().x;
			m_FrameSizes[1].y = ImGui::GetWindowSize().y;
			m_FramePositions[1].x = ImGui::GetWindowPos().x;
			m_FramePositions[1].y = ImGui::GetWindowPos().y;

			ImGuiID frame2DockID = ImGui::GetID("Frame2Dock");
			ImGui::DockSpace(frame2DockID, { -1.0f, -1.0f });

			ImGui::EndChild();

			ImGui::EndTable();
		}

		// --- Center Splitter ---
		ImGui::SameLine();
		ImGui::Button("##CenterSplitter", { 3.0f, 80.0f });
		if (ImGui::IsItemHovered())
			ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
		if (ImGui::IsItemActive())
		{
			m_CenterSplitterPos += io.MouseDelta.x;
		}
		ImGui::SameLine();

		// --- Right Table ---
		if (ImGui::BeginTable("LayoutRight", 1, ImGuiTableFlags_None, { -1.0, -1.0f }))
		{
			ImGui::TableNextColumn();
			ImGui::BeginChild("Frame3", { -1.0f, m_HierarchyHeight }, false, childFlags);

			m_FrameSizes[2].x = ImGui::GetWindowSize().x;
			m_FrameSizes[2].y = ImGui::GetWindowSize().y;
			m_FramePositions[2].x = ImGui::GetWindowPos().x;
			m_FramePositions[2].y = ImGui::GetWindowPos().y;

			ImGuiID frame3DockID = ImGui::GetID("Frame3Dock");
			ImGui::DockSpace(frame3DockID, { -1.0f, -1.0f });

			ImGui::EndChild();

			ImGui::Button("##Divider", { 80.0f, 3.0f });
			if (ImGui::IsItemHovered())
				ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNS);
			if (ImGui::IsItemActive())
			{
				m_HierarchyHeight += io.MouseDelta.y;
			}

			ImGui::BeginChild("Frame4", { -1.0f, -1.0f }, false, childFlags);

			m_FrameSizes[3].x = ImGui::GetWindowSize().x;
			m_FrameSizes[3].y = ImGui::GetWindowSize().y;
			m_FramePositions[3].x = ImGui::GetWindowPos().x;
			m_FramePositions[3].y = ImGui::GetWindowPos().y;

			ImGuiID frame4DockID = ImGui::GetID("Frame4Dock");
			ImGui::DockSpace(frame4DockID, { -1.0f, -1.0f });

			ImGui::EndChild();

			ImGui::EndTable();
		}
		ImGui::PopStyleVar(5);
	}

	void LocusEditorLayer::DrawToolbar()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { -1.0f, 10.0f });
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 12.0f, -1.0f });
		ImGui::PushStyleColor(ImGuiCol_MenuBarBg, LocusColors::DarkGrey);
		ImGui::PushStyleColor(ImGuiCol_Header, LocusColors::DarkGrey);
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, LocusColors::Grey);
		ImGui::PushStyleColor(ImGuiCol_HeaderActive, LocusColors::Grey);
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New", "Ctrl+N"))
					NewScene();

				if (ImGui::MenuItem("Open...", "Ctrl+O"))
					OpenScene();

				if (ImGui::MenuItem("Save", "Ctrl+S"))
					SaveScene();

				if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S"))
					SaveSceneAs();

				if (ImGui::MenuItem("Exit"))
				{
					if (Application::Get().GetIsSavedStatus())
						Application::Get().Close();
					else
						Application::Get().SetSaveChangesPopupStatus(true);
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Project"))
			{
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Help"))
			{
				ImGui::EndMenu();
			}

			float avail = ImGui::GetWindowSize().x;

			// --- Runtime buttons --------------------------------------------
			// Background
			ImDrawList* draw_list = ImGui::GetWindowDrawList();
			ImVec2 bgSize = { 100.0f, 22.0f };
			float bgOffset = (avail - bgSize.x) * 0.5f;
			float menuPadding = 10.0f;
			ImVec2 bgTopLeft = { bgOffset, ImGui::GetWindowPos().y + menuPadding };
			ImVec2 bgBottomRight = { bgOffset + bgSize.x, ImGui::GetWindowPos().y + menuPadding + bgSize.y };
			draw_list->AddRectFilled(bgTopLeft, bgBottomRight, ImColor(LocusColors::LightGrey), 3.0f, ImDrawFlags_None);

			// Play button
			float buttonSize = 20.0f;
			float playOffset = ((avail - buttonSize) * 0.5f) - (bgSize.x * 0.33f) + 3.0f;
			ImGui::SetCursorPosX(playOffset);
			ImGui::SetCursorPosY(11.0f);
			ImGui::PushStyleColor(ImGuiCol_Button, LocusColors::Transparent);
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, LocusColors::Transparent);
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, LocusColors::Transparent);
			// TODO: Fix texture resolution causing jagged edges
			if (ImGui::ImageButton((ImTextureID)(uint64_t)m_PlayButton->GetRendererID(), ImVec2(buttonSize, buttonSize), ImVec2(0, 0), ImVec2(1, 1), 0))
			{
				if (m_SceneState == SceneState::Edit)
					OnScenePlay();
			}

			// Pause button
			float pauseOffset = ((avail - buttonSize) * 0.5f);
			ImGui::SetCursorPosX(pauseOffset);
			ImGui::SetCursorPosY(11.0f);
			if (ImGui::ImageButton((ImTextureID)(uint64_t)m_PauseButton->GetRendererID(), ImVec2(buttonSize, buttonSize), ImVec2(0, 0), ImVec2(1, 1), 0))
			{
				if (m_SceneState == SceneState::Play)
					OnScenePause();
				else if (m_SceneState == SceneState::Pause)
					m_SceneState = SceneState::Play;
			}

			// Stop button
			float stopOffset = ((avail - buttonSize) * 0.5f + (bgSize.x * 0.33f));
			ImGui::SetCursorPosX(stopOffset);
			ImGui::SetCursorPosY(11.0f);
			if (ImGui::ImageButton((ImTextureID)(uint64_t)m_StopButton->GetRendererID(), ImVec2(buttonSize, buttonSize), ImVec2(0, 0), ImVec2(1, 1), 0))
			{
				if (m_SceneState == SceneState::Play || m_SceneState == SceneState::Pause)
					OnSceneStop();
			}

			ImGui::PopStyleColor(3);

			ImGui::EndMainMenuBar();
		}
		ImGui::PopStyleVar(3);
		ImGui::PopStyleColor(4);
	}

	void LocusEditorLayer::DrawDebugPanel()
	{
		ImGui::Begin(" Debug ", false);
		auto stats = Renderer2D::GetStats();
		ImGui::Text("Renderer2D Stats:");
		ImGui::Text("Draw Calls: %d", stats.DrawCalls);
		ImGui::Text("Quads: %d", stats.QuadCount);
		ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
		ImGui::Text("Indices: %d", stats.GetTotalIndexCount());
		ImGui::Text("Frame Time: %f", stats.FrameTime);
		ImGui::Text("FPS: %f", stats.FramesPerSecond);

		std::string name = "None";
		if (m_HoveredEntity)
			if (m_HoveredEntity.HasComponent<IDComponent>())
				name = m_HoveredEntity.GetComponent<TagComponent>().Tag;
		ImGui::Text("Hovered Entity: %s", name.c_str());

		std::string collisionLayer = "None";
		if (m_HoveredEntity)
			if (m_HoveredEntity.HasComponent<BoxCollider2DComponent>())
				collisionLayer = std::to_string(m_HoveredEntity.GetComponent<BoxCollider2DComponent>().CollisionLayer);
		ImGui::Text("Hovered Collision Layer: %s", collisionLayer.c_str());

		// Relationships debug
		if (m_SelectedEntity)
		{
			if (m_SelectedEntity.HasComponent<RelationshipComponent>())
			{
				ImGui::Separator();
				ImGui::Text("Relationships");
				auto& rc = m_SelectedEntity.GetComponent<RelationshipComponent>();
				ImGui::Text("Children Count: %d", rc.ChildrenCount);
				Entity firstChild;
				Entity parent;
				Entity next;
				Entity prev;
				if (rc.FirstChild != entt::null)
				{
					firstChild = Entity(rc.FirstChild, m_ActiveScene.get());
					ImGui::Text("First Child: %s", firstChild.GetComponent<TagComponent>().Tag.c_str());
				}
				if (rc.Parent != entt::null)
				{
					parent = Entity(rc.Parent, m_ActiveScene.get());
					ImGui::Text("Parent: %s", parent.GetComponent<TagComponent>().Tag.c_str());
				}
				if (rc.Next != entt::null)
				{
					next = Entity(rc.Next, m_ActiveScene.get());
					ImGui::Text("Next: % s", next.GetComponent<TagComponent>().Tag.c_str());
				}
				if (rc.Prev != entt::null)
				{
					prev = Entity(rc.Prev, m_ActiveScene.get());
					ImGui::Text("Prev: %s", prev.GetComponent<TagComponent>().Tag.c_str());
				}
			}
		}
		ImGui::End();
	}
}
