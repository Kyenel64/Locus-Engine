#pragma once

#include "SideA.h"
#include "Panels/SceneHierarchyPanel.h"
#include "SideA/Renderer/EditorCamera.h"

namespace SideA
{
	class SideAEditorLayer : public Layer
	{
	public:
		SideAEditorLayer();
		virtual ~SideAEditorLayer() {}

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate(Timestep deltaTime) override;
		virtual void OnEvent(Event& event) override;
		virtual void OnImGuiRender() override;

	private:
		bool OnKeyPressed(KeyPressedEvent& e);

		void NewScene();
		void OpenScene();
		void SaveSceneAs();
		void SaveScene();

		void showGizmoUI();

	private:
		bool m_SavedStatus = false;
		std::string m_SavePath;

		Ref<Scene> m_ActiveScene;

		// Viewport
		Ref<Framebuffer> m_Framebuffer;
		EditorCamera m_EditorCamera;
		bool m_ViewportFocused = false; bool m_ViewportHovered = false;
		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };
		float m_ViewportMenuHeight;


		// Viewport Gizmo
		int m_GizmoType = -1;

		// Panels
		SceneHierarchyPanel m_SceneHierarchyPanel;

	};
}
