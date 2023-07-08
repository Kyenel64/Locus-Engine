#include "SApch.h"
#include "GraphicsContext.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLContext.h"

namespace SideA
{
	Scope<GraphicsContext> GraphicsContext::Create(void* window)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None: SIDEA_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL: return CreateScope<OpenGLContext>(static_cast<GLFWwindow*>(window));
		}

		SIDEA_CORE_ASSERT(false, "Unknown Renderer API!");
		return nullptr;
	}
}