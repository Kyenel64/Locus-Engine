#include "LocusLauncher.h"
#include "LocusEditorLayer.h"

#include <Locus.h>

int main(int argc, char** argv)
{
	// Opens the locus launcher if a project is not set through command line arguments.
	// Opens the editor if project path and name is provided.
	if (argc > 2)
	{
		Locus::Application* app = new Locus::Application("Locus Launcher App", argv[1], argv[2]);
		app->PushLayer(new Locus::LocusEditorLayer());
		app->Run();
		delete app;
	}
	else
	{
		// TODO: Write separate GUI application. 
		// Shouldnt create a whole Locus application just for launcher.
		Locus::Application* app = new Locus::Application("Locus Launcher App");
		app->PushLayer(new Locus::LocusLauncher());
		app->Run();
		delete app;
	}
	
	return 0;
}
