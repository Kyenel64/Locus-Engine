#pragma once

#include <string>

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

		void OnImGuiRender();

		void InitGLFW();
		void InitImGui();

		void CenterGLFWWindow();
		void SetDarkTheme();

		void Close() { m_IsRunning = false; }

		inline static Launcher& Get() { return *s_Instance; }

	private:
		static Launcher* s_Instance;
		GLFWwindow* m_Window = nullptr;
		uint32_t m_WindowWidth;
		uint32_t m_WindowHeight;
		std::string m_WindowName = "Locus Launcher";
		bool m_IsRunning = true;

		LocalProjectsPanel m_LocalProjectsPanel;
	};
}