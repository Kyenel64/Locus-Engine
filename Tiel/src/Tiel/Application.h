// --- Application ------------------------------------------------------------
// Main application class for the game engine.
// Contains pointer to window class. Contains core run loop.

#pragma once
#include "Core.h"
#include "Events/Event.h"
#include "Tiel/Events/ApplicationEvent.h"
#include "Window.h"

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
	private:
		bool OnWindowClose(WindowCloseEvent& e);

		std::unique_ptr<Window> m_Window;
		bool m_Running = true;
	};

	// To be defined in client
	Application* CreateApplication();
}