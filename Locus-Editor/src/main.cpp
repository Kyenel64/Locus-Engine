#include "LocusEditorLayer.h"

#include <Locus.h>
#include <LocusLauncher.h>

int main(int argc, char** argv)
{
	// Opens the locus launcher if a project is not set through command line arguments.
	// Opens the editor if project path and name is provided.
	if (argc > 2)
	{
		Locus::Application* app = new Locus::Application("Locus Editor", argv[1], argv[2]);
		app->PushLayer(new Locus::LocusEditorLayer());
		app->Run();
		delete app;
	}
	else
	{
		// Basic GUI application. Write a GUI library in the future for more flexibility.
		LocusLauncher::Launcher* launcher = new LocusLauncher::Launcher("Locus Launcher", 1200, 700);
		launcher->Run();
		delete launcher;
	}
	
	return 0;
}
