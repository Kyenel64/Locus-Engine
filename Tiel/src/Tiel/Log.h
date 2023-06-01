// --- Log --------------------------------------------------------------------
// Logging class for Tiel. Uses spdlog and includes macros for logging.

#pragma once

#include "Core.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace Tiel
{
	class Log
	{
	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }

	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;

	};
}


// Core log macros
#define TIEL_CORE_ERROR(...) ::Tiel::Log::GetCoreLogger()->error(__VA_ARGS__)
#define TIEL_CORE_WARN(...)  ::Tiel::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define TIEL_CORE_INFO(...)  ::Tiel::Log::GetCoreLogger()->info(__VA_ARGS__)
#define TIEL_CORE_TRACE(...) ::Tiel::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define TIEL_CORE_FATAL(...) ::Tiel::Log::GetCoreLogger()->fatal(__VA_ARGS__)

// Client log macros
#define TIEL_ERROR(...) ::Tiel::Log::GetClientLogger()->error(__VA_ARGS__)
#define TIEL_WARN(...)  ::Tiel::Log::GetClientLogger()->warn(__VA_ARGS__)
#define TIEL_INFO(...)  ::Tiel::Log::GetClientLogger()->info(__VA_ARGS__)
#define TIEL_TRACE(...) ::Tiel::Log::GetClientLogger()->trace(__VA_ARGS__)
#define TIEL_FATAL(...) ::Tiel::Log::GetClientLogger()->fatal(__VA_ARGS__)