#include "tpch.h"
#include "RenderCommand.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace Tiel
{
	RendererAPI* RenderCommand::s_RendererAPI = new OpenGLRendererAPI;
}