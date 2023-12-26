#include "LLpch.h"
#include "Launcher.h"

#include "Utils/Colors.h"

namespace LocusLauncher
{
	Launcher* Launcher::s_Instance = nullptr;

	Launcher::Launcher(const std::string& name, uint32_t windowWidth, uint32_t windowHeight)
	{
		if (!s_Instance)
			s_Instance = this;
		m_Window = std::make_unique<Window>(name, windowWidth, windowHeight);
		m_GUI = std::make_unique<GUI>();
	}

	Launcher::~Launcher()
	{
		
	}

	void Launcher::Run()
	{
		while (m_IsRunning)
		{
			m_GUI->Begin();
			m_LocalProjectsPanel.OnImGuiRender();
			m_GUI->End();

			m_Window->OnUpdate();
		}
	}
}
