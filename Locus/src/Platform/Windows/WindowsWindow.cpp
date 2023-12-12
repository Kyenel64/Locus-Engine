#include "Lpch.h"
#include "WindowsWindow.h"

#include <stb_image.h>

#include "Locus/Core/Input.h"
#include "Locus/Events/ApplicationEvent.h"
#include "Locus/Events/MouseEvent.h"
#include "Locus/Events/KeyEvent.h"
#include "Locus/Events/Event.h"
#include "Locus/Renderer/Renderer.h"

namespace Locus
{
	static uint8_t s_GLFWWindowCount = 0;

	static void GLFWErrorCallback(int error, const char* description)
	{
		LOCUS_CORE_ERROR("GLFW Error {{0}}: {1}", error, description);
	}

	WindowsWindow::WindowsWindow(const WindowProps& props)
	{
		LOCUS_PROFILE_FUNCTION();

		Init(props);
	}

	WindowsWindow::~WindowsWindow()
	{
		LOCUS_PROFILE_FUNCTION();

		Shutdown();
	}

	void WindowsWindow::Init(const WindowProps& props)
	{
		LOCUS_PROFILE_FUNCTION();

		// --- Initialize data, and GLFW --------------------------------------s
		m_Data.Title = props.Title;
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;
		LOCUS_CORE_INFO("Create Window {0} ({1}, {2})", props.Title, props.Width, props.Height);
		if (s_GLFWWindowCount == 0)
		{
			LOCUS_PROFILE_SCOPE("glfwInit");
			int success = glfwInit();
			LOCUS_CORE_ASSERT(success, "Could not initialize GLFW");
			glfwSetErrorCallback(GLFWErrorCallback);
		}

		// --- Create window and initialize renderer context ------------------
		{
			LOCUS_PROFILE_SCOPE("glfwCreateWindow");
			#if defined(LOCUS_DEBUG)
			if (Renderer::GetAPI() == RendererAPI::API::OpenGL)
				glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
			#endif
			m_Window = glfwCreateWindow((int)props.Width, (int)props.Height, m_Data.Title.c_str(), nullptr, nullptr);
			glfwMaximizeWindow(m_Window);
			glfwGetWindowSize(m_Window, &(int&)m_Data.Width, &(int&)m_Data.Height); // Set window size after maximizing window
		}
		m_Context = GraphicsContext::Create(m_Window);
		m_Context->Init();
		++s_GLFWWindowCount;

		glfwSetWindowUserPointer(m_Window, &m_Data);
		SetVSync(m_Data.VSync);

		// Set window icon
		GLFWimage icons[1];
		icons[0].pixels = stbi_load("resources/icons/LocusLogoIcon.png", &icons[0].width, &icons[0].height, 0, 4);
		glfwSetWindowIcon(m_Window, 1, icons);
		stbi_image_free(icons[0].pixels);

		// --- Set GLFW callbacks ---------------------------------------------
		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
		{
			// Sets window data to glfw callback data.
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			data.Width = width;
			data.Height = height;

			// Create a Locus event
			WindowResizeEvent event(width, height);
			// Calls OnEvent(event)
			data.EventCallback(event);
		});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			WindowCloseEvent event;
			data.EventCallback(event);
		});

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action)
			{
				case GLFW_PRESS:
				{
					KeyPressedEvent event(key, 0);
					data.EventCallback(event);
					Input::SetKeyState(key, KeyState::Pressed);
					break;
				}
				case GLFW_RELEASE:
				{
					KeyReleasedEvent event(key);
					data.EventCallback(event);
					Input::SetKeyState(key, KeyState::Released);
					break;
				}
			}
		});

		glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int keycode)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			KeyTypedEvent event(keycode);
			data.EventCallback(event);
		});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action)
			{
				case GLFW_PRESS:
				{
					MouseButtonPressedEvent event(button);
					data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					MouseButtonReleasedEvent event(button);
					data.EventCallback(event);
					break;
				}
			}
		});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseScrolledEvent event((float)xOffset, (float)yOffset);
			data.EventCallback(event);
		});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				MouseMovedEvent event((float)xPos, (float)yPos);
				data.EventCallback(event);
		});

	}

	void WindowsWindow::Shutdown()
	{
		LOCUS_PROFILE_FUNCTION();

		glfwDestroyWindow(m_Window);

		if (--s_GLFWWindowCount == 0)
		{
			LOCUS_CORE_INFO("Terminating GLFW");
			glfwTerminate();
		}	
	}

	void WindowsWindow::OnUpdate()
	{
		LOCUS_PROFILE_FUNCTION();

		glfwPollEvents();
		m_Context->SwapBuffers();
	}

	void WindowsWindow::SetVSync(bool enabled)
	{
		LOCUS_PROFILE_FUNCTION();

		if (enabled)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);

		m_Data.VSync = enabled;
	}

	bool WindowsWindow::IsVSync() const
	{
		return m_Data.VSync;
	}
}
