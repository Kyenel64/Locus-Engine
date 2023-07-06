// --- Application ------------------------------------------------------------
// Main application class for the game engine.
// Contains pointer to window class. Contains core run loop.
// Contains a layer stack for executing layers.

#pragma once

#include "Core.h"

#include "Window.h"
#include "Tiel/Events/Event.h"

#include "Tiel/Core/LayerStack.h"
#include "Tiel/Events/ApplicationEvent.h"
#include "Tiel/ImGui/ImGuiLayer.h"

#include "Tiel/Renderer/Shader.h"
#include "Tiel/Renderer/Buffer.h"
#include "Tiel/Renderer/VertexArray.h"

#include "Tiel/Renderer/OrthographicCamera.h"

namespace Tiel
{
	class Application
	{
	public:
		// Creates window, sets event callbacks, and creates ImGui layer.
		Application(const std::string& name = "Tiel App");
		virtual ~Application() {}

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

		void OnEvent(Event& e);
		// --- Main Engine loop ---
		void Run();
		void Close();

		inline Window& GetWindow() { return *m_Window; }
		inline static Application& Get() { return *s_Instance; }

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