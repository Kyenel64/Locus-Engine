// --- Core -------------------------------------------------------------------
// Macros and memory management
#pragma once

#include <memory>

#include "Locus/Core/PlatformDetection.h"

// --- Debugs -----------------------------------------------------------------
#ifdef LOCUS_DEBUG
	#define LOCUS_ENABLE_ASSERTS
#endif

#ifdef LOCUS_ENABLE_ASSERTS
	#define LOCUS_ASSERT(x, ...) { if(!(x)) { LOCUS_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
	#define LOCUS_CORE_ASSERT(x, ...) { if(!(x)) { LOCUS_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
	#define LOCUS_ASSERT(x, ...)
	#define LOCUS_CORE_ASSERT(x, ...)
#endif


// --- Utility ----------------------------------------------------------------
#define BIT(x) (1 << x)

#define LOCUS_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)

// Binds function into std::function. Works with member functions.
// Pass in function and types of parameters.
#define LOCUS_BIND_FN(func, ...) std::function([&](__VA_ARGS__ val) { func(val); })

// --- Memory Management ------------------------------------------------------
namespace Locus
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