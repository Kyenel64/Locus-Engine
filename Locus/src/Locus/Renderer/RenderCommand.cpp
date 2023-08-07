#include "Lpch.h"
#include "RenderCommand.h"

#include "RendererAPI.h"

namespace Locus
{
	Scope<RendererAPI> RenderCommand::s_RendererAPI = RendererAPI::Create();
}