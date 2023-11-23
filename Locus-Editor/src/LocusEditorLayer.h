// --- LocusEditorLayer -------------------------------------------------------
// Locus's editor. Uses dockspace ImGui.
#pragma once

#include <filesystem>

#include <Locus.h>

#include "Command/Command.h"
#include "Command/CommandHistory.h"
#include "Command/EntityCommands.h"
#include "Command/ValueCommands.h"
#include "Panels/SceneHierarchyPanel.h"
#include "Panels/ContentBrowserPanel.h"
#include "Panels/PropertiesPanel.h"

namespace Locus
{
	enum class LayoutStyle { Default = 0 };
	enum class Panels { None = 0, Viewport = 1, SceneHierarchy = 2, Properties = 3, ContentBrowser = 4, Debug = 5 };

	class LocusEditorLayer : public Layer
	{
	public:
		LocusEditorLayer();
		virtual ~LocusEditorLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate(Timestep deltaTime) override;
		virtual void OnEvent(Event& event) override;
		virtual void OnImGuiRender() override;

		void SetSavedStatus(bool status) { m_IsSaved = status; }

	private:
		// Events
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnKeyPressed(KeyPressedEvent& e);
		bool OnMouseButtonPressed(MouseButtonPressedEvent& e);

		// Serialization
		void NewScene();
		void OpenScene();
		void OpenScene(const std::filesystem::path& path);
		bool SaveSceneAs();
		bool SaveScene();

		// Viewport
		void showGizmoUI();
		void ProcessViewportDragDrop();
		void OnScenePlay();
		void OnPhysicsPlay();
		void OnSceneStop();

		// Layout
		void DrawViewport();
		void DrawViewportToolbar(const glm::vec2& position);
		void DrawLayoutTable();
		void DrawActivePanel(Panels activePanel);
		void DrawTabBar(const std::string& frameName, std::vector<Panels> panelList, Panels& activePanel);
		void DrawToolbar();
		void DrawSettingsBar();
		void DrawDebugPanel();
		void ProcessSavePopup();
		void ProcessViewSettingsPopup();

		// Overlay
		void OnRenderOverlay();
		void DrawCollisionMesh();

	private:
		glm::vec2 m_WindowSize;
		bool m_IsSaved = true;
		bool m_OpenSavePopup = false;

		// Scene
		std::string m_SavePath;
		Ref<Scene> m_ActiveScene;
		Ref<Scene> m_EditorScene;
		enum class SceneState { Edit = 0, Play, Pause, Physics, PhysicsPause};
		SceneState m_SceneState = SceneState::Edit;

		// Textures
		Ref<Texture2D> m_PlayIcon;
		Ref<Texture2D> m_PauseIcon;
		Ref<Texture2D> m_StopIcon;
		Ref<Texture2D> m_PhysicsIcon;
		Ref<Texture2D> m_PointerIcon;
		Ref<Texture2D> m_TranslateIcon;
		Ref<Texture2D> m_RotateIcon;
		Ref<Texture2D> m_ScaleIcon;

		// Viewport
		Ref<Framebuffer> m_Framebuffer;
		EditorCamera m_EditorCamera;
		bool m_ViewportFocused = false, m_ViewportHovered = false;
		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };
		glm::vec2 m_ViewportBounds[2];

		// Overlay
		glm::vec4 m_CollisionMeshColor;
		glm::vec4 m_FocusOutlineColor;
		bool m_ShowAllCollisionMesh = false;

		// Entity
		Entity m_HoveredEntity;
		Entity m_SelectedEntity;
		Entity m_ClipboardEntity;

		// Viewport Gizmo
		int m_GizmoType = -1;
		bool m_GizmoVisible = false;
		bool m_GizmoFirstClick = true;

		// Panels
		SceneHierarchyPanel m_SceneHierarchyPanel;
		ContentBrowserPanel m_ContentBrowserPanel;
		PropertiesPanel m_PropertiesPanel;

		// Layout
		float m_LeftSplitterPos = 0.7f;
		float m_RightSplitterPos = 0.3f;
		float m_CenterSplitterPos = 0.8f;
		LayoutStyle m_LayoutStyle = LayoutStyle::Default;

		Panels m_Frame2ActivePanel = Panels::Debug;
		std::vector<Panels> m_Frame2AttachedPanels = { Panels::ContentBrowser, Panels::Debug };
		Panels m_Frame3ActivePanel = Panels::SceneHierarchy;
		std::vector<Panels> m_Frame3AttachedPanels = { Panels::SceneHierarchy };
		Panels m_Frame4ActivePanel = Panels::Properties;
		std::vector<Panels> m_Frame4AttachedPanels = { Panels::Properties };
	};
}
