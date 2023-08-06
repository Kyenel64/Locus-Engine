#include "SideAEditorLayer.h"

#include "imgui/imgui.h"
#include <glm/gtc/type_ptr.hpp>
#include "ImGuizmo.h"

#include "SideA/Core/Application.h"
#include "SideA/Scene/SceneSerializer.h"
#include "SideA/Utils/PlatformUtils.h"
#include "SideA/Math/Math.h"

#include "Command/Command.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>

namespace SideA
{
	extern const std::filesystem::path g_ProjectPath;

	SideAEditorLayer::SideAEditorLayer()
		: Layer("SideAEditorLayer")
	{

	}

	SideAEditorLayer::~SideAEditorLayer()
	{
		CommandHistory::Shutdown();
	}

	void SideAEditorLayer::OnAttach()
	{
		SIDEA_PROFILE_FUNCTION();

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
		m_ActiveScene = CreateRef<Scene>();

		// Open startup project given through args
		auto commandLineArgs = Application::Get().GetCommandLineArgs();
		if (commandLineArgs.Count > 1)
		{
			auto sceneFilePath = commandLineArgs[1];
			SceneSerializer serializer(m_ActiveScene);
			serializer.Deserialize(sceneFilePath);
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

		// Editor Camera
		m_EditorCamera = EditorCamera(30.0f, 1920.0f / 1080.0f, 0.1f, 1000.0f);

	}

	void SideAEditorLayer::OnDetach()
	{
		SIDEA_PROFILE_FUNCTION();
	}

	void SideAEditorLayer::OnUpdate(Timestep deltaTime)
	{
		// Profiling
		SIDEA_PROFILE_FUNCTION();
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

		// Update
		m_EditorCamera.OnUpdate(deltaTime); // These are camera specific update commands. Actual rendering is in scene object.

		// --- Render ---------------------------------------------------------
		Renderer2D::ResetStats();
		m_Framebuffer->Bind();
		
		// Clears
		RenderCommand::SetClearColor(m_EditorCamera.GetBackgroundColor());
		RenderCommand::Clear();
		m_Framebuffer->ClearAttachmentInt(1, -1);

		m_ActiveScene->OnUpdateEditor(deltaTime, m_EditorCamera);

		// Read pixel
		auto [mx, my] = ImGui::GetMousePos();
		mx -= m_ViewportBounds[0].x;
		my -= m_ViewportBounds[0].y;
		glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];
		my = viewportSize.y - my;
		int mouseX = (int)mx;
		int mouseY = (int)my;

		if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)viewportSize.x && mouseY < (int)viewportSize.y)
		{
			int pixelData = m_Framebuffer->ReadPixel(1, mouseX, mouseY);
			m_HoveredEntity = pixelData == -1 ? Entity() : Entity((entt::entity)pixelData, m_ActiveScene.get());
		}
		m_SelectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();

		
		m_Framebuffer->Unbind();
		// --- Render End -----------------------------------------------------
		Renderer2D::StatsEndFrame();
	}

	void SideAEditorLayer::OnEvent(Event& e)
	{
		m_EditorCamera.OnEvent(e);

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<KeyPressedEvent>(SIDEA_BIND_EVENT_FN(SideAEditorLayer::OnKeyPressed));
		dispatcher.Dispatch<MouseButtonPressedEvent>(SIDEA_BIND_EVENT_FN(SideAEditorLayer::OnMouseButtonPressed));
		dispatcher.Dispatch<MouseButtonReleasedEvent>(SIDEA_BIND_EVENT_FN(SideAEditorLayer::OnMouseButtonReleased));
	}

	void SideAEditorLayer::OnImGuiRender()
	{
		SIDEA_PROFILE_FUNCTION();
		//ImGui::ShowDemoWindow();

		static bool dockspaceOpen = true;
		static bool opt_fullscreen_persistant = true;
		bool opt_fullscreen = opt_fullscreen_persistant;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->Pos);
			ImGui::SetNextWindowSize(viewport->Size);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}

		// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
		// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive, 
		// all active windows docked into it will lose their parent and become undocked.
		// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise 
		// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace", &dockspaceOpen, window_flags);
		ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// DockSpace
		ImGuiIO& io = ImGui::GetIO();
		ImGuiStyle& style = ImGui::GetStyle();
		//float minWinSizeX = style.WindowMinSize.x;
		//style.WindowMinSize.x = 400.0f;
		style.WindowMenuButtonPosition = ImGuiDir_None;
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}
		//style.WindowMinSize.x = minWinSizeX;

		// --- Menu Bar -------------------------------------------------------
		if (ImGui::BeginMenuBar())
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

			ImGui::EndMenuBar();
		}

		// --- Debug panel ---------------------------------------------------
		ImGui::Begin("Debug");

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
			name = m_HoveredEntity.GetComponent<TagComponent>().Tag;
		ImGui::Text("Hovered Entity: %s", name.c_str());

		ImGui::End();

		// --- Panels ---------------------------------------------------------
		m_SceneHierarchyPanel.OnImGuiRender();
		m_ContentBrowserPanel.OnImGuiRender();

		// --- Viewport window ------------------------------------------------
		ImGuiWindowFlags viewportFlags = ImGuiWindowFlags_MenuBar;
		if (!Application::Get().GetIsSavedStatus())
			viewportFlags |= ImGuiWindowFlags_UnsavedDocument;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin("Viewport", 0, viewportFlags);
	
		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };

		uint32_t textureID = m_Framebuffer->GetColorAttachmentRendererID();
		ImGui::Image((void*)(uintptr_t)textureID, ImVec2{ m_ViewportSize.x, m_ViewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

		m_ViewportBounds[0].x = ImGui::GetItemRectMin().x, m_ViewportBounds[0].y = ImGui::GetItemRectMin().y;
		m_ViewportBounds[1].x = ImGui::GetItemRectMax().x, m_ViewportBounds[1].y = ImGui::GetItemRectMax().y;
		m_ViewportHovered = ImGui::IsMouseHoveringRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
		m_ViewportFocused = ImGui::IsWindowFocused();
		//Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportFocused);

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ITEM_PATH"))
			{
				const wchar_t* path = (const wchar_t*)payload->Data;
				OpenScene(std::filesystem::path(g_ProjectPath) / path);
			}
			ImGui::EndDragDropTarget();
		}
		
		// --- viewport gizmo ---
		Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
		if (selectedEntity && m_GizmoType != -1)
			showGizmoUI();

		// --- viewport menu ---
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
		
		ImGui::End(); // End viewport
		ImGui::PopStyleVar();

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

	bool SideAEditorLayer::OnKeyPressed(KeyPressedEvent& e)
	{
		//if (e.GetRepeatCount() > 0)
			//return false;

		bool control = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
		bool shift = Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift);
		switch (e.GetKeyCode())
		{
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
			case Key::Z:
			{
				if (control)
					CommandHistory::Undo();
				break;
			}
			case Key::Y:
			{
				if (control)
					CommandHistory::Redo();
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

	bool SideAEditorLayer::OnMouseButtonPressed(MouseButtonPressedEvent& e)
	{
		if (e.GetMouseButton() == Mouse::ButtonLeft)
		{
			if (m_ViewportHovered && !ImGuizmo::IsOver())
				m_SceneHierarchyPanel.SetSelectedEntity(m_HoveredEntity);
		}

		return false;
	}

	bool SideAEditorLayer::OnMouseButtonReleased(MouseButtonReleasedEvent& e)
	{
		CommandHistory::SetNoMergeMostRecent();
		return false;
	}

	void SideAEditorLayer::NewScene()
	{
		m_ActiveScene = CreateRef<Scene>();
		m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
		m_SavePath = std::string();
		Application::Get().SetIsSavedStatus(false);
		CommandHistory::Reset();
	}

	void SideAEditorLayer::OpenScene()
	{
		std::string path = FileDialogs::OpenFile("SideA Scene (*.sidea)\0*.sidea\0");
		if (!path.empty())
			OpenScene(path);
	}

	void SideAEditorLayer::OpenScene(const std::filesystem::path& path)
	{
		m_ActiveScene = CreateRef<Scene>();
		m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		m_SceneHierarchyPanel.SetContext(m_ActiveScene);

		SceneSerializer serializer(m_ActiveScene);
		serializer.Deserialize(path.string());
		m_SavePath = path.string();
		Application::Get().SetIsSavedStatus(true);
		CommandHistory::Reset();
	}

	void SideAEditorLayer::SaveSceneAs()
	{
		std::string path = FileDialogs::SaveFile("SideA Scene (*.sidea)\0*.sidea\0");
		if (!path.empty())
		{
			SceneSerializer serializer(m_ActiveScene);
			serializer.Serialize(path);
			m_SavePath = path;
			Application::Get().SetIsSavedStatus(true);
		}
	}

	void SideAEditorLayer::SaveScene()
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

	void SideAEditorLayer::showGizmoUI()
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
}
