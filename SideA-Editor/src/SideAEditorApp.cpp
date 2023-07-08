#include <SideA.h>
// --- Entry Point ------------------------------------------------------------
#include <SideA/Core/EntryPoint.h>

#include "SideAEditorLayer.h"


namespace SideA
{
	class SideAEditorApp : public Application
	{
	public:
		SideAEditorApp() : Application("SideA Editor")
		{
			PushLayer(new SideAEditorLayer());
		}

		~SideAEditorApp() {}
	};

	Application* CreateApplication()
	{
		return new SideAEditorApp();
	}
}