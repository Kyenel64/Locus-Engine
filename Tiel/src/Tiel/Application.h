#pragma once
#include "Core.h"

namespace Tiel
{
	class TIEL_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();
	};

	// To be defined in client
	Application* CreateApplication();
}