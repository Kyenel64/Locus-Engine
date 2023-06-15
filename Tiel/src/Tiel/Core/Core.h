// --- Core -------------------------------------------------------------------
// Includes all the macros used across Tiel.

#pragma once

#include <memory>

// Platform detection using predefined macros
#ifdef _WIN32
	#ifdef _WIN64
		#define TIEL_PLATFORM_WINDOWS
	#else
		#error "x86 builds are not supported!"
	#endif
#elif defined(__APPLE__) || defined(__MACH__)
	#include <TargetConditionals.h>
	#if TARGET_IPHONE_SIMULATOR == 1
		#error "IOS simulator is not supported!"
	#elif TARGET_OS_IPHONE == 1
		#define TIEL_PLATFORM_IOS
		#error "IOS is not supported!"
	#elif TARGET_OS_MAC == 1
		#define TIEL_PLATFORM_MACOS
		#error "MacOS is not supported!"
	#else
		#error "Unknown Apple platform!"
	#endif
#elif defined(__ANDROID__)
	#define TIEL_PLATFORM_ANDROID
	#error "Android is not supported!"
#elif defined(__linux__)
	#define TIEL_PLATFORM_LINUX
	#error "Linux is not supported!"
#else
	#error "Unknown platform!"
#endif // End of platform detection


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

namespace Tiel
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