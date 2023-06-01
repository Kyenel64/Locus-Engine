// --- WindowsWindow ----------------------------------------------------------
// Window class for Windows. Handles GLFW actions
// Sets and calls callback functions from GLFW to the application class.

#pragma once

#include "Tiel/Window.h"

#include "Tiel/Renderer/GraphicsContext.h"

#include <GLFW/glfw3.h>

namespace Tiel
{
	class WindowsWindow : public Window
	{
	public:
		// Calls Init()
		WindowsWindow(const WindowProps& props);
		// Calls Shutdown()
		virtual ~WindowsWindow();

		void OnUpdate() override;

		inline unsigned int GetWidth() const override { return m_Data.Width; }
		inline unsigned int GetHeight() const override { return m_Data.Height; }

		// Window attributes
		inline void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
		void SetVSync(bool enabled) override;
		bool IsVSync() const override;

		inline virtual void* GetNativeWindow() const { return m_Window; }

	private:
		// Initiates GLFW and glad, sets GLFW callbacks
		virtual void Init(const WindowProps& props);
		virtual void Shutdown();

	private:
		GLFWwindow* m_Window;
		GraphicsContext* m_Context;

		struct WindowData
		{
			std::string Title;
			unsigned int Width = 1920;
			unsigned int Height = 1080;
			bool VSync = true;

			EventCallbackFn EventCallback;
		};

		WindowData m_Data;
	};
}