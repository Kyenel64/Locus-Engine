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

		void showGizmoUI();
		void ProcessViewportDragDrop();

	private:
		std::string m_SavePath;

		Ref<Scene> m_ActiveScene;

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

		// Panels
		SceneHierarchyPanel m_SceneHierarchyPanel;
		ContentBrowserPanel m_ContentBrowserPanel;
	};
}
