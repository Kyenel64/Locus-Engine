// --- Window -----------------------------------------------------------------
// Window interface. Platform specific window class must be implemented.
// Sets event callbacks.
#pragma once

#include <sstream>

#include "Locus/Core/Core.h"
#include "Locus/Events/Event.h"

namespace Locus
{
	struct WindowProps
	{
		std::string Title;
		uint32_t Width;
		uint32_t Height;

		WindowProps(const std::string title = "Locus Engine",
			uint32_t width = 1920,
			uint32_t height = 1080)
			: Title(title), Width(width), Height(height) {}
	};

	// Window class interface for desktops
	class Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		virtual ~Window() {}

		virtual void OnUpdate() = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual float GetTime() const = 0;

		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsVSync() const = 0;

		virtual void* GetNativeWindow() const = 0;


		static Scope<Window> Create(const WindowProps& props = WindowProps());
	};
}