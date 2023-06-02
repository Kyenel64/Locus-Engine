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

		TIEL_CORE_INFO("OpenGL Info: ");
		TIEL_CORE_INFO("  Vendor: {0}", (const char*)glGetString(GL_VENDOR));
		TIEL_CORE_INFO("  Renderer: {0}", (const char*)glGetString(GL_RENDERER));
		TIEL_CORE_INFO("  Version : {0}", (const char*)glGetString(GL_VERSION));
	}

	void OpenGLContext::SwapBuffers()
	{
		glfwSwapBuffers(m_WindowHandle);
	}
}