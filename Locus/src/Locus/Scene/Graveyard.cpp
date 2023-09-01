#include "Lpch.h"
#include "Graveyard.h"

#include "Locus/Scene/Entity.h"
#include "Locus/Scene/Components.h"

namespace Locus
{
	entt::entity Graveyard::AddEntity(Entity entity)
	{
		auto newEntity = m_Registry.create(entity);
		LOCUS_CORE_ASSERT((entt::entity)entity == newEntity, "Entity ID does not match entt ID!");
		m_Registry.emplace_or_replace<TagComponent>(newEntity, entity.GetComponent<TagComponent>());
		m_Registry.emplace_or_replace<IDComponent>(newEntity, entity.GetComponent<IDComponent>());
		m_Registry.emplace_or_replace<RelationshipComponent>(newEntity, entity.GetComponent<RelationshipComponent>());
		m_Registry.emplace_or_replace<TransformComponent>(newEntity, entity.GetComponent<TransformComponent>());

		if (entity.HasComponent<SpriteRendererComponent>())
			m_Registry.emplace_or_replace<SpriteRendererComponent>(newEntity, entity.GetComponent<SpriteRendererComponent>());
		if (entity.HasComponent<CameraComponent>())
			m_Registry.emplace_or_replace<CameraComponent>(newEntity, entity.GetComponent<CameraComponent>());
		if (entity.HasComponent<Rigidbody2DComponent>())
			m_Registry.emplace_or_replace<Rigidbody2DComponent>(newEntity, entity.GetComponent<Rigidbody2DComponent>());
		if (entity.HasComponent<BoxCollider2DComponent>())
			m_Registry.emplace_or_replace<BoxCollider2DComponent>(newEntity, entity.GetComponent<BoxCollider2DComponent>());
		if (entity.HasComponent<NativeScriptComponent>())
			m_Registry.emplace_or_replace<NativeScriptComponent>(newEntity, entity.GetComponent<NativeScriptComponent>());

		return newEntity;
	}

	Entity Graveyard::MoveEntityToScene(entt::entity entity, Ref<Scene> scene)
	{
		auto& tc = m_Registry.get<TagComponent>(entity);
		UUID uuid = m_Registry.get<IDComponent>(entity).ID;
		Entity newEntity = Entity(entity, scene.get());
		scene->CreateEntityWithUUID(newEntity, uuid, tc.Tag, tc.Enabled);

		newEntity.AddOrReplaceComponent<RelationshipComponent>(m_Registry.get<RelationshipComponent>(entity));
		newEntity.AddOrReplaceComponent<TransformComponent>(m_Registry.get<TransformComponent>(entity));
		if (m_Registry.any_of<SpriteRendererComponent>(entity))
			newEntity.AddOrReplaceComponent<SpriteRendererComponent>(m_Registry.get<SpriteRendererComponent>(entity));
		if (m_Registry.any_of<CameraComponent>(entity))
			newEntity.AddOrReplaceComponent<CameraComponent>(m_Registry.get<CameraComponent>(entity));
		if (m_Registry.any_of<Rigidbody2DComponent>(entity))
			newEntity.AddOrReplaceComponent<Rigidbody2DComponent>(m_Registry.get<Rigidbody2DComponent>(entity));
		if (m_Registry.any_of<BoxCollider2DComponent>(entity))
			newEntity.AddOrReplaceComponent<BoxCollider2DComponent>(m_Registry.get<BoxCollider2DComponent>(entity));
		if (m_Registry.any_of<NativeScriptComponent>(entity))
			newEntity.AddOrReplaceComponent<NativeScriptComponent>(m_Registry.get<NativeScriptComponent>(entity));

		m_Registry.destroy(entity);
		return newEntity;
	}
}