#include "LocusEditorLayer.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>
#include <ImGuizmo.h>

#include "Widgets/Widgets.h"

namespace Locus
{
	extern const std::filesystem::path g_ProjectPath;

	LocusEditorLayer::LocusEditorLayer()
		: Layer("LocusEditorLayer")
	{
		m_PlayIcon = Texture2D::Create("resources/icons/PlayIcon.png");
		m_StopIcon = Texture2D::Create("resources/icons/StopIcon.png");
		m_PauseIcon = Texture2D::Create("resources/icons/PauseIcon.png");
		m_PhysicsIcon = Texture2D::Create("resources/icons/SimulatePhysicsIcon.png");
		m_PointerIcon = Texture2D::Create("resources/icons/PointerIcon.png");
		m_TranslateIcon = Texture2D::Create("resources/icons/TranslateIcon.png");
		m_ScaleIcon = Texture2D::Create("resources/icons/ScaleIcon.png");
		m_RotateIcon = Texture2D::Create("resources/icons/RotateIcon.png");

		CommandHistory::Init(this);
	}

	LocusEditorLayer::~LocusEditorLayer()
	{
		CommandHistory::Shutdown();
	}

	void LocusEditorLayer::OnAttach()
	{
		LOCUS_PROFILE_FUNCTION();

		// Framebuffer
		FramebufferSpecification framebufferSpecs;
		framebufferSpecs.Attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INT, FramebufferTextureFormat::Depth };
		framebufferSpecs.Width = 1920;
		framebufferSpecs.Height = 1080;
		m_Framebuffer = Framebuffer::Create(framebufferSpecs);

