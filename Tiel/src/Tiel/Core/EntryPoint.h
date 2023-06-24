// --- Entrypoint -------------------------------------------------------------
// Contains main function. Creates a project application where the 
// CreateApplication() is contained in the project (eg. Sandbox) application.
// Calls the run-loop.

#pragma once

#ifdef TIEL_PLATFORM_WINDOWS

// We dont want any connection with the client app so we cant link a header.
extern Tiel::Application* Tiel::CreateApplication();

int main(int argc, char** argv)
{
	// --- Initialize Log ---
	Tiel::Log::Init();

	// --- Create project application ---
	TIEL_PROFILE_BEGIN_SESSION("Startup", "TielProfile-Startup.json");
	auto app = Tiel::CreateApplication();
	TIEL_PROFILE_END_SESSION();

	TIEL_PROFILE_BEGIN_SESSION("Runtime", "TielProfile-Runtime.json");
	app->Run();
	TIEL_PROFILE_END_SESSION();

	TIEL_PROFILE_BEGIN_SESSION("Shutdown", "TielProfile-Shutdown.json");
	delete app;
	TIEL_PROFILE_END_SESSION();

	return 0;
}

#endif