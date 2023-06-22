#include "tpch.h"
#include "RenderCommand.h"

#include "RendererAPI.h"

namespace Tiel
{
	Scope<RendererAPI> RenderCommand::s_RendererAPI = RendererAPI::Create();
}