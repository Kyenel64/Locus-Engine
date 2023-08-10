#include "Lpch.h"
#include "Entity.h"

namespace Locus
{
	Entity Entity::Null = {};

	Entity::Entity(entt::entity handle, Scene* scene)
		: m_EntityHandle(handle), m_Scene(scene)
	{
	}
}