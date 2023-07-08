#include "SApch.h"
#include "VertexArray.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"

namespace SideA
{
	Ref<VertexArray> VertexArray::Create()
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None: SIDEA_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL: return CreateRef<OpenGLVertexArray>();
		}

		SIDEA_CORE_ASSERT(false, "Unknown Renderer API!");
		return nullptr;
	}
}