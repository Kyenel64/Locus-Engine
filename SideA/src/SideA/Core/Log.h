// --- Log --------------------------------------------------------------------
// Logging class for SideA. Uses spdlog and includes macros for logging.

#pragma once

#include "Core.h"

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

namespace SideA
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
#define SIDEA_CORE_ERROR(...) ::SideA::Log::GetCoreLogger()->error(__VA_ARGS__)
#define SIDEA_CORE_WARN(...)  ::SideA::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define SIDEA_CORE_INFO(...)  ::SideA::Log::GetCoreLogger()->info(__VA_ARGS__)
#define SIDEA_CORE_TRACE(...) ::SideA::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define SIDEA_CORE_CRITICAL(...) ::SideA::Log::GetCoreLogger()->critical(__VA_ARGS__)

// Client log macros
#define SIDEA_ERROR(...) ::SideA::Log::GetClientLogger()->error(__VA_ARGS__)
#define SIDEA_WARN(...)  ::SideA::Log::GetClientLogger()->warn(__VA_ARGS__)
#define SIDEA_INFO(...)  ::SideA::Log::GetClientLogger()->info(__VA_ARGS__)
#define SIDEA_TRACE(...) ::SideA::Log::GetClientLogger()->trace(__VA_ARGS__)
#define SIDEA_CRITICAL(...) ::SideA::Log::GetClientLogger()->critical(__VA_ARGS__)