		FramebufferSpecification activeCameraFramebufferSpecs;
		activeCameraFramebufferSpecs.Attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::Depth };
		activeCameraFramebufferSpecs.Width = 640;
		activeCameraFramebufferSpecs.Height = 360;
		m_ActiveCameraFramebuffer = Framebuffer::Create(activeCameraFramebufferSpecs);

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
			LOCUS_CORE_INFO("Loaded startup scene: {0}", commandLineArgs[1]);
		}

		// Panels
		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
		m_PropertiesPanel.SetContext(m_ActiveScene);

		// Editor Camera
		m_EditorCamera = EditorCamera(30.0f, 1920.0f / 1080.0f, 0.1f, 10000.0f);

		m_WindowSize = { Application::Get().GetWindow().GetWidth(), Application::Get().GetWindow().GetHeight() };

		m_CollisionMeshColor = ToGLMVec4(LocusColors::LightBlue);
		m_FocusOutlineColor = ToGLMVec4(LocusColors::Green);
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

		// On viewport resize
		if (FramebufferSpecification spec = m_Framebuffer->GetSpecification();
			m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f &&
			(spec.Width != m_ViewportSize.x || spec.Height != m_ViewportSize.y))
		{
			m_Framebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_EditorCamera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
			m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_ActiveCameraViewportSize = m_ViewportSize * 0.2f;
		}

		// On window resize
		if (m_WindowSize.x != Application::Get().GetWindow().GetWidth() || 
			m_WindowSize.y != Application::Get().GetWindow().GetHeight())
		{
			m_WindowSize = { Application::Get().GetWindow().GetWidth(), Application::Get().GetWindow().GetHeight() };
		}

		Renderer2D::ResetStats();
		m_Framebuffer->Bind();
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
			case SceneState::Physics:
			{
				RenderCommand::SetClearColor(m_EditorCamera.GetBackgroundColor());
				RenderCommand::Clear();
				m_ActiveScene->OnUpdatePhysics(deltaTime, m_EditorCamera);
				m_EditorCamera.OnUpdate(deltaTime); // These are camera specific update commands. Actual rendering is in scene object.
				break;
			}
			case SceneState::PhysicsPause:
			{
				break;
			}
		}

		// Read pixel
		auto [mx, my] = ImGui::GetMousePos();
		mx -= m_ViewportBounds[0].x;
		my -= m_ViewportBounds[0].y;
		my = m_ViewportSize.y - my;
		int mouseX = (int)mx;
		int mouseY = (int)my;

		// Set hovered and selected entity
		m_HoveredEntity = {};
		if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)m_ViewportSize.x && mouseY < (int)m_ViewportSize.y)
		{
			int pixelData = m_Framebuffer->ReadPixel(1, mouseX, mouseY); // TODO: This is really slow??
			m_HoveredEntity = pixelData == -1 ? Entity() : Entity((entt::entity)pixelData, m_ActiveScene.get());
		}
		m_SelectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
		if (!m_SelectedEntity.IsValid())
			m_SelectedEntity = {};
		m_PropertiesPanel.SetSelectedEntity(m_SelectedEntity);

		// Gizmo visibility
		if (m_SelectedEntity.IsValid())
			m_GizmoVisible = true;
		else
			m_GizmoVisible = false;
		if (m_GizmoType == -1)
			m_GizmoVisible = false;

		// Disable keyboard capture when running scene
		if (m_SceneState == SceneState::Play && m_ViewportFocused)
			m_BlockEditorKeyInput = true;
		else
			m_BlockEditorKeyInput = false;

		OnRenderOverlay();

		m_Framebuffer->Unbind();

		// Make sure this is after unbinding main framebuffer as it uses a separate framebuffer.
		DrawActiveCameraView();

		Input::ProcessKeys();

		Renderer2D::StatsEndFrame();
	}

	void LocusEditorLayer::OnEvent(Event& e)
	{
		if (m_ViewportHovered)
			m_EditorCamera.OnEvent(e);

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(LOCUS_BIND_EVENT_FN(LocusEditorLayer::OnWindowClose));
		dispatcher.Dispatch<KeyPressedEvent>(LOCUS_BIND_EVENT_FN(LocusEditorLayer::OnKeyPressed));
		dispatcher.Dispatch<MouseButtonPressedEvent>(LOCUS_BIND_EVENT_FN(LocusEditorLayer::OnMouseButtonPressed));
	}

	void LocusEditorLayer::OnImGuiRender()
	{
		LOCUS_PROFILE_FUNCTION();

		if (m_LayoutStyle == LayoutStyle::Default)
			DrawDefaultLayout();
	}

	void LocusEditorLayer::OnRenderOverlay()
	{
		if (m_SceneState == SceneState::Edit)
		{
			Renderer2D::BeginScene(m_EditorCamera);
		}
		else if (m_SceneState == SceneState::Play)
		{
			Entity primaryCamera = m_ActiveScene->GetPrimaryCameraEntity();
			if (primaryCamera)
			{
				glm::mat4 transform = m_ActiveScene->GetWorldTransform(primaryCamera);
				Renderer2D::BeginScene(primaryCamera.GetComponent<CameraComponent>().Camera, transform);
			}
		}


		if (m_SelectedEntity.IsValid())
		{
			// Display focus outline
			auto& tc = m_SelectedEntity.GetComponent<TransformComponent>();
			glm::mat4 transform = m_ActiveScene->GetWorldTransform(m_SelectedEntity);
			transform *= glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0.001f));
			if (m_SelectedEntity.HasComponent<SpriteRendererComponent>())
			{
				Renderer2D::DrawRect(transform, m_FocusOutlineColor);
			}
			else if (m_SelectedEntity.HasComponent<CircleRendererComponent>())
			{
				Renderer2D::DrawDebugCircle(transform, m_FocusOutlineColor);
			}
		}

		DrawCollisionMesh();

		Renderer2D::EndScene();
	}

	bool LocusEditorLayer::OnWindowClose(WindowCloseEvent& e)
	{
		if (m_IsSaved)
			Application::Get().Close();
		else
			m_OpenSavePopup = true;
		e.m_Handled = true;

		return true;
	}

	bool LocusEditorLayer::OnKeyPressed(KeyPressedEvent& e)
	{
		if (m_BlockEditorKeyInput)
			return false;
		bool control = Input::IsKeyHeld(Key::LeftControl) || Input::IsKeyHeld(Key::RightControl);
		bool shift = Input::IsKeyHeld(Key::LeftShift) || Input::IsKeyHeld(Key::RightShift);
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
						CommandHistory::AddCommand(new DuplicateEntityCommand(m_ActiveScene, m_ClipboardEntity));
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
						CommandHistory::AddCommand(new DuplicateEntityCommand(m_ActiveScene, m_SelectedEntity));
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

			case Key::F:
			{
				if (m_SelectedEntity)
				{
					glm::mat4 transform = m_ActiveScene->GetWorldTransform(m_SelectedEntity);
					glm::vec3 position;
					Math::Decompose(transform, glm::vec3(), glm::quat(), position);
					m_EditorCamera.SetFocalPoint(position);
					m_EditorCamera.SetDistance(10.0f);
				}
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
			{
				if (control && shift)
				{
					ScriptEngine::ReloadScripts();
					m_PropertiesPanel.m_ScriptClasses = ScriptEngine::GetClassNames();
				}
				else
				{
					m_GizmoType = ImGuizmo::OPERATION::SCALE;
				}
				break;
			}
		}

		return true;
	}

	bool LocusEditorLayer::OnMouseButtonPressed(MouseButtonPressedEvent& e)
	{
		if (e.GetMouseButton() == Mouse::ButtonLeft)
		{
			if (m_ViewportHovered && (!ImGuizmo::IsOver() || ImGuizmo::IsOver() && !m_GizmoVisible))
			{
				m_SceneHierarchyPanel.SetSelectedEntity(m_HoveredEntity);
				m_SelectedEntity = m_HoveredEntity;
				m_GizmoFirstClick = true;
			}
		}
		return false;
	}

	void LocusEditorLayer::NewScene()
	{
		if (m_SceneState != SceneState::Edit)
			OnSceneStop();
		m_SelectedEntity = {};
		m_HoveredEntity = {};
		m_EditorScene = CreateRef<Scene>();
		m_EditorScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		m_ActiveScene = m_EditorScene;
		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
		m_PropertiesPanel.SetContext(m_ActiveScene);
		m_SavePath = std::string();
		m_IsSaved = false;
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
		if (m_SceneState != SceneState::Edit)
			OnSceneStop();
		m_SelectedEntity = {};
		m_HoveredEntity = {};
		m_EditorScene = CreateRef<Scene>();
		m_EditorScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		m_ActiveScene = m_EditorScene;
		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
		m_PropertiesPanel.SetContext(m_ActiveScene);
		SceneSerializer serializer(m_EditorScene);
		serializer.Deserialize(path.string());
		m_SavePath = path.string();
		m_IsSaved = true;
		CommandHistory::Reset();
	}

	// Returns false if canceling file dialog.
	bool LocusEditorLayer::SaveSceneAs()
	{
		std::string path = FileDialogs::SaveFile("Locus Scene (*.locus)\0*.locus\0");
		if (!path.empty())
		{
			SceneSerializer serializer(m_EditorScene);
			serializer.Serialize(path);
			m_SavePath = path;
			m_IsSaved = true;
			return true;
		}
		return false;
	}

	bool LocusEditorLayer::SaveScene()
	{
		if (m_SavePath.empty())
		{
			return SaveSceneAs();
		}
		else
		{
			SceneSerializer serializer(m_EditorScene);
			serializer.Serialize(m_SavePath);
			m_IsSaved = true;
			return true;
		}
	}

	void LocusEditorLayer::showGizmoUI()
	{
		ImGuizmo::SetOrthographic(false);
		ImGuizmo::SetDrawlist();
		ImGuizmo::SetRect(m_ViewportBounds[0].x, m_ViewportBounds[0].y, m_ViewportSize.x, m_ViewportSize.y);
		ImGuizmo::AllowAxisFlip(false);

		// Camera
		const glm::mat4& cameraProjection = m_EditorCamera.GetProjection();
		glm::mat4 cameraView = m_EditorCamera.GetViewMatrix();

		// Entity transform
		auto& tc = m_SelectedEntity.GetComponent<TransformComponent>();
		glm::mat4& transform = m_ActiveScene->GetWorldTransform(m_SelectedEntity);

		// Snapping
		bool snap = Input::IsKeyPressed(Key::LeftControl);
		float snapValue = 0.5f; // Snap 0.5m for translation & scale
		if (m_GizmoType == ImGuizmo::OPERATION::ROTATE)
			snapValue = 45.0f; // Snap to 45 degrees for rotation
		float snapValues[3] = { snapValue, snapValue, snapValue };

		ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
			(ImGuizmo::OPERATION)m_GizmoType, ImGuizmo::LOCAL, glm::value_ptr(transform),
			nullptr, snap ? snapValues : nullptr);

		// Convert back to local space
		if (tc.Parent != Entity::Null)
			transform = glm::inverse(m_ActiveScene->GetWorldTransform(tc.Parent)) * transform;

		glm::vec3 translation, scale;
		glm::quat rotation;
		Math::Decompose(transform, scale, rotation, translation);

		if (ImGuizmo::IsUsing())
		{
			switch (m_GizmoType)
			{
				case ImGuizmo::TRANSLATE:
				{
					CommandHistory::AddCommand(new ChangeValueCommand(translation, tc.LocalPosition));
					break;
				}
				case ImGuizmo::ROTATE:
				{
					// Do this in Euler in an attempt to preserve any full revolutions (> 360)
					glm::vec3 originalRotationEuler = tc.GetLocalRotation();

					// Map original rotation to range [-180, 180] which is what ImGuizmo gives us
					originalRotationEuler.x = fmodf(originalRotationEuler.x + glm::pi<float>(), glm::two_pi<float>()) - glm::pi<float>();
					originalRotationEuler.y = fmodf(originalRotationEuler.y + glm::pi<float>(), glm::two_pi<float>()) - glm::pi<float>();
					originalRotationEuler.z = fmodf(originalRotationEuler.z + glm::pi<float>(), glm::two_pi<float>()) - glm::pi<float>();

					glm::vec3 deltaRotationEuler = glm::eulerAngles(rotation) - originalRotationEuler;

					// Try to avoid drift due numeric precision
					if (fabs(deltaRotationEuler.x) < 0.001) deltaRotationEuler.x = 0.0f;
					if (fabs(deltaRotationEuler.y) < 0.001) deltaRotationEuler.y = 0.0f;
					if (fabs(deltaRotationEuler.z) < 0.001) deltaRotationEuler.z = 0.0f;

					glm::vec3 rotationEuler = tc.GetLocalRotation();
					CommandHistory::AddCommand(new ChangeValueCommand(rotationEuler + deltaRotationEuler, tc.LocalRotation));
					break;
				}
				case ImGuizmo::SCALE:
				{
					CommandHistory::AddCommand(new ChangeValueCommand(scale, tc.LocalScale));
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
		ImGui::SetWindowFocus("Viewport");
	}

	void LocusEditorLayer::OnPhysicsPlay()
	{
		m_SceneState = SceneState::Physics;
		m_ActiveScene = Scene::Copy(m_EditorScene);
		m_ActiveScene->OnPhysicsStart();
		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
		ImGui::SetWindowFocus("Viewport");
	}

	void LocusEditorLayer::OnSceneStop()
	{
		m_SelectedEntity = {};
		m_SceneState = SceneState::Edit;
		m_ActiveScene->OnRuntimeStop();

		m_ActiveScene = m_EditorScene;
		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
	}

	void LocusEditorLayer::DrawDefaultLayout()
	{
		// --- Dockspace ---
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoScrollbar |
			ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoScrollWithMouse;
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);

		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 13.0f, 13.0f });
		ImGui::PushStyleColor(ImGuiCol_WindowBg, LocusColors::DarkGrey);
		ImGui::Begin("Layout", false, window_flags);
		ImGui::PopStyleVar(2);

		ImGuiDockNodeFlags docknodeFlags = ImGuiDockNodeFlags_None;
		//docknodeFlags |= ImGuiDockNodeFlags_TabBarCollapsed; // Behaves the same as showing tab bar
		docknodeFlags |= ImGuiDockNodeFlags_WindowRounding;
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::PushStyleColor(ImGuiCol_Separator, LocusColors::Transparent);
		ImGui::PushStyleColor(ImGuiCol_SeparatorHovered, LocusColors::Transparent);
		ImGui::PushStyleColor(ImGuiCol_SeparatorActive, LocusColors::Transparent);
		ImGui::DockSpace(dockspace_id, { 0.0f, 0.0f }, docknodeFlags);
		ImGui::PopStyleColor(4);


		// --- Panels ---
		DrawToolbar();
		DrawViewport();
		DrawDebugPanel();
		m_SceneHierarchyPanel.OnImGuiRender();
		m_PropertiesPanel.OnImGuiRender();
		m_ContentBrowserPanel.OnImGuiRender();
		ImGui::ShowDemoWindow();


		ProcessSavePopup();

		ImGui::End();
	}

	void LocusEditorLayer::DrawViewport()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });
		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_TabBarAlignLeft | ImGuiWindowFlags_DockedWindowBorder;
		if (!m_IsSaved)
			windowFlags |= ImGuiWindowFlags_UnsavedDocument;
		ImGui::Begin("Viewport", false, windowFlags);

		// --- Draw Viewport ---
		m_ViewportSize = { ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y };
		uint32_t textureID = m_Framebuffer->GetColorAttachmentRendererID();
		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		ImVec2 topLeft = { ImGui::GetCursorPosX() + ImGui::GetWindowPos().x,ImGui::GetCursorPosY() + ImGui::GetWindowPos().y };
		ImVec2 botRight = { topLeft.x + m_ViewportSize.x, topLeft.y + m_ViewportSize.y };
		draw_list->AddImageRounded((void*)(uintptr_t)textureID, topLeft, botRight, { 0, 1 }, { 1, 0 }, ImGui::GetColorU32(LocusColors::White), 10.0f, ImDrawFlags_RoundCornersBottom);
		
		m_ViewportBounds[0] = { topLeft.x, topLeft.y };
		m_ViewportBounds[1] = { botRight.x, botRight.y };
		m_ViewportHovered = ImGui::IsMouseHoveringRect({ topLeft.x, topLeft.y }, { topLeft.x + m_ViewportSize.x, topLeft.y + m_ViewportSize.y });
		m_ViewportFocused = ImGui::IsWindowFocused();

		ProcessViewportDragDrop();

		// --- Gizmo ---
		// Checks for first click to prevent moving the object when selecting an entity.
		if (m_SelectedEntity && m_GizmoType != -1)
		{
			if (!m_GizmoFirstClick)
				showGizmoUI();
			else
				m_GizmoFirstClick = false;
		}

		// --- Viewport Toolbar ---
		glm::vec2 toolbarPos = { 10.0f, 10.0f };
		DrawViewportToolbar(toolbarPos);

		// --- Active camera view ---
		if (m_SelectedEntity.IsValid() && m_SceneState == SceneState::Edit)
		{
			if (m_SelectedEntity.HasComponent<CameraComponent>())
			{
				// Draw view of camera if a camera component is selected
				float margin = 10.0f;
				ImVec2 viewTopLeft = { ImGui::GetWindowPos().x + m_ViewportSize.x - m_ActiveCameraViewportSize.x - margin,
					ImGui::GetWindowPos().y + m_ViewportSize.y - m_ActiveCameraViewportSize.y - margin + ImGui::CalcTextSize("A").y + ImGui::GetStyle().FramePadding.y * 2.0f };
				ImVec2 viewBotRight = { viewTopLeft.x + m_ActiveCameraViewportSize.x, viewTopLeft.y + m_ActiveCameraViewportSize.y };
				ImGui::SetCursorScreenPos(viewTopLeft);
				ImGui::BeginChild("ActiveCameraView", { m_ActiveCameraViewportSize.x, m_ActiveCameraViewportSize.y }, true);

				uint32_t texID = m_ActiveCameraFramebuffer->GetColorAttachmentRendererID();
				draw_list->AddImageRounded((void*)(uintptr_t)texID, viewTopLeft, viewBotRight, { 0, 1 }, { 1, 0 }, 
					ImGui::GetColorU32(LocusColors::White), ImGui::GetStyle().WindowRounding);

				ImGui::EndChild();
			}
		}

		ImGui::End();
		ImGui::PopStyleVar();
	}

	void LocusEditorLayer::DrawViewportToolbar(const glm::vec2& position)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, LocusColors::Transparent);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, LocusColors::Transparent);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, LocusColors::Transparent);
		ImGui::PushStyleColor(ImGuiCol_ChildBg, { 0.0f, 0.0f, 0.0f, 0.3f });
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0.0f, ImGui::GetStyle().ItemSpacing.y });
		
		float toolbarHeight = 35.0f;

		ImGui::SetCursorPos({ position.x, ImGui::GetCurrentWindow()->DockTabItemRect.GetHeight() + position.y });
		ImGui::BeginChild("Gizmo toolbar", { toolbarHeight * 4.0f, toolbarHeight }, true);

		int imagePadding = 5;
		float imageWidth = ImGui::GetContentRegionAvail().y - imagePadding * 2.0f;
		ImVec4 tintColor;

		// --- Gizmo buttons ---
		// Pointer
		tintColor = LocusColors::Orange;
		if (m_GizmoType == -1)
			tintColor = LocusColors::White;
		if (ImGui::ImageButton((ImTextureID)(uint64_t)m_PointerIcon->GetRendererID(), { imageWidth, imageWidth }, { 0, 1 }, { 1, 0 }, imagePadding, LocusColors::Transparent, tintColor))
			m_GizmoType = -1;

		ImGui::SameLine();

		// Translate
		tintColor = LocusColors::Orange;
		if (m_GizmoType == ImGuizmo::OPERATION::TRANSLATE)
			tintColor = LocusColors::White;
		if (ImGui::ImageButton((ImTextureID)(uint64_t)m_TranslateIcon->GetRendererID(), { imageWidth, imageWidth }, { 0, 1 }, { 1, 0 }, imagePadding, LocusColors::Transparent, tintColor))
			m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;

		ImGui::SameLine();

		// Rotate
		tintColor = LocusColors::Orange;
		if (m_GizmoType == ImGuizmo::OPERATION::ROTATE)
			tintColor = LocusColors::White;
		if (ImGui::ImageButton((ImTextureID)(uint64_t)m_RotateIcon->GetRendererID(), { imageWidth, imageWidth }, { 0, 1 }, { 1, 0 }, imagePadding, LocusColors::Transparent, tintColor))
			m_GizmoType = ImGuizmo::OPERATION::ROTATE;

		ImGui::SameLine();

		// Scale
		tintColor = LocusColors::Orange;
		if (m_GizmoType == ImGuizmo::OPERATION::SCALE)
			tintColor = LocusColors::White;
		if (ImGui::ImageButton((ImTextureID)(uint64_t)m_ScaleIcon->GetRendererID(), { imageWidth, imageWidth }, { 0, 1 }, { 1, 0 }, imagePadding, LocusColors::Transparent, tintColor))
			m_GizmoType = ImGuizmo::OPERATION::SCALE;

		ImGui::EndChild();

		if (ImGui::IsItemHovered())
			m_ViewportHovered = false;

		
		ImGui::SameLine(ImGui::GetItemRectSize().x + 20.0f);


		// --- Editor settings ---
		ImGui::BeginChild("Editor toolbar", { 55.0f, toolbarHeight }, true);

		if (ImGui::Button("View", { -1.0f, -1.0f }))
			ImGui::OpenPopup("ViewSettings");

		ProcessViewSettingsPopup();

		ImGui::EndChild();

		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor(4);
	}

	void LocusEditorLayer::DrawToolbar()
	{
		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_None;
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });
		ImGui::PushStyleColor(ImGuiCol_WindowBg, LocusColors::Transparent);
		ImGui::Begin("Toolbar", false, windowFlags);
		ImGui::GetCurrentWindow()->DockNode->LocalFlags |= ImGuiDockNodeFlags_TabBarCollapsed | ImGuiDockNodeFlags_NoResize;

		// --- Settings bar ---
		ImGui::SetCursorPos({ ImGui::GetCursorPosX() + 1.0f, ImGui::GetCursorPosY() + 1.0f });
		ImGui::BeginChild("SettingsBar", { 250.0f, -1.0f }, true, ImGuiWindowFlags_None);

		ImGui::PushStyleColor(ImGuiCol_Button, LocusColors::Transparent);
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0.0f, ImGui::GetStyle().ItemSpacing.y });
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);

		float settingsBarWidth = ImGui::GetWindowWidth();
		if (ImGui::Button("File", { settingsBarWidth / 3.0f, -1.0f }))
			ImGui::OpenPopup("File Settings");

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 6.0f, 6.0f });
		ImGui::SetNextWindowPos({ ImGui::GetCursorPos().x + ImGui::GetWindowPos().x, ImGui::GetCursorPos().y + ImGui::GetWindowPos().y - ImGui::GetStyle().ItemSpacing.y });
		if (ImGui::BeginPopup("File Settings"))
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
				if (m_IsSaved)
					Application::Get().Close();
				else
					m_OpenSavePopup = true;
			}
			ImGui::EndPopup();
		}
		ImGui::PopStyleVar();

		ImGui::SameLine();

		if (ImGui::Button("Project", { settingsBarWidth / 3.0f, -1.0f }))
		{
			ImGui::OpenPopup("Project Settings");
		}

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 6.0f, 6.0f });
		ImGui::SetNextWindowPos({ ImGui::GetCursorPos().x + ImGui::GetWindowPos().x + settingsBarWidth / 3.0f, 
			ImGui::GetCursorPos().y + ImGui::GetWindowPos().y - ImGui::GetStyle().ItemSpacing.y });
		if (ImGui::BeginPopup("Project Settings"))
		{
			if (ImGui::MenuItem("Reload Scripts", "Ctrl+Shift+R"))
			{
				ScriptEngine::ReloadScripts();
				m_PropertiesPanel.m_ScriptClasses = ScriptEngine::GetClassNames();
			}
			ImGui::EndPopup();
		}
		ImGui::PopStyleVar();

		ImGui::SameLine();

		if (ImGui::Button("Help", { settingsBarWidth / 3.0f, -1.0f }))
		{

		}

		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor();
		ImGui::EndChild();


		ImGui::SameLine(ImGui::GetContentRegionAvail().x - ImGui::GetWindowHeight() * 4.0f + 5.0f); // 5.0f is imagePadding


		// --- Runtime buttons ---
		ImGui::BeginChild("RuntimeButtons", { -1.0f, -1.0f }, true);
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0.0f, ImGui::GetStyle().ItemSpacing.y });
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
		ImGui::PushStyleColor(ImGuiCol_Button, LocusColors::Transparent);

		int imagePadding = 5;
		float imageWidth = ImGui::GetWindowHeight() - imagePadding * 2.0f;
		ImVec4 tintColor;

		// Play button
		tintColor = LocusColors::LightGrey;
		if (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Pause)
			tintColor = LocusColors::White;
		if (ImGui::ImageButton((ImTextureID)(uint64_t)m_PlayIcon->GetRendererID(), { imageWidth, imageWidth }, { 0, 1 }, { 1, 0 }, imagePadding, LocusColors::Transparent, tintColor))
		{
			if (m_SceneState == SceneState::Edit)
				OnScenePlay();
			else if (m_SceneState == SceneState::Pause)
				m_SceneState = SceneState::Play;
		}

		ImGui::SameLine();

		// Physics button
		tintColor = LocusColors::LightGrey;
		if (m_SceneState == SceneState::Edit || m_SceneState == SceneState::PhysicsPause)
			tintColor = LocusColors::White;
		if (ImGui::ImageButton((ImTextureID)(uint64_t)m_PhysicsIcon->GetRendererID(), { imageWidth, imageWidth }, { 0, 1 }, { 1, 0 }, imagePadding, LocusColors::Transparent, tintColor))
		{
			if (m_SceneState == SceneState::Edit)
				OnPhysicsPlay();
			else if (m_SceneState == SceneState::PhysicsPause)
				m_SceneState = SceneState::Physics;
		}

		ImGui::SameLine();

		// Pause button
		tintColor = LocusColors::LightGrey;
		if (m_SceneState == SceneState::Play || m_SceneState == SceneState::Physics)
			tintColor = LocusColors::White;
		if (ImGui::ImageButton((ImTextureID)(uint64_t)m_PauseIcon->GetRendererID(), { imageWidth, imageWidth }, { 0, 1 }, { 1, 0 }, imagePadding, LocusColors::Transparent, tintColor))
		{
			if (m_SceneState == SceneState::Play)
				m_SceneState = SceneState::Pause;
			else if (m_SceneState == SceneState::Physics)
				m_SceneState = SceneState::PhysicsPause;
		}

		ImGui::SameLine();

		// Stop button
		tintColor = LocusColors::LightGrey;
		if (m_SceneState != SceneState::Edit)
			tintColor = LocusColors::White;
		if (ImGui::ImageButton((ImTextureID)(uint64_t)m_StopIcon->GetRendererID(), { imageWidth, imageWidth }, { 0, 1 }, { 1, 0 }, imagePadding, LocusColors::Transparent, tintColor))
		{
			if (m_SceneState != SceneState::Edit)
				OnSceneStop();
		}

		ImGui::PopStyleColor();
		ImGui::PopStyleVar(2);
		ImGui::EndChild();

		ImGui::End();
		ImGui::PopStyleColor();
		ImGui::PopStyleVar(2);
	}
	
	void LocusEditorLayer::ProcessSavePopup()
	{
		if (m_OpenSavePopup)
			ImGui::OpenPopup("You have unsaved changes...");

		ImVec2 center = { ImGui::GetWindowPos().x + m_WindowSize.x / 2, ImGui::GetWindowPos().y + m_WindowSize.y / 2};
		ImGui::SetNextWindowPos(center, ImGuiCond_None, { 0.5f, 0.5f });
		if (ImGui::BeginPopupModal("You have unsaved changes...", &m_OpenSavePopup, ImGuiWindowFlags_AlwaysAutoResize))
		{
			m_SelectedEntity = {};

			ImGui::Text("Save Current Project?");

			if (ImGui::Button("Save and Close", { 140.0f, 40.0f }))
			{
				if (SaveScene())
					Application::Get().Close();
			}

			ImGui::SameLine();

			if (ImGui::Button("Don't Save", { 140.0f, 40.0f }))
			{
				Application::Get().Close();
			}

			ImGui::SameLine();

			if (ImGui::Button("Cancel", { 140.0f, 40.0f }))
			{
				m_OpenSavePopup = false;
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
	}

	void LocusEditorLayer::ProcessViewSettingsPopup()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 10.0f, 10.0f });

		ImGui::SetNextWindowPos({ ImGui::GetWindowPos().x, ImGui::GetWindowPos().y + ImGui::GetWindowSize().y});
		if (ImGui::BeginPopup("ViewSettings"))
		{
			float labelWidth = 150.0f;
			// Grid visibility
			Widgets::DrawControlLabel("Collision Mesh", { labelWidth, 0.0f });
			ImGui::SameLine();
			if (ImGui::IsItemHovered())
			{
				ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
				if (ImGui::IsMouseDoubleClicked(0))
					m_ShowAllCollisionMesh = false;
			}
			bool collisionMesh = m_ShowAllCollisionMesh;
			if (ImGui::Checkbox("##Collision Mesh", &collisionMesh))
				m_ShowAllCollisionMesh = !m_ShowAllCollisionMesh;

			// Background color
			Widgets::DrawControlLabel("Background Color", { labelWidth, 0.0f });
			ImGui::SameLine();
			if (ImGui::IsItemHovered())
			{
				ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
				if (ImGui::IsMouseDoubleClicked(0))
					m_EditorCamera.SetBackgroundColor({ 0.25f, 0.5f, 0.5f, 1.0f });
			}
			glm::vec4 bgColor = m_EditorCamera.GetBackgroundColor();
			if (ImGui::ColorEdit4("##Background Color", glm::value_ptr(bgColor)))
				m_EditorCamera.SetBackgroundColor(bgColor);

			// Grid visibility
			Widgets::DrawControlLabel("Grid Visibility", { labelWidth, 0.0f });
			ImGui::SameLine();
			if (ImGui::IsItemHovered())
			{
				ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
				if (ImGui::IsMouseDoubleClicked(0))
					m_EditorCamera.SetGridVisibility(true);
			}
			bool gridEnabled = m_EditorCamera.GetGridVisibility();
			if (ImGui::Checkbox("##Grid Visibility", &gridEnabled))
				m_EditorCamera.SetGridVisibility(gridEnabled);

			// Grid scale
			Widgets::DrawControlLabel("Grid Scale", { labelWidth, 0.0f });
			ImGui::SameLine();
			if (ImGui::IsItemHovered())
			{
				ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
				if (ImGui::IsMouseDoubleClicked(0))
					m_EditorCamera.SetGridScale(1.0f);
			}
			float gridScale = m_EditorCamera.GetGridScale();
			if (ImGui::DragFloat("##Grid Scale", &gridScale, 0.01f, 0.01f, FLT_MAX))
				m_EditorCamera.SetGridScale(gridScale);

			// Grid color
			Widgets::DrawControlLabel("Grid Color", { labelWidth, 0.0f });
			ImGui::SameLine();
			if (ImGui::IsItemHovered())
			{
				ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
				if (ImGui::IsMouseDoubleClicked(0))
					m_EditorCamera.SetGridColor({ 0.8f, 0.8f, 0.8f, 1.0f });
			}
			glm::vec4 gridColor = m_EditorCamera.GetGridColor();
			if (ImGui::ColorEdit4("##Grid Color", glm::value_ptr(gridColor)))
				m_EditorCamera.SetGridColor(gridColor);

			// Near Clip
			Widgets::DrawControlLabel("Near Clip", { labelWidth, 0.0f });
			ImGui::SameLine();
			if (ImGui::IsItemHovered())
			{
				ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
				if (ImGui::IsMouseDoubleClicked(0))
					m_EditorCamera.SetNearClip(0.1f);;
			}
			float nearClip = m_EditorCamera.GetNearClip();
			if (ImGui::DragFloat("##Near Clip", &nearClip, 0.01f, 0.0f, FLT_MAX))
				m_EditorCamera.SetNearClip(nearClip);

			// Far Clip
			Widgets::DrawControlLabel("Far Clip", { labelWidth, 0.0f });
			ImGui::SameLine();
			if (ImGui::IsItemHovered())
			{
				ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
				if (ImGui::IsMouseDoubleClicked(0))
					m_EditorCamera.SetFarClip(10000.0f);;
			}
			float farClip = m_EditorCamera.GetFarClip();
			if (ImGui::DragFloat("##Far Clip", &farClip, 5.0f, 0.0f, FLT_MAX))
				m_EditorCamera.SetFarClip(farClip);

			ImGui::EndPopup();
		}

		ImGui::PopStyleVar();
	}

	void LocusEditorLayer::DrawDebugPanel()
	{
		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_TabBarAlignLeft | ImGuiWindowFlags_DockedWindowBorder;
		ImGui::Begin("Debug", false, windowFlags);

		auto stats = Renderer2D::GetStats();
		ImGui::Text("Renderer2D Stats:");
		ImGui::Text("Draw Calls: %d", stats.DrawCalls);
		ImGui::Text("Quads: %d", stats.QuadCount);
		ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
		ImGui::Text("Indices: %d", stats.GetTotalIndexCount());
		ImGui::Text("Frame Time: %f", stats.FrameTime);
		ImGui::Text("FPS: %f", stats.FramesPerSecond);

		std::string name = "None";
		if (m_HoveredEntity.IsValid())
			if (m_HoveredEntity.HasComponent<IDComponent>())
				name = m_HoveredEntity.GetComponent<TagComponent>().Tag;
		ImGui::Text("Hovered Entity: %s", name.c_str());

		ImGui::Text("Entity Value: %d", (entt::entity)m_SelectedEntity);
		if (m_SelectedEntity.IsValid())
			ImGui::Text("Hierarchy position: %d", m_SelectedEntity.GetComponent<TagComponent>().HierarchyPos);

		// Collision
		if (m_SelectedEntity.IsValid())
		{
			if (m_SelectedEntity.HasComponent<BoxCollider2DComponent>())
				ImGui::Text("Collision category: %d", m_SelectedEntity.GetComponent<BoxCollider2DComponent>().CollisionCategory);
		}
		// Child debug
		if (m_SelectedEntity.IsValid())
		{
			if (m_SelectedEntity.HasComponent<ChildComponent>())
			{
				auto& cc = m_SelectedEntity.GetComponent<ChildComponent>();
				ImGui::Separator();
				ImGui::Text("Children:");
				ImGui::Indent();
				for (auto entity : cc.ChildEntities)
				{
					auto tag = entity.GetComponent<TagComponent>().Tag;
					ImGui::Text(tag.c_str());
				}
			}
		}

		// Transforms
		if (m_SelectedEntity.IsValid())
		{
			ImGui::Separator();
			ImGui::Text("Transforms");

			auto tc = m_SelectedEntity.GetComponent<TransformComponent>();
			if (tc.Parent != Entity::Null)
				ImGui::Text("Parent: %s", tc.Parent.GetComponent<TagComponent>().Tag.c_str());
			else
				ImGui::Text("Parent: Entity::Null");

			ImGui::Text("Self: %s", tc.Self.GetComponent<TagComponent>().Tag.c_str());

			glm::mat4 worldTransform = m_ActiveScene->GetWorldTransform(m_SelectedEntity);
			glm::vec3 worldPosition, worldScale;
			glm::quat worldRotationQuat;
			Math::Decompose(worldTransform, worldScale, worldRotationQuat, worldPosition);
			glm::vec3 worldRotation = glm::eulerAngles(worldRotationQuat);

			ImGui::Text("LocalPosition: %f, %f, %f", tc.LocalPosition.x, tc.LocalPosition.y, tc.LocalPosition.z);
			ImGui::Text("WorldPosition: %f, %f, %f", worldPosition.x, worldPosition.y, worldPosition.z);

			ImGui::Text("LocalRotation: %f, %f, %f", glm::degrees(tc.LocalRotation.x), glm::degrees(tc.LocalRotation.y), glm::degrees(tc.LocalRotation.z));
			ImGui::Text("WorldRotation: %f, %f, %f", glm::degrees(worldRotation.x), glm::degrees(worldRotation.y), glm::degrees(worldRotation.z));

			ImGui::Text("LocalScale: %f, %f, %f", tc.LocalScale.x, tc.LocalScale.y, tc.LocalScale.z);
			ImGui::Text("WorldScale: %f, %f, %f", worldScale.x, worldScale.y, worldScale.z);
		}

		ImGui::End();
	}

	void LocusEditorLayer::DrawCollisionMesh()
	{
		// Draw single collision mesh for selected entity
		if (m_SelectedEntity.IsValid() && !m_ShowAllCollisionMesh)
		{
			// Box collider
			if (m_SelectedEntity.HasComponent<BoxCollider2DComponent>())
			{
				auto& b2D = m_SelectedEntity.GetComponent<BoxCollider2DComponent>();
				auto& tc = m_SelectedEntity.GetComponent<TransformComponent>();

				// Combine the box collider offset and size to the transform
				glm::mat4 transform = tc.GetLocalTransform();
				transform *= glm::translate(glm::mat4(1.0f), { b2D.Offset.x, b2D.Offset.y, 0.001f })
					* glm::scale(glm::mat4(1.0f), { b2D.Size.x, b2D.Size.y, 1.0f });

				Renderer2D::DrawRect(transform, m_CollisionMeshColor);
			}
			// Circle collider
			else if (m_SelectedEntity.HasComponent<CircleCollider2DComponent>())
			{
				auto& c2D = m_SelectedEntity.GetComponent<CircleCollider2DComponent>();
				auto& tc = m_SelectedEntity.GetComponent<TransformComponent>();

				// Combine the box collider offset and size to the transform and
				// calculate the circle radius for the larger scale axis (x or y).
				float maxScale = tc.LocalScale.x > tc.LocalScale.y ? tc.LocalScale.x : tc.LocalScale.y;
				glm::mat4 transform = glm::translate(glm::mat4(1.0f), tc.LocalPosition)
					* glm::rotate(glm::mat4(1.0f), tc.LocalRotation.z, glm::vec3(0, 0, 1))
					* glm::scale(glm::mat4(1.0f), { maxScale, maxScale, 1.0f });
				transform *= glm::translate(glm::mat4(1.0f), { c2D.Offset.x, c2D.Offset.y, 0.001f })
					* glm::scale(glm::mat4(1.0f), { c2D.Radius * 2.0f, c2D.Radius * 2.0f, 1.0f });

				Renderer2D::DrawDebugCircle(transform, m_CollisionMeshColor);
			}
		}
		// Draw collision mesh for all entities in the scene
		else if (m_ShowAllCollisionMesh)
		{
			// Box collider
			{
				auto view = m_ActiveScene->GetEntitiesWith<BoxCollider2DComponent>();
				for (auto e : view)
				{
					Entity entity = Entity(e, m_ActiveScene.get());
					auto& b2D = entity.GetComponent<BoxCollider2DComponent>();
					auto& tc = entity.GetComponent<TransformComponent>();

					// Circle colliders never becomes ovals. 
					// Radius of collision circle is always equal to the larger scale axis.
					glm::mat4 transform = tc.GetLocalTransform();
					transform *= glm::translate(glm::mat4(1.0f), { b2D.Offset.x, b2D.Offset.y, 0.001f })
						* glm::scale(glm::mat4(1.0f), { b2D.Size.x, b2D.Size.y, 1.0f });
					Renderer2D::DrawRect(transform, m_CollisionMeshColor);
				}
			}
			// Circle collider
			{
				auto view = m_ActiveScene->GetEntitiesWith<CircleCollider2DComponent>();
				for (auto e : view)
				{
					Entity entity = Entity(e, m_ActiveScene.get());
					auto& c2D = entity.GetComponent<CircleCollider2DComponent>();
					auto& tc = entity.GetComponent<TransformComponent>();

					// Circle colliders never becomes ovals. 
					// Radius of collision circle is always equal to the larger scale axis.
					float maxScale = tc.LocalScale.x > tc.LocalScale.y ? tc.LocalScale.x : tc.LocalScale.y;
					glm::mat4 transform = glm::translate(glm::mat4(1.0f), tc.LocalPosition)
						* glm::rotate(glm::mat4(1.0f), tc.LocalRotation.z, glm::vec3(0, 0, 1))
						* glm::scale(glm::mat4(1.0f), { maxScale, maxScale, 1.0f });
					transform *= glm::translate(glm::mat4(1.0f), { c2D.Offset.x, c2D.Offset.y, 0.001f })
						* glm::scale(glm::mat4(1.0f), { c2D.Radius * 2.0f, c2D.Radius * 2.0f, 1.0f });

					Renderer2D::DrawDebugCircle(transform, m_CollisionMeshColor);
				}
			}
		}
	}

	void LocusEditorLayer::DrawActiveCameraView()
	{
		if (m_SelectedEntity.IsValid())
		{
			if (m_SelectedEntity.HasComponent<CameraComponent>())
			{
				SceneCamera& camera = m_SelectedEntity.GetComponent<CameraComponent>().Camera;
				camera.SetViewportSize(m_ViewportSize.x * 0.2f, m_ViewportSize.y * 0.2f);
				m_ActiveCameraFramebuffer->Bind();
				m_Framebuffer->ClearAttachmentInt(1, -1);

				RenderCommand::SetClearColor(camera.GetBackgroundColor());
				RenderCommand::Clear();

				Renderer2D::BeginScene(camera, m_ActiveScene->GetWorldTransform(m_SelectedEntity));

				{ // Sprite
					auto view = m_ActiveScene->GetEntitiesWith<SpriteRendererComponent>();
					for (auto e : view)
					{
						Entity entity = Entity(e, m_ActiveScene.get());
						bool enabled = entity.GetComponent<TagComponent>().Enabled;
						auto& sprite = entity.GetComponent<SpriteRendererComponent>();
						if (enabled)
							Renderer2D::DrawSprite(m_ActiveScene->GetWorldTransform(entity), sprite, -1);
					}
				}

				{ // Circle
					auto view = m_ActiveScene->GetEntitiesWith<CircleRendererComponent>();
					for (auto e : view)
					{
						Entity entity = Entity(e, m_ActiveScene.get());
						bool enabled = entity.GetComponent<TagComponent>().Enabled;
						auto& circle = entity.GetComponent<CircleRendererComponent>();
						if (enabled)
							Renderer2D::DrawCircle(m_ActiveScene->GetWorldTransform(entity), circle.Color, circle.Thickness, circle.Fade, -1);
					}
				}

				Renderer2D::EndScene();

				m_ActiveCameraFramebuffer->Unbind();
			}
		}
	}
}
