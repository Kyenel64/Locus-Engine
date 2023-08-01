#include <SideA.h>
// --- Entry Point ------------------------------------------------------------
#include <SideA/Core/EntryPoint.h>

#include "Sandbox2D.h"
#include "ExampleLayer.h"


namespace SideA
{
	class Sandbox : public Application
	{
	public:
		Sandbox(ApplicationCommandLineArgs args)
			: Application("Sandbox", args)
		{
			//PushLayer(new ExampleLayer());
			PushLayer(new Sandbox2D());
		}

		~Sandbox() {}
	};

	Application* CreateApplication(ApplicationCommandLineArgs args)
	{
		return new Sandbox(args);
	}
}


