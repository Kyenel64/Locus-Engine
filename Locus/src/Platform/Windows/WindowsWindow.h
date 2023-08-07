// --- WindowsWindow ----------------------------------------------------------
// Window class for Windows. Handles GLFW actions
// Sets and calls callback functions from GLFW to the application class.

#pragma once

#include "Locus/Core/Window.h"

#include "Locus/Renderer/GraphicsContext.h"

#include <GLFW/glfw3.h>

namespace Locus
{
	class WindowsWindow : public Window
	{
	public:
		// Calls Init()
		WindowsWindow(const WindowProps& props);
		// Calls Shutdown()
		virtual ~WindowsWindow();

		virtual void OnUpdate() override;

		virtual inline unsigned int GetWidth() const override { return m_Data.Width; }
		virtual inline unsigned int GetHeight() const override { return m_Data.Height; }

		virtual inline float GetTime() const override { return (float)glfwGetTime(); }

		virtual inline void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
		virtual void SetVSync(bool enabled) override;
		virtual bool IsVSync() const override;

		inline virtual void* GetNativeWindow() const { return m_Window; }

	private:
		// Initiates GLFW and glad, sets GLFW callbacks
		void Init(const WindowProps& props);
		void Shutdown();

	private:
		GLFWwindow* m_Window;
		Scope<GraphicsContext> m_Context;

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