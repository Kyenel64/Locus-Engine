#include "Lpch.h"
#include "Framebuffer.h"

#include "Locus/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLFramebuffer.h"

namespace Locus
{
	Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification& specs)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None: LOCUS_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL: return CreateRef<OpenGLFramebuffer>(specs);
		}

		LOCUS_CORE_ASSERT(false, "Unknown Renderer API!");
		return nullptr;
	}
}
