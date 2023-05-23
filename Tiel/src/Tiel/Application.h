// --- Application ------------------------------------------------------------
// Main application class for the game engine.
// Contains pointer to window class. Contains core run loop.
// Contains a layer stack for executing layers.

#pragma once
#include "Core.h"

#include "Window.h"
#include "Events/Event.h"
#include "Tiel/LayerStack.h"
#include "Tiel/Events/ApplicationEvent.h"

namespace Tiel
{
	class TIEL_API Application
	{
	public:
		// Constructor
		Application();
		virtual ~Application();

		// Main run loop for engine
		void Run();

		// Called whenever there is an event
		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

		inline Window& GetWindow() { return *m_Window; }
		inline static Application& Get() { return *s_Instance; }
	private:
		bool OnWindowClose(WindowCloseEvent& e);

		std::unique_ptr<Window> m_Window;
		bool m_Running = true;
		LayerStack m_LayerStack;

	private:
		static Application* s_Instance;
	};

	// To be defined in client
	Application* CreateApplication();
}