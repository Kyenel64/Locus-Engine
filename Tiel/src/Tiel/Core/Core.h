// --- Core -------------------------------------------------------------------
// Includes all the macros used across Tiel.

#pragma once

#ifdef TIEL_PLATFORM_WINDOWS
#if TIEL_DYNAMIC_LINK
	#ifdef TIEL_BUILD_DLL
		#define TIEL_API __declspec(dllexport)

	#else
		#define TIEL_API __declspec(dllimport)
	#endif
#else
	#define TIEL_API
#endif
#else
	#error Tiel only supports Windows
#endif

#ifdef TIEL_DEBUG
	#define TIEL_ENABLE_ASSERTS
#endif

#ifdef TIEL_ENABLE_ASSERTS
	#define TIEL_ASSERT(x, ...) { if(!(x)) { TIEL_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
	#define TIEL_CORE_ASSERT(x, ...) { if(!(x)) { TIEL_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
	#define TIEL_ASSERT(x, ...)
	#define TIEL_CORE_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)

#define TIEL_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)