#include "Lpch.h"
#include "RenderCommand.h"

#include "Locus/Renderer/RendererAPI.h"

namespace Locus
{
	Scope<RendererAPI> RenderCommand::s_RendererAPI = RendererAPI::Create();
}
