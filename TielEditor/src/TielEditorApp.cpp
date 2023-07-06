#include <Tiel.h>
// --- Entry Point ------------------------------------------------------------
#include <Tiel/Core/EntryPoint.h>

#include "TielEditorLayer.h"


namespace Tiel
{
	class TielEditorApp : public Application
	{
	public:
		TielEditorApp() : Application("Tiel Editor")
		{
			PushLayer(new TielEditorLayer());
		}

		~TielEditorApp() {}
	};

	Application* CreateApplication()
	{
		return new TielEditorApp();
	}
}