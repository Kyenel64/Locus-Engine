#include "tpch.h"
#include "OpenGLContext.h"

#include "GLFW/glfw3.h"
#include "glad/glad.h"


namespace Tiel
{
	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle) : m_WindowHandle(windowHandle) 
	{
		TIEL_CORE_ASSERT(windowHandle, "Window handle is null!")
	}

	void OpenGLContext::Init()
	{
		glfwMakeContextCurrent(m_WindowHandle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		TIEL_CORE_ASSERT(status, "Failed to initialize Glad");
	}

	void OpenGLContext::SwapBuffers()
	{
		glfwSwapBuffers(m_WindowHandle);
	}
}