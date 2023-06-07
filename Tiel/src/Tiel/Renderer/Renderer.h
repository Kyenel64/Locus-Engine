// --- Renderer ---------------------------------------------------------------
// Main high-level renderer class. 
// Submit draws Vertex Arrays with given shader class.
// Stores a SceneData struct that holds scene specific data.

#pragma once

#include "RenderCommand.h"
#include "OrthographicCamera.h"
#include "Shader.h"

namespace Tiel
{
	class Renderer
	{
	public:
		static void BeginScene(OrthographicCamera& camera);

		static void EndScene();

		static void Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray);

		inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }


	private:
		struct SceneData
		{
			glm::mat4 ViewProjectionMatrix;
		};

		static SceneData* m_SceneData;
	};

}