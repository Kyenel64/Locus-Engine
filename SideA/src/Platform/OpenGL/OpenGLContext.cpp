#include "SApch.h"
#include "OpenGLContext.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"


namespace SideA
{
	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle) : m_WindowHandle(windowHandle) 
	{
		SIDEA_CORE_ASSERT(windowHandle, "Window handle is null!")
	}

	void OpenGLContext::Init()
	{
		SIDEA_PROFILE_FUNCTION();

		glfwMakeContextCurrent(m_WindowHandle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		SIDEA_CORE_ASSERT(status, "Failed to initialize Glad");

		SIDEA_CORE_INFO("OpenGL Info: ");
		SIDEA_CORE_INFO("  Vendor: {0}", (const char*)glGetString(GL_VENDOR));
		SIDEA_CORE_INFO("  Renderer: {0}", (const char*)glGetString(GL_RENDERER));
		SIDEA_CORE_INFO("  Version : {0}", (const char*)glGetString(GL_VERSION));

		#ifdef SIDEA_ENABLE_ASSERTS
			int versionMajor;
			int versionMinor;
			glGetIntegerv(GL_MAJOR_VERSION, &versionMajor);
			glGetIntegerv(GL_MINOR_VERSION, &versionMinor);
			SIDEA_CORE_ASSERT(versionMajor > 4 || (versionMajor == 4 && versionMinor >= 5),
				"SideA requires OpenGL version 4.5 or above.");
		#endif
	}

	void OpenGLContext::SwapBuffers()
	{
		SIDEA_PROFILE_FUNCTION();

		glfwSwapBuffers(m_WindowHandle);
	}

}