// --- Renderer ---------------------------------------------------------------
// General renderer class for managing Renderer2D & Renderer3D. 
// Also manages globally used uniform buffers.
#pragma once

#include "Locus/Renderer/RenderCommand.h"
#include "Locus/Renderer/EditorCamera.h"
#include "Locus/Scene/SceneCamera.h"

namespace Locus
{
	class Renderer
	{
	public:
		static void Init();
		static void Shutdown();

		// Editor: Sets UBO's that are used globally and clears framebuffer
		static void BeginScene(const EditorCamera& camera);
		// Runtime: Sets UBO's that are used globally and clears framebuffer
		static void BeginScene(const SceneCamera& camera, const glm::mat4& transform);

		static void EndScene();

		static void OnWindowResize(uint32_t width, uint32_t height);

		inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
	};
}
