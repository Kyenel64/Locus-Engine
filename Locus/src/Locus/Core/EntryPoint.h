// --- Entrypoint -------------------------------------------------------------
// Entrypoint of Locus.
#pragma once

#include "Locus/Core/Application.h"

#ifdef LOCUS_PLATFORM_WINDOWS

// We dont want any connection with the client app so we cant link a header.
extern Locus::Application* Locus::CreateApplication(ApplicationCommandLineArgs args);

int main(int argc, char** argv) // *argv is program path, *argv[0+n] are command line arguments
{
	// --- Initialize Log ---
	Locus::Log::Init();

	LOCUS_CORE_WARN(*argv);

	// --- Create project application ---
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