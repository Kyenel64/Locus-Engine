// --- Renderer ---------------------------------------------------------------
// Renderer class. Handles basic system functions for all rendering systems
//	(2D & 3D).
#pragma once

#include "Locus/Renderer/RenderCommand.h"

namespace Locus
{
	class Renderer
	{
	public:
		static void Init();
		static void Shutdown();

		static void OnWindowResize(uint32_t width, uint32_t height);

		static void EndScene();

		inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
	};
}
