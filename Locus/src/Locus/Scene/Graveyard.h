// --- Graveyard --------------------------------------------------------------
// The graveyard contains a separate registry from the main scene which holds
// destroyed entities. The command pattern utilizes this to easily undo and 
// redo entity creation/deletion.
// TODO: If graveyard isnt needed anywhere else, move to editor project.
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