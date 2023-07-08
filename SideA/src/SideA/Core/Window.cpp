#include "SApch.h"
#include "Window.h"

#ifdef SIDEA_PLATFORM_WINDOWS
	#include "Platform/Windows/WindowsWindow.h"
#endif

namespace SideA
{
	Scope<Window> Window::Create(const WindowProps& props)
	{
		#ifdef SIDEA_PLATFORM_WINDOWS
			return CreateScope<WindowsWindow>(props);
		#else
			SIDEA_CORE_ASSERT(false, "Unknown Platform!");
			return nullptr;
		#endif
	}
}