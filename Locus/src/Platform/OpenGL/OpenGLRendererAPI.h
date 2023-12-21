// --- OpenGLRendererAPI ------------------------------------------------------
// OpenGL rendering class. Calls OpenGL commands
#pragma once

#include <glad/glad.h>

#include "Locus/Renderer/RendererAPI.h"

namespace Locus
{
	class OpenGLRendererAPI : public RendererAPI
	{
	public:
		virtual void Init() override;

		virtual void SetClearColor(const glm::vec4 color) override;

		virtual void Clear() override;

		virtual void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) override;

		virtual void DrawLine(const Ref<VertexArray>& vertexArray, uint32_t vertexCount = 0) override;

		virtual void Resize(int x, int y, int width, int height) override;

		virtual void SetLineWidth(float width) override;
	};

}
