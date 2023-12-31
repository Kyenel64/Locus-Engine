#pragma once

#include "Locus/Renderer/EditorCamera.h"
#include "Locus/Scene/Components.h"
#include "Locus/Renderer/Model.h"

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

		// TODO: Take in optional shader for custom shaders
		static void DrawCube(const glm::mat4& transform, const CubeRendererComponent& crc, int entityID);

		static void DrawModel(const glm::mat4& transform, Ref<Model> model, int entityID);

		static void DrawCubeMask(const glm::mat4& transform);
		static void DrawGrid();
	};
}