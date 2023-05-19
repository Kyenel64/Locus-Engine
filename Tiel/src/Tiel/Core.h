#pragma once

#ifdef TIEL_PLATFORM_WINDOWS
	#ifdef TIEL_BUILD_DLL
		#define TIEL_API __declspec(dllexport)

	#else
		#define TIEL_API __declspec(dllimport)
	#endif
#else
	#error Tiel only supports Windows
#endif

#define BIT(x) (1 << x)