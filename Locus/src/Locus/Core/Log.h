// --- Log --------------------------------------------------------------------
// Logging class for Locus. Uses spdlog and includes macros for logging.
#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

#include "Locus/Core/Core.h"

namespace Locus
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
#define LOCUS_CORE_ERROR(...) ::Locus::Log::GetCoreLogger()->error(__VA_ARGS__)
#define LOCUS_CORE_WARN(...)  ::Locus::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define LOCUS_CORE_INFO(...)  ::Locus::Log::GetCoreLogger()->info(__VA_ARGS__)
#define LOCUS_CORE_TRACE(...) ::Locus::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define LOCUS_CORE_CRITICAL(...) ::Locus::Log::GetCoreLogger()->critical(__VA_ARGS__)

// Client log macros
#define LOCUS_ERROR(...) ::Locus::Log::GetClientLogger()->error(__VA_ARGS__)
#define LOCUS_WARN(...)  ::Locus::Log::GetClientLogger()->warn(__VA_ARGS__)
#define LOCUS_INFO(...)  ::Locus::Log::GetClientLogger()->info(__VA_ARGS__)
#define LOCUS_TRACE(...) ::Locus::Log::GetClientLogger()->trace(__VA_ARGS__)
#define LOCUS_CRITICAL(...) ::Locus::Log::GetClientLogger()->critical(__VA_ARGS__)