// --- ScriptableEntity -------------------------------------------------------
// Scriptable entity class. For native scripting. Currently not in use.
#pragma once

#include "Locus/Scene/Entity.h"

namespace Locus
{
	class ScriptableEntity : Entity
	{
	public:
		~ScriptableEntity() = default;

		template <typename T>
		T& GetComponent()
		{
			return m_Entity.GetComponent<T>();
		}

	protected:
		void OnCreate() {}
		void OnDestroy() {}
		void OnUpdate(Timestep deltaTime) {}
	private:
		Entity m_Entity;
		friend class Scene;
	};
}
