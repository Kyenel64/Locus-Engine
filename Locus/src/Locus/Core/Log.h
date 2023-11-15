// --- Log --------------------------------------------------------------------
// Logging class for Locus. Uses spdlog and includes macros for logging.
#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

namespace Locus
{
	class Log
	{
	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetScriptLogger() { return s_ScriptLogger; }

	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
		static std::shared_ptr<spdlog::logger> s_ScriptLogger;

	};
}

// Core log macros
#define LOCUS_CORE_ERROR(...) ::Locus::Log::GetCoreLogger()->error(__VA_ARGS__)
#define LOCUS_CORE_WARN(...)  ::Locus::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define LOCUS_CORE_INFO(...)  ::Locus::Log::GetCoreLogger()->info(__VA_ARGS__)
#define LOCUS_CORE_TRACE(...) ::Locus::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define LOCUS_CORE_CRITICAL(...) ::Locus::Log::GetCoreLogger()->critical(__VA_ARGS__)

// Core log macros
#define LOCUS_SCRIPT_ERROR(...) ::Locus::Log::GetSCRIPTLogger()->error(__VA_ARGS__)
#define LOCUS_SCRIPT_WARN(...)  ::Locus::Log::GetSCRIPTLogger()->warn(__VA_ARGS__)
#define LOCUS_SCRIPT_INFO(...)  ::Locus::Log::GetSCRIPTLogger()->info(__VA_ARGS__)
#define LOCUS_SCRIPT_TRACE(...) ::Locus::Log::GetSCRIPTLogger()->trace(__VA_ARGS__)
#define LOCUS_SCRIPT_CRITICAL(...) ::Locus::Log::GetScriptLogger()->critical(__VA_ARGS__)

// Client log macros
#define LOCUS_ERROR(...) ::Locus::Log::GetClientLogger()->error(__VA_ARGS__)
#define LOCUS_WARN(...)  ::Locus::Log::GetClientLogger()->warn(__VA_ARGS__)
#define LOCUS_INFO(...)  ::Locus::Log::GetClientLogger()->info(__VA_ARGS__)
#define LOCUS_TRACE(...) ::Locus::Log::GetClientLogger()->trace(__VA_ARGS__)
#define LOCUS_CRITICAL(...) ::Locus::Log::GetClientLogger()->critical(__VA_ARGS__)
