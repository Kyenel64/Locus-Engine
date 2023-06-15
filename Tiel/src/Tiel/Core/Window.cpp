#include "tpch.h"
#include "Window.h"

#ifdef TIEL_PLATFORM_WINDOWS
	#include "Platform/Windows/WindowsWindow.h"
#endif

namespace Tiel
{
	Scope<Window> Window::Create(const WindowProps& props)
	{
		#ifdef TIEL_PLATFORM_WINDOWS
			return CreateScope<WindowsWindow>(props);
		#else
			TIEL_CORE_ASSERT(false, "Unknown Platform!");
			return nullptr;
		#endif
	}
}