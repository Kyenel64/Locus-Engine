// --- Application ------------------------------------------------------------
// Main application class for the game engine.
// Contains pointer to window class. Contains core run loop.
// Contains a layer stack for executing layers.

#pragma once

#include "Core.h"

#include "Window.h"
#include "SideA/Events/Event.h"

#include "SideA/Core/LayerStack.h"
#include "SideA/Events/ApplicationEvent.h"
#include "SideA/ImGui/ImGuiLayer.h"

#include "SideA/Renderer/Shader.h"
#include "SideA/Renderer/Buffer.h"
#include "SideA/Renderer/VertexArray.h"

#include "SideA/Renderer/OrthographicCamera.h"

namespace SideA
{
	class Application
	{
	public:
		// Creates window, sets event callbacks, and creates ImGui layer.
		Application(const std::string& name = "SideA App");
		virtual ~Application() {}

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

		void OnEvent(Event& e);
		// --- Main Engine loop ---
		void Run();
		void Close();

		inline Window& GetWindow() { return *m_Window; }
		inline static Application& Get() { return *s_Instance; }
		ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }

	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

	private:
		static Application* s_Instance;

		Scope<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		bool m_Minimized = false;
		LayerStack m_LayerStack;

		float m_LastFrameTime = 0.0f;
	};

	// To be defined in client
	Application* CreateApplication();
}