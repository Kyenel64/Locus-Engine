#include "SApch.h"
#include "Entity.h"

namespace SideA
{
	Entity::Entity(entt::entity handle, Scene* scene)
		: m_EntityHandle(handle), m_Scene(scene)
	{
	}
}