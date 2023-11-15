#include "Lpch.h"
#include "Window.h"

#ifdef LOCUS_PLATFORM_WINDOWS
	#include "Platform/Windows/WindowsWindow.h"
#endif

namespace Locus
{
	Scope<Window> Window::Create(const WindowProps& props)
	{
		#ifdef LOCUS_PLATFORM_WINDOWS
			return CreateScope<WindowsWindow>(props);
		#else
			LOCUS_CORE_ASSERT(false, "Unknown Platform!");
			return nullptr;
		#endif
	}
}
