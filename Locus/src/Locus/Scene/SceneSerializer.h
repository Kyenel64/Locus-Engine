// --- SceneSerializer --------------------------------------------------------
// Serializes and deserializes scene to a given .locus file.
#pragma once

#include "Locus/Scene/Scene.h"

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
		Ref<Scene> m_Scene;
	};
}