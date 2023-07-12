// --- Core -------------------------------------------------------------------
// Includes all the macros used across SideA.

#pragma once
#include <memory>

#include "SideA/Core/PlatformDetection.h"

// --- Debugs -----------------------------------------------------------------
#ifdef SIDEA_DEBUG
	#define SIDEA_ENABLE_ASSERTS
#endif

#ifdef SIDEA_ENABLE_ASSERTS
	#define SIDEA_ASSERT(x, ...) { if(!(x)) { SIDEA_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
	#define SIDEA_CORE_ASSERT(x, ...) { if(!(x)) { SIDEA_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
	#define SIDEA_ASSERT(x, ...)
	#define SIDEA_CORE_ASSERT(x, ...)
#endif


// --- Utility ----------------------------------------------------------------
#define BIT(x) (1 << x)

#define SIDEA_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)


// --- Memory Management ------------------------------------------------------
namespace SideA
{
	template<typename T>
	using Scope = std::unique_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Scope<T> CreateScope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Ref = std::shared_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}
}