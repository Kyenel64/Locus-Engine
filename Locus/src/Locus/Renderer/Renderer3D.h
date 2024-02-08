#pragma once

#include "Locus/Renderer/EditorCamera.h"
#include "Locus/Scene/Components.h"
#include "Locus/Renderer/Model.h"
#include "Locus/Renderer/Material.h"

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
		static void DrawCube(const glm::mat4& transform, Ref<Material> material, int entityID);
		static void DrawModel(const glm::mat4& transform, Ref<VertexArray> va, Ref<Material> material, int entityID);

		static void DrawCubeMask(const glm::mat4& transform, Ref<Shader> shader);
		static void DrawGrid();

		static uint32_t GetMaxInstances();

	private:
		static int ProcessTextureSlot(Ref<Texture2D> texture);
		static int ProcessMaterialSlot(Ref<Material> material, int albedoIndex, int normalIndex, int metallicIndex, int roughnessIndex, int aoIndex);
	};
}