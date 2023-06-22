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
	auto app = Tiel::CreateApplication();
	app->Run();
	delete app;

	return 0;
}

#endif