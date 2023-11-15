// --- OpenGLContext ----------------------------------------------------------
// OpenGL rendering context.
#pragma once

#include "Locus/Renderer/GraphicsContext.h"

struct GLFWwindow;

namespace Locus
{
	class OpenGLContext : public GraphicsContext
	{
	public:
		OpenGLContext(GLFWwindow* windowHandle);

		virtual void Init() override;
		virtual void SwapBuffers() override;
	private:
		GLFWwindow* m_WindowHandle;
	};
}
