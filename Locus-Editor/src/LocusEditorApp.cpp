// --- Entry Point ------------------------------------------------------------
#include <Locus.h>
#include <Locus/Core/EntryPoint.h>

#include "LocusEditorLayer.h"
#include "NewGUILayer.h"

namespace Locus
{
	class LocusEditorApp : public Application
	{
	public:
		LocusEditorApp(ApplicationCommandLineArgs args) 
			: Application("Locus Editor", args)
		{
			PushLayer(new LocusEditorLayer());
			//PushLayer(new NewGUILayer());
		}

		~LocusEditorApp() {}
	};

	Application* CreateApplication(ApplicationCommandLineArgs args)
	{
		return new LocusEditorApp(args);
	}
}