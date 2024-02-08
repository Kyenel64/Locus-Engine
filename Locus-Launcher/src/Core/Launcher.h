#pragma once

#include "GUI.h"
#include "Window.h"
#include "LocalProjectsPanel.h"

struct GLFWwindow;

namespace LocusLauncher
{
	class Launcher
	{
	public:
		Launcher() = default;
		Launcher(const std::string& name, uint32_t windowWidth = 1920, uint32_t windowHeight = 1080);
		~Launcher();

		void Run();
		void Close() { m_IsRunning = false; }

		inline static Launcher& Get() { return *s_Instance; }
		inline Window& GetWindow() const { return *m_Window; }

	private:
		static Launcher* s_Instance;
		std::unique_ptr<Window> m_Window = nullptr;
		std::unique_ptr<GUI> m_GUI = nullptr;
		bool m_IsRunning = true;

		LocalProjectsPanel m_LocalProjectsPanel;
	};
}