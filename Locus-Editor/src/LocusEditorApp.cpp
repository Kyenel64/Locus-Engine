#include "LocusEditorLayer.h"

#include <Locus.h>

namespace Locus
{
	class LocusEditorApp : public Application
	{
	public:
		LocusEditorApp(ApplicationCommandLineArgs args) 
			: Application("Locus Editor", args)
		{
			PushLayer(new LocusEditorLayer());
		}

		~LocusEditorApp() = default;
	};

	Application* CreateApplication(ApplicationCommandLineArgs args)
	{
		return new LocusEditorApp(args);
	}
}