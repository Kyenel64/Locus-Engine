#include "SApch.h"
#include "Texture.h"

#include "Renderer.h"

#include "Platform/OpenGL/OpenGLTexture.h"

namespace SideA
{
	Ref<Texture2D> Texture2D::Create(uint32_t width, uint32_t height)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: SIDEA_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL: return CreateRef<OpenGLTexture2D>(width, height);
		}

		SIDEA_CORE_ASSERT(false, "Unknown Renderer API!");
		return nullptr;
	}

	Ref<Texture2D> Texture2D::Create(const std::string& path)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: SIDEA_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL: return CreateRef<OpenGLTexture2D>(path);
		}

		SIDEA_CORE_ASSERT(false, "Unknown Renderer API!");
		return nullptr;
	}
}