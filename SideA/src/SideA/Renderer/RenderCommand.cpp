#include "SApch.h"
#include "RenderCommand.h"

#include "RendererAPI.h"

namespace SideA
{
	Scope<RendererAPI> RenderCommand::s_RendererAPI = RendererAPI::Create();
}