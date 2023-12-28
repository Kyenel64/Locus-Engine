#pragma once

#include "Locus/Renderer/EditorCamera.h"
#include "Locus/Scene/Components.h"

namespace Locus
{
	class Scene;

	class Renderer3D
	{
	public:
		static void Init();
		static void Shutdown();

		static void BeginScene(const EditorCamera& camera, Scene* scene);
		static void BeginScene(const Camera& camera, const glm::mat4& transform, Scene* scene);
		static void EndScene();

		static void StartBatch();
		static void Flush();

		static void FlushAndReset();

		static void DrawCube(const glm::mat4& transform, const CubeRendererComponent& crc, int entityID);

		static void DrawGrid();
	};
}