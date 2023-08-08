#pragma once

#include "Locus.h"
#include "Panels/SceneHierarchyPanel.h"
#include "Panels/ContentBrowserPanel.h"
#include "Locus/Renderer/EditorCamera.h"

#include <filesystem>

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
		bool OnMouseButtonReleased(MouseButtonReleasedEvent& e);

		void NewScene();
		void OpenScene();
		void OpenScene(const std::filesystem::path& path);
		void SaveSceneAs();
		void SaveScene();

		void OnScenePlay();
		void OnSceneStop();

		void showGizmoUI();
		void ProcessViewportDragDrop();
		void EditorToolbar();

	private:
		std::string m_SavePath;

		Ref<Scene> m_ActiveScene;

		// Textures
		Ref<Texture2D> m_PlayButton;
		Ref<Texture2D> m_PauseButton;

		// Viewport
		Ref<Framebuffer> m_Framebuffer;
		EditorCamera m_EditorCamera;
		bool m_ViewportFocused = false, m_ViewportHovered = false;
		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };
		glm::vec2 m_ViewportBounds[2];
		Entity m_HoveredEntity;
		Entity m_SelectedEntity;


		// Viewport Gizmo
		int m_GizmoType = -1;

		enum class SceneState { Edit = 0, Play = 1 };
		SceneState m_SceneState = SceneState::Edit;

		// Panels
		SceneHierarchyPanel m_SceneHierarchyPanel;
		ContentBrowserPanel m_ContentBrowserPanel;
	};
}