// --- SceneSerializer --------------------------------------------------------
// Serializes and deserializes scene to a given .locus file.
#pragma once

#include "Locus/Scene/Scene.h"

namespace YAML
{
	class Emitter;
}

namespace Locus
{
	class SceneSerializer
	{
	public:
		SceneSerializer(const Ref<Scene>& scene);
		
		void Serialize(const std::string& path);
		void SerializeRunTime(const std::string& path);

		bool Deserialize(const std::string& path);
		bool DeserializeRunTime(const std::string& path);

	private:
		void SerializeEntity(YAML::Emitter& out, Entity entity);

	private:
		Ref<Scene> m_Scene;
	};
}