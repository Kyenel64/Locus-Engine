// --- LocusEditorLayer -------------------------------------------------------
// Locus's editor. Uses dockspace ImGui.
#pragma once

#include <Locus.h>

#include "Panels/SceneHierarchyPanel.h"
#include "Panels/ContentBrowserPanel.h"
#include "Panels/PropertiesPanel.h"
#include "Panels/ConsolePanel.h"
#include "Panels/ResourceInspectorPanel.h"

namespace Locus
{
	enum class LayoutStyle { Default = 0 };

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
		void DrawGizmo();
		void ProcessViewportDragDrop();
		void OnScenePlay();
		void OnPhysicsPlay();
		void OnSceneStop();

		// Layout
		void DrawViewport();
		void DrawViewportToolbar(const glm::vec2& position);
		void DrawDefaultLayout();
		void DrawToolbar();
		void DrawDebugPanel();
		void ProcessSavePopup();
		void ProcessViewSettingsPopup();

		// Overlay
		void OnRenderOverlay();
		// Draws to a framebuffer. Rendering is separate.
		void DrawActiveCameraView();
		void DrawToMaskFramebuffer();

	private:
		glm::vec2 m_WindowSize;
		bool m_IsSaved = true;
		bool m_OpenSavePopup = false;
		bool m_BlockEditorKeyInput = false;
		std::filesystem::path m_ProjectPath;
		std::string m_ProjectName;

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
		bool m_ShowAllCollisionMesh = false;
		glm::vec2 m_ActiveCameraViewportSize;
		Ref<Framebuffer> m_ActiveCameraFramebuffer;
		Ref<Framebuffer> m_MaskFramebuffer;
		Ref<Texture> m_MaskTexture;
		Ref<Shader> m_MaskShader;
		Ref<Shader> OutlinePostProcessShader;

		// Entity
		Entity m_SelectedEntity;
		Entity m_ClipboardEntity;

		// Viewport Gizmo
		int m_GizmoType = -1;
		bool m_GizmoVisible = false;
		bool m_GizmoFirstClick = true;

		// Panels
		Ref<SceneHierarchyPanel> m_SceneHierarchyPanel;
		Ref<ContentBrowserPanel> m_ContentBrowserPanel;
		Ref<PropertiesPanel> m_PropertiesPanel;
		Ref<ConsolePanel> m_ConsolePanel;
		Ref<ResourceInspectorPanel> m_ResourceInspectorPanel;

		// Layout
		LayoutStyle m_LayoutStyle = LayoutStyle::Default;
	};
}
