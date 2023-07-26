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
		OrthographicCameraController m_CameraController;
		Ref<Framebuffer> m_Framebuffer;
		bool m_ViewportFocused = false; bool m_ViewportHovered = false;
		bool m_SavedStatus = false;

		Ref<Scene> m_ActiveScene;
		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };

		// Panels
		SceneHierarchyPanel m_SceneHierarchyPanel;
		std::string m_SavePath;

		int m_GizmoType = -1;

		EditorCamera m_EditorCamera;
	};
}
