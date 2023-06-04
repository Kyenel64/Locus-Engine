// --- Renderer ---------------------------------------------------------------
// Main high-level renderer class. 
// Submit draws VertexArray onto context

#pragma once

#include "RenderCommand.h"

namespace Tiel
{
	class Renderer
	{
	public:
		static void BeginScene();

		static void EndScene();

		static void Submit(const std::shared_ptr<VertexArray>& vertexArray);

		inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }

	};

}