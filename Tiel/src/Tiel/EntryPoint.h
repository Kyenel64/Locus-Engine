#pragma once

#ifdef TIEL_PLATFORM_WINDOWS

extern Tiel::Application* Tiel::CreateApplication();

int main(int argc, char** argv)
{
	Tiel::Log::Init();
	TIEL_CORE_WARN("Initialized Log!");
	TIEL_INFO("Hello!");

	auto app = Tiel::CreateApplication();
	app->Run();
	delete app;

	return 0;
}

#endif