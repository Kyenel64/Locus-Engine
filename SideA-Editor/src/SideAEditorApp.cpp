#include <SideA.h>
// --- Entry Point ------------------------------------------------------------
#include <SideA/Core/EntryPoint.h>

#include "SideAEditorLayer.h"

//#define SIDEA_EXPORT

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