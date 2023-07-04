// --- OpenGLRendererAPI ------------------------------------------------------
// OpenGL rendering class. Calls OpenGL commands

#pragma once

#include "Tiel/Renderer/RendererAPI.h"

#include "glad/glad.h"

namespace Tiel
{
	class OpenGLRendererAPI : public RendererAPI
	{
	public:
		virtual void Init() override;

		virtual void SetClearColor(const glm::vec4 color) override;

		virtual void Clear() override;

		virtual void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) override;

		virtual void Resize(int x, int y, int width, int height) override;
	};

}
