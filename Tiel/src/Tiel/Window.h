// --- Window -----------------------------------------------------------------
// Window interface. Platform specific window class must be implemented.
// Sets event callbacks.

#pragma once

#include "tpch.h"

#include "Tiel/Core.h"
#include "Tiel/Events/Event.h"

namespace Tiel
{
	struct WindowProps
	{
		std::string Title;
		unsigned int Width;
		unsigned int Height;

		WindowProps(const std::string title = "Tiel Engine",
			unsigned int width = 1920,
			unsigned int height = 1080)
			: Title(title), Width(width), Height(height) {}
	};

	// Window class interface for desktops
	class Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		virtual ~Window() {}

		virtual void OnUpdate() = 0;

		virtual unsigned int GetWidth() const = 0;
		virtual unsigned int GetHeight() const = 0;

		// Window attributes
		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsVSync() const = 0;

		virtual void* GetNativeWindow() const = 0;


		static Window* Create(const WindowProps& props = WindowProps());
	};
}