// --- RenderCommand ----------------------------------------------------------
// Interface for rendering commands. Implementation sets s_RendererAPI to 
// a specific rendering API.

#pragma once

#include "RendererAPI.h"
namespace Tiel
{
	class RenderCommand
	{
	public:
		inline static void SetClearColor(const glm::vec4& color) 
		{
			return s_RendererAPI->SetClearColor(color);
		}

		inline static void Clear()
		{
			s_RendererAPI->Clear();
		}

		inline static void DrawIndexed(const Ref<VertexArray>& vertexArray)
		{
			s_RendererAPI->DrawIndexed(vertexArray);
		}

		inline static void Resize(int x, int y, int width, int height)
		{
			s_RendererAPI->Resize(x, y, width, height);
		}

	private:
		static RendererAPI* s_RendererAPI;
	};
}
