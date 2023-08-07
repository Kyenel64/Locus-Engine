#include "Lpch.h"
#include "Entity.h"

namespace Locus
{
	Entity::Entity(entt::entity handle, Scene* scene)
		: m_EntityHandle(handle), m_Scene(scene)
	{
	}
}