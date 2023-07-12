#pragma once

#include "SideA/Scene/Entity.h"

namespace SideA
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