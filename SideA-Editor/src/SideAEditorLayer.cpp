#include "SideAEditorLayer.h"

#include "imgui/imgui.h"
#include <glm/gtc/type_ptr.hpp>

#include "SideA/Scene/SceneSerializer.h"
#include "SideA/Utils/PlatformUtils.h"

namespace SideA
{
	SideAEditorLayer::SideAEditorLayer()
		: Layer("SideAEditorLayer"), m_CameraController(1920.0f / 1080.0f)
	{

	}

	void SideAEditorLayer::OnAttach()
	{
		SIDEA_PROFILE_FUNCTION();

		// Textures
		m_CheckerboardTexture = Texture2D::Create("assets/textures/Checkerboard.png");
		m_SpriteSheet = Texture2D::Create("assets/textures/TX_Tileset_Grass.png");
		m_Grass = SubTexture2D::CreateFromCoords(m_SpriteSheet, { 1, 0 }, { 32, 32 }, { 1, 2 });

		// Framebuffer
		FramebufferSpecs FramebufferSpecs;
		FramebufferSpecs.Width = 1920;
		FramebufferSpecs.Height = 1080;
		m_Framebuffer = Framebuffer::Create(FramebufferSpecs);

		// Scene
		m_ActiveScene = CreateRef<Scene>();

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
		if (FramebufferSpecs spec = m_Framebuffer->GetSpecification();
			m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f &&
			(spec.Width != m_ViewportSize.x || spec.Height != m_ViewportSize.y))
		{
			m_Framebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_CameraController.OnResize(m_ViewportSize.x, m_ViewportSize.y);
			m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		}

		// Update
		if (m_ViewportFocused)
			m_CameraController.OnUpdate(deltaTime);

		// Render
		Renderer2D::ResetStats();
		m_Framebuffer->Bind();
		m_ActiveScene->OnUpdate(deltaTime);
		m_Framebuffer->Unbind();

		Renderer2D::StatsEndFrame();
	}

	void SideAEditorLayer::OnEvent(Event& e)
	{
		m_CameraController.OnEvent(e);

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<KeyPressedEvent>(SIDEA_BIND_EVENT_FN(SideAEditorLayer::OnKeyPressed));
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
		float minWinSizeX = style.WindowMinSize.x;
		style.WindowMinSize.x = 400.0f;
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}
		style.WindowMinSize.x = minWinSizeX;

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
					Application::Get().Close();

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		// --- Panels ---------------------------------------------------------
		m_SceneHierarchyPanel.OnImGuiRender();

		// --- Stats window ---------------------------------------------------
		ImGui::Begin("Stats");
		auto stats = Renderer2D::GetStats();
		ImGui::Text("Renderer2D Stats:");
		ImGui::Text("Draw Calls: %d", stats.DrawCalls);
		ImGui::Text("Quads: %d", stats.QuadCount);
		ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
		ImGui::Text("Indices: %d", stats.GetTotalIndexCount());
		ImGui::Text("Frame Time: %f", stats.FrameTime);
		ImGui::Text("FPS: %f", stats.FramesPerSecond);
		ImGui::End();

		// --- Viewport window ------------------------------------------------
		// saved status
		m_SavedStatus = m_SceneHierarchyPanel.GetSavedStatus();
		ImGuiWindowFlags viewportFlags = 0;
		if (!m_SavedStatus)
			viewportFlags = ImGuiWindowFlags_UnsavedDocument;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin("Viewport", 0, viewportFlags);
	
		m_ViewportFocused = ImGui::IsWindowFocused();
		m_ViewportHovered = ImGui::IsWindowHovered();
		Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportFocused || !m_ViewportHovered);

		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };

		uint32_t textureID = m_Framebuffer->GetColorAttachmentRendererID();
		ImGui::Image((void*)textureID, ImVec2{ m_ViewportSize.x, m_ViewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
		ImGui::End();
		ImGui::PopStyleVar();

		ImGui::End();
	}

	bool SideAEditorLayer::OnKeyPressed(KeyPressedEvent& e)
	{
		if (e.GetRepeatCount() > 0)
			return false;

		bool control = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
		bool shift = Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift);
		// TODO: Currently only works within viewport window
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
		}
	}

	void SideAEditorLayer::NewScene()
	{
		m_ActiveScene = CreateRef<Scene>();
		m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
		m_SavePath = std::string();
		m_SavedStatus = false;
		m_SceneHierarchyPanel.SetSavedStatus(false);
	}

	void SideAEditorLayer::OpenScene()
	{
		std::string path = FileDialogs::OpenFile("SideA Scene (*.sidea)\0*.sidea\0");
		if (!path.empty())
		{
			m_ActiveScene = CreateRef<Scene>();
			m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_SceneHierarchyPanel.SetContext(m_ActiveScene);

			SceneSerializer serializer(m_ActiveScene);
			serializer.Deserialize(path);
			m_SavePath = path;
			m_SavedStatus = true;
			m_SceneHierarchyPanel.SetSavedStatus(true);
		}
	}

	void SideAEditorLayer::SaveSceneAs()
	{
		std::string path = FileDialogs::SaveFile("SideA Scene (*.sidea)\0*.sidea\0");
		if (!path.empty())
		{
			SceneSerializer serializer(m_ActiveScene);
			serializer.Serialize(path);
			m_SavePath = path;
			m_SavedStatus = true;
			m_SceneHierarchyPanel.SetSavedStatus(true);
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
			m_SavedStatus = true;
			m_SceneHierarchyPanel.SetSavedStatus(true);
		}
	}
}
