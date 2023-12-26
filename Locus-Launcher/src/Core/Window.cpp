#include "LLpch.h"
#include "Window.h"

#include <GLFW/glfw3.h>

#include "Launcher.h"

namespace LocusLauncher
{
	Window::Window(const std::string& name, uint32_t windowWidth, uint32_t windowHeight)
		: m_WindowName(name), m_WindowWidth(windowWidth), m_WindowHeight(windowHeight)
	{
		Init();
	}

	Window::~Window()
	{
		glfwTerminate();
		m_NativeWindow = nullptr;
	}

	bool Window::Init()
	{
		if (!glfwInit())
			return false;

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		m_NativeWindow = glfwCreateWindow(m_WindowWidth, m_WindowHeight, m_WindowName.c_str(), nullptr, nullptr);
		if (!m_NativeWindow)
		{
			glfwTerminate();
			return false;
		}
		glfwMakeContextCurrent(m_NativeWindow);

		CenterGLFWWindow();

		return true;
	}

	void Window::OnUpdate()
	{
		if (glfwWindowShouldClose(m_NativeWindow))
			Launcher::Get().Close();
		glfwPollEvents();
		glfwSwapBuffers(m_NativeWindow);
	}

	void Window::CenterGLFWWindow()
	{
		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);
		glfwSetWindowPos(m_NativeWindow, mode->width / 2 - (m_WindowWidth / 2), mode->height / 2 - (m_WindowHeight / 2));
	}
}