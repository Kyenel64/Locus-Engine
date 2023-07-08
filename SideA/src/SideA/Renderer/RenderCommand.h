// --- RenderCommand ----------------------------------------------------------
// Interface for rendering commands. Implementation sets s_RendererAPI to 
// a specific rendering API.

#pragma once

#include "RendererAPI.h"

namespace SideA
{
	class RenderCommand
	{
	public:
		inline static void Init()
		{
			s_RendererAPI->Init();
		}

		inline static void SetClearColor(const glm::vec4& color) 
		{
			s_RendererAPI->SetClearColor(color);
		}

		inline static void Clear()
		{
			s_RendererAPI->Clear();
		}

		inline static void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t count = 0)
		{
			s_RendererAPI->DrawIndexed(vertexArray, count);
		}

		inline static void Resize(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
		{
			s_RendererAPI->Resize(x, y, width, height);
		}

	private:
		static Scope<RendererAPI> s_RendererAPI;
	};
}
