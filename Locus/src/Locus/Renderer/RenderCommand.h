// --- RenderCommand ----------------------------------------------------------
// Interface for rendering commands. Implementation is renderer API dependent.
#pragma once

#include "Locus/Renderer/RendererAPI.h"

namespace Locus
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

		inline static void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0)
		{
			s_RendererAPI->DrawIndexed(vertexArray, indexCount);
		}

		inline static void DrawArray(const Ref<VertexArray>& vertexArray, uint32_t vertexCount = 0)
		{
			s_RendererAPI->DrawArray(vertexArray, vertexCount);
		}

		inline static void DrawIndexedInstanced(const Ref<VertexArray>& vertexArray, uint32_t indexCount, uint32_t instanceCount, uint32_t instanceBase = 0)
		{
			s_RendererAPI->DrawIndexedInstanced(vertexArray, indexCount, instanceCount, instanceBase);
		}

		inline static void DrawArrayInstanced(const Ref<VertexArray>& vertexArray, uint32_t vertexCount, uint32_t instanceCount, uint32_t instanceBase = 0)
		{
			s_RendererAPI->DrawArrayInstanced(vertexArray, vertexCount, instanceCount, instanceBase);
		}

		inline static void DrawLine(const Ref<VertexArray>& vertexArray, uint32_t vertexCount = 0)
		{
			s_RendererAPI->DrawLine(vertexArray, vertexCount);
		}

		inline static void Resize(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
		{
			s_RendererAPI->Resize(x, y, width, height);
		}

		inline static void SetLineWidth(float width)
		{
			s_RendererAPI->SetLineWidth(width);
		}

	private:
		static Scope<RendererAPI> s_RendererAPI;
	};
}
