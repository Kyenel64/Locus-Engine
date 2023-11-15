// --- Entrypoint -------------------------------------------------------------
// Entrypoint for Locus. Creates an application class defined by the client. 
#include "Lpch.h"
#pragma once

#include "Locus/Core/Application.h"

#ifdef LOCUS_PLATFORM_WINDOWS

// This is defined in the client application. 
extern Locus::Application* Locus::CreateApplication(ApplicationCommandLineArgs args);

int main(int argc, char** argv)
{
	LOCUS_PROFILE_BEGIN_SESSION("Startup", "LocusProfile-Startup.log");
	auto app = Locus::CreateApplication({ argc, argv });
	LOCUS_PROFILE_END_SESSION();

	LOCUS_PROFILE_BEGIN_SESSION("Runtime", "LocusProfile-Runtime.log");
	app->Run();
	LOCUS_PROFILE_END_SESSION();

	LOCUS_PROFILE_BEGIN_SESSION("Shutdown", "LocusProfile-Shutdown.log");
	delete app;
	LOCUS_PROFILE_END_SESSION();

	return 0;
}

#endif
