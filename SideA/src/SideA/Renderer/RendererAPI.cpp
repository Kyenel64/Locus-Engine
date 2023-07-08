#include "SApch.h"
#include "RendererAPI.h"

#include "Renderer.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace SideA
{
	RendererAPI::API RendererAPI::s_API = RendererAPI::API::OpenGL;

	Scope<RendererAPI> RendererAPI::Create()
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None: SIDEA_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL: return CreateScope<OpenGLRendererAPI>();
		}

		SIDEA_CORE_ASSERT(false, "Unknown Renderer API!");
		return nullptr;
	}
}
