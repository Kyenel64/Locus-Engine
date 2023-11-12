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

	private:
		// Events
		bool OnKeyPressed(KeyPressedEvent& e);
		bool OnMouseButtonPressed(MouseButtonPressedEvent& e);

		// Serialization
		void NewScene();
		void OpenScene();
		void OpenScene(const std::filesystem::path& path);
		void SaveSceneAs();
		void SaveScene();

		// Viewport
		void showGizmoUI();
		void ProcessViewportDragDrop();
		void OnScenePlay();
		void OnPhysicsPlay();
		void OnSceneStop();

		// Layout
		void DrawViewport();
		void DrawLayoutTable();
		void DrawToolbar();
		void DrawDebugPanel();
		void ProcessSavePopup();
		void ProcessViewSettingsPopup(const glm::vec2& position);

		// Overlay
		void OnRenderOverlay();
		void DrawCollisionMesh();

	private:
		glm::vec2 m_WindowSize;
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
		float m_ViewportHeight; // Makes more sense to do m_LeftSplitterPos & m_RightSplitterPos
		float m_HierarchyHeight;
		float m_CenterSplitterPos;
		glm::vec2 m_FrameSizes[4];
		glm::vec2 m_FramePositions[4];
		enum class LayoutStyle { Default = 0 };
		LayoutStyle m_LayoutStyle = LayoutStyle::Default;
	};
}
