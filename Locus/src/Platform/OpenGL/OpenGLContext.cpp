#include "Lpch.h"
#include "OpenGLContext.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Locus
{
	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle) : m_WindowHandle(windowHandle) 
	{
		LOCUS_CORE_ASSERT(windowHandle, "Window handle is null!")
	}

	void OpenGLContext::Init()
	{
		LOCUS_PROFILE_FUNCTION();

		glfwMakeContextCurrent(m_WindowHandle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		LOCUS_CORE_ASSERT(status, "Failed to initialize Glad");

		LOCUS_CORE_INFO("OpenGL Info: ");
		LOCUS_CORE_INFO("  Vendor: {0}", (const char*)glGetString(GL_VENDOR));
		LOCUS_CORE_INFO("  Renderer: {0}", (const char*)glGetString(GL_RENDERER));
		LOCUS_CORE_INFO("  Version : {0}", (const char*)glGetString(GL_VERSION));

		LOCUS_CORE_ASSERT(GLVersion.major > 4 || (GLVersion.major == 4 && GLVersion.minor >= 5),
			"Side A requires atleast OpenGL version 4.5!");
	}

	void OpenGLContext::SwapBuffers()
	{
		LOCUS_PROFILE_FUNCTION();

		glfwSwapBuffers(m_WindowHandle);
	}
}
