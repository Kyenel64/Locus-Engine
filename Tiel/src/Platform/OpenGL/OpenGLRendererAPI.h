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
		virtual void SetClearColor(const glm::vec4 color) override;

		virtual void Clear() override;

		virtual void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray) override;

		virtual void Resize(int x, int y, int width, int height) override;
	};

}
