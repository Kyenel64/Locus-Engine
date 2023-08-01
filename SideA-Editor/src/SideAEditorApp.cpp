#include <SideA.h>
// --- Entry Point ------------------------------------------------------------
#include <SideA/Core/EntryPoint.h>

#include "SideAEditorLayer.h"

namespace SideA
{
	class SideAEditorApp : public Application
	{
	public:
		SideAEditorApp(ApplicationCommandLineArgs args) 
			: Application("SideA Editor", args)
		{
			PushLayer(new SideAEditorLayer());
		}

		~SideAEditorApp() {}
	};

	Application* CreateApplication(ApplicationCommandLineArgs args)
	{
		return new SideAEditorApp(args);
	}
}