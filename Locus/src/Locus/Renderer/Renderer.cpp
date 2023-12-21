#include "Lpch.h"
#include "Renderer.h"

#include "Locus/Renderer/Renderer2D.h"

namespace Locus
{
	void Renderer::Init()
	{
		LOCUS_PROFILE_FUNCTION();

		RenderCommand::Init();
		Renderer2D::Init();
	}

	void Renderer::Shutdown()
	{
		Renderer2D::Shutdown();
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		RenderCommand::Resize(0, 0, width, height);
	}

	void Renderer::EndScene()
	{

	}
}
