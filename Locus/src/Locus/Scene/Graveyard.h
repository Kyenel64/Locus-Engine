#pragma once

#include <entt.hpp>

namespace Locus
{
	class Entity;
	class Scene;

	class Graveyard
	{
	public:
		Graveyard() = default;
		~Graveyard() = default;

		entt::entity AddEntity(Entity entity);
		Entity MoveEntityToScene(entt::entity entity, Ref<Scene> scene);
	private:
		entt::registry m_Registry;

		friend class DestroyEntityCommand;
	};
}