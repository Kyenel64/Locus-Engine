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

		static void Flush();

		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);
		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));

		static void DrawRotatedQuad(const glm::vec2& position, float rotation, const glm::vec2& size, const glm::vec4& color);
		static void DrawRotatedQuad(const glm::vec3& position, float rotation, const glm::vec2& size, const glm::vec4& color);
		static void DrawRotatedQuad(const glm::vec2& position, float rotation, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));
		static void DrawRotatedQuad(const glm::vec3& position, float rotation, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));

		// --- Statistics -----------------------------------------------------
		struct Statistics
		{
			// Render stats
			uint32_t DrawCalls = 0;
			uint32_t QuadCount = 0;

			uint32_t GetTotalVertexCount() { return QuadCount * 4; }
			uint32_t GetTotalIndexCount() { return QuadCount * 6; }

			// Frame stats
			std::chrono::time_point<std::chrono::steady_clock> StartTime;
			std::chrono::time_point<std::chrono::steady_clock> EndTime;
			float FrameTime = 0;
			float FramesPerSecond = 0;
		};
		static void ResetStats();
		static Statistics GetStats();
		static void StatsStartFrame();
		static void StatsEndFrame();

	private:
		static void FlushAndReset();
	};
}