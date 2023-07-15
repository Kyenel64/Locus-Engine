#include <SideA.h>
// --- Entry Point ------------------------------------------------------------
#include <SideA/Core/EntryPoint.h>

#include "SideAEditorLayer.h"
#include "SideAExport.h"

//#define SIDEA_EXPORT

namespace SideA
{
	class SideAEditorApp : public Application
	{
	public:
		SideAEditorApp() : Application("SideA Editor")
		{
			#ifdef SIDEA_EXPORT
				PushLayer(new SideAExport());
			#else
				PushLayer(new SideAEditorLayer());
			#endif
		}

		~SideAEditorApp() {}
	};

	Application* CreateApplication()
	{
		return new SideAEditorApp();
	}
}