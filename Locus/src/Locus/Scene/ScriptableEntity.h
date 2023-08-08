// --- ScriptableEntity -------------------------------------------------------
// Scriptable entity class.
#pragma once

#include "Locus/Scene/Entity.h"

namespace Locus
{
	class ScriptableEntity : Entity
	{
	public:
		virtual ~ScriptableEntity() {}

		template <typename T>
		T& GetComponent()
		{
			return m_Entity.GetComponent<T>();
		}

	protected:
		virtual void OnCreate() {}
		virtual void OnDestroy() {}
		virtual void OnUpdate(Timestep deltaTime) {}
	private:
		Entity m_Entity;
		friend class Scene;
	};
}