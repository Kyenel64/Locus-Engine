#include "tpch.h"
#include "Shader.h"
#include "Renderer.h"

#include "Platform/OpenGL/OpenGLShader.h"


namespace Tiel
{
	Shader* Shader::Create(const std::string vertexSrc, const std::string fragmentSrc)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None: TIEL_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL: return new OpenGLShader(vertexSrc, fragmentSrc);
		}

		TIEL_CORE_ASSERT(false, "Unknown Renderer API!");
		return nullptr;
	}
}
