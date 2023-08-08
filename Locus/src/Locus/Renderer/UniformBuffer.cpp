#include "Lpch.h"
#include "UniformBuffer.h"

#include "Locus/Renderer/Renderer.h"

#include "Platform/OpenGL/OpenGLUniformBuffer.h"

namespace Locus
{
	Ref<UniformBuffer> UniformBuffer::Create(uint32_t size, uint32_t binding)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None: LOCUS_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL: return CreateRef<OpenGLUniformBuffer>(size, binding);
		}

		LOCUS_CORE_ASSERT(false, "Unknown Renderer API!");
		return nullptr;
	}
}