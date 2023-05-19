#include "tpch.h"
#include "Application.h"

#include "Tiel/Events/ApplicationEvent.h"
#include "Tiel/Log.h"

namespace Tiel
{
	Application::Application()
	{

	}

	Application::~Application()
	{

	}

	void Application::Run()
	{
		WindowResizeEvent e(1280, 720);
		if (e.IsInCategory(EventCategoryApplication))
		{
			TIEL_TRACE(e);
		}
		if (e.IsInCategory(EventCategoryInput))
		{
			TIEL_TRACE(e);
		}

		while (true);
	}
}