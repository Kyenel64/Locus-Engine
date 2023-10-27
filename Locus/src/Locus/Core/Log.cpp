#include "Lpch.h"
#include "Log.h"

#include <spdlog/sinks/stdout_color_sinks.h>

namespace Locus
{

	std::shared_ptr<spdlog::logger> Log::s_CoreLogger;
	std::shared_ptr<spdlog::logger> Log::s_ClientLogger;
	std::shared_ptr<spdlog::logger> Log::s_ScriptLogger;
	

	void Log::Init()
	{
		spdlog::set_pattern("%^[%T] %n: %v%$");
		s_CoreLogger = spdlog::stdout_color_mt("LOCUS");
		s_CoreLogger->set_level(spdlog::level::trace);

		s_ClientLogger = spdlog::stdout_color_mt("APP");
		s_ClientLogger->set_level(spdlog::level::trace);

		s_ScriptLogger = spdlog::stdout_color_mt("SCRIPT");
		s_ScriptLogger->set_level(spdlog::level::trace);

	}
}
