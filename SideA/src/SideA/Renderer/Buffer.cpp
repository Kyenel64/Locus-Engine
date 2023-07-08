#include "SApch.h"
#include "Buffer.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLBuffer.h"

namespace SideA
{
	Ref<VertexBuffer> VertexBuffer::Create(uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None: SIDEA_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL: return CreateRef<OpenGLVertexBuffer>(size);
		}

		SIDEA_CORE_ASSERT(false, "Unknown Renderer API!");
		return nullptr;
	}

	Ref<VertexBuffer> VertexBuffer::Create(float* vertices, uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None: SIDEA_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL: return CreateRef<OpenGLVertexBuffer>(vertices, size);
		}

		SIDEA_CORE_ASSERT(false, "Unknown Renderer API!");
		return nullptr;
	}

	Ref<IndexBuffer> IndexBuffer::Create(uint32_t* indices, uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None: SIDEA_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL: return CreateRef<OpenGLIndexBuffer>(indices, size);
		}

		SIDEA_CORE_ASSERT(false, "Unknown Renderer API!");
		return nullptr;
	}
}
