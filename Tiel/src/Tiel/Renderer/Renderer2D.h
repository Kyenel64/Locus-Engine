// --- Renderer2D -------------------------------------------------------------
// Renderer2D handles all 2D rendering commands including UI elements.
// Init and Shutdown acts like a constructor/destructor for this static class.

#pragma once

#include "OrthographicCamera.h"
#include "Texture.h"

namespace Tiel
{
	class Renderer2D
	{
	public:
		static void Init();
		static void Shutdown();

		static void BeginScene(const OrthographicCamera& camera);
		static void EndScene();

		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);
		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture);
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture);
	};
}