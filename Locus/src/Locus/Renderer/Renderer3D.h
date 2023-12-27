#pragma once

#include "Locus/Renderer/EditorCamera.h"

namespace Locus
{
	class Renderer3D
	{
	public:
		static void Init();
		static void Shutdown();

		static void BeginScene(const EditorCamera& camera);
		static void EndScene();

		static void StartBatch();
		static void Flush();

		static void FlushAndReset();

		static void DrawCube(const glm::mat4& transform, const glm::vec4& color, int entityID);

		static void DrawGrid();
	};
}