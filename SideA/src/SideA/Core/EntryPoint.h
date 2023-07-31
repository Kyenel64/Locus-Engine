// --- Entrypoint -------------------------------------------------------------
// Contains main function. Creates a project application where the 
// CreateApplication() is contained in the project (eg. Sandbox) application.
// Calls the run-loop.

#pragma once

#ifdef SIDEA_PLATFORM_WINDOWS

// We dont want any connection with the client app so we cant link a header.
extern SideA::Application* SideA::CreateApplication();

int main(int argc, char** argv) // *argv is program path, *argv[0+n] are command line arguments
{
	// --- Initialize Log ---
	SideA::Log::Init();

	SIDEA_CORE_WARN(*argv);

	// --- Create project application ---
	SIDEA_PROFILE_BEGIN_SESSION("Startup", "SideAProfile-Startup.log");
	auto app = SideA::CreateApplication();
	SIDEA_PROFILE_END_SESSION();

	SIDEA_PROFILE_BEGIN_SESSION("Runtime", "SideAProfile-Runtime.log");
	app->Run();
	SIDEA_PROFILE_END_SESSION();

	SIDEA_PROFILE_BEGIN_SESSION("Shutdown", "SideAProfile-Shutdown.log");
	delete app;
	SIDEA_PROFILE_END_SESSION();

	return 0;
}

#endif