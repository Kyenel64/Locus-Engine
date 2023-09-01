#include "Lpch.h"
#include "Scene.h"

#include <glm/glm.hpp>
#include <box2d/b2_world.h>
#include <box2d/b2_body.h>
#include <box2d/b2_polygon_shape.h>
#include <box2d/b2_fixture.h>

#include "Locus/Renderer/Renderer2D.h"
#include "Locus/Renderer/RenderCommand.h"
#include "Locus/Renderer/EditorCamera.h"
#include "Locus/Scene/Components.h"
#include "Locus/Scene/ScriptableEntity.h"
#include "Locus/Scene/Entity.h"

namespace Locus
{
	static b2BodyType Rigidbody2DTypeToBox2DType(Rigidbody2DComponent::Rigidbody2DType bodyType)
	{
		switch (bodyType)
		{
		case Rigidbody2DComponent::Rigidbody2DType::Static: return b2_staticBody;
		case Rigidbody2DComponent::Rigidbody2DType::Dynamic: return b2_dynamicBody;
		case Rigidbody2DComponent::Rigidbody2DType::Kinematic: return b2_kinematicBody;
		}

		LOCUS_CORE_ASSERT(false, "Unknown Rigidbody2DType");
		return b2_staticBody;
	}

	Scene::Scene()
	{
		m_Graveyard = CreateRef<Graveyard>();
	}

	Ref<Scene> Scene::Copy(Ref<Scene> other)
	{
		Ref<Scene> newScene = CreateRef<Scene>();
		newScene->m_SceneName = other->m_SceneName;
		newScene->m_ViewportWidth = other->m_ViewportWidth;
		newScene->m_ViewportHeight = other->m_ViewportHeight;

		auto& otherRegistry = other->m_Registry;

		// Display each entity
		otherRegistry.each([&](auto entityID)
			{
				Entity entity = Entity(entityID, other.get());
				UUID uuid = otherRegistry.get<IDComponent>(entity).ID;
				const auto& tag = otherRegistry.get<TagComponent>(entity).Tag;

				Entity newEntity = newScene->CreateEntityWithUUID(uuid, tag);
				CopyAllComponents(entity, newEntity);
			});

		return newScene;
	}

	template<typename T>
	void Scene::CopyComponent(Entity from, Entity to)
	{
		if (from.HasComponent<T>())
			to.AddOrReplaceComponent<T>(from.GetComponent<T>());
	}

	void Scene::CopyAllComponents(Entity from, Entity to)
	{
		to.GetComponent<TagComponent>().Enabled = from.GetComponent<TagComponent>().Enabled;
		CopyComponent<RelationshipComponent>(from, to);
		CopyComponent<TransformComponent>(from, to);
		CopyComponent<SpriteRendererComponent>(from, to);
		CopyComponent<CameraComponent>(from, to);
		CopyComponent<Rigidbody2DComponent>(from, to);
		CopyComponent<BoxCollider2DComponent>(from, to);
		CopyComponent<SpriteRendererComponent>(from, to);
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		return CreateEntityWithUUID(UUID(), name);
	}

	Entity Scene::CreateEntityWithUUID(UUID uuid, const std::string& name, bool enabled)
	{
		Entity entity = Entity(m_Registry.create(), this);
		entity.AddComponent<IDComponent>(uuid);
		entity.AddComponent<TransformComponent>();
		entity.AddComponent<RelationshipComponent>();
		auto& tag = entity.AddComponent<TagComponent>();
		tag.Tag = name.empty() ? "Entity" : name;
		tag.Enabled = enabled;
		return entity;
	}

	// Only used when we want to create an entity with the same entt id. 
	// For example, undoing a destroy entity command will create a new Entity with a new entt id.
	// To prevent that, we use this function so the entt id stays the same.
	Entity Scene::CreateEntityWithUUID(Entity copyEntity, UUID uuid, const std::string& name, bool enabled)
	{
		Entity entity = Entity(m_Registry.create(copyEntity), this);
		entity.AddComponent<IDComponent>(uuid);
		entity.AddComponent<TransformComponent>();
		entity.AddComponent<RelationshipComponent>();
		auto& tag = entity.AddComponent<TagComponent>();
		tag.Tag = name.empty() ? "Entity" : name;
		tag.Enabled = enabled;
		return entity;
	}

	void Scene::DestroyEntity(Entity entity)
	{
		m_Registry.destroy(entity);
	}

	void Scene::OnUpdateRuntime(Timestep deltaTime)
	{
		// --- Update Scripts -------------------------------------------------
		{
			// TODO: Check for enabled.
			auto view = m_Registry.view<NativeScriptComponent, TagComponent>();
			for (auto entity : view)
			{
				auto& nsc = view.get<NativeScriptComponent>(entity);
				bool enabled = view.get<TagComponent>(entity).Enabled;
				if (enabled)
				{
					// temp. move this if statement to OnScenePlay
					if (!nsc.Instance)
					{
						nsc.Instance = nsc.InstantiateScript();
						nsc.Instance->m_Entity = Entity(entity, this);
						nsc.Instance->OnCreate();
					}
					nsc.Instance->OnUpdate(deltaTime);
				}
			}
		}

		// --- Physics --------------------------------------------------------
		m_Box2DWorld->Step(deltaTime, 6, 2); // TODO: paremeterize

		auto view = m_Registry.view<Rigidbody2DComponent, TagComponent>();
		for (auto e : view)
		{
			Entity entity = Entity(e, this);
			if (entity.GetComponent<TagComponent>().Enabled)
			{
				auto& transform = entity.GetComponent<TransformComponent>();
				auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();
			
				b2Body* body = (b2Body*)rb2d.RuntimeBody;
				const b2Vec2& position = body->GetPosition();
				transform.Translation.x = position.x;
				transform.Translation.y = position.y;
				transform.SetRotationEuler({ 0, 0, body->GetAngle()});
			}
			
		}

		// --- Rendering 2D ---------------------------------------------------
		// Find first main camera
		SceneCamera* mainCamera = nullptr;
		glm::mat4 cameraTransform;
		{
			auto view = m_Registry.view<TransformComponent, CameraComponent>();
			for (auto entity : view)
			{
				auto [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);
				if (camera.Primary)
				{
					mainCamera = &camera.Camera;
					cameraTransform = transform.GetTransform();
					break;
				}
			}
		}
		// Render if main camera exists
		if (mainCamera)
		{
			// Main rendering
			RenderCommand::SetClearColor(mainCamera->GetBackgroundColor());
			RenderCommand::Clear();

			Renderer2D::BeginScene(*mainCamera, cameraTransform);
			auto group = m_Registry.group<TransformComponent, SpriteRendererComponent, TagComponent>();
			for (auto entity : group)
			{
				auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
				bool enabled = group.get<TagComponent>(entity).Enabled;
				if (enabled)
					Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int)entity);
			}
			Renderer2D::EndScene();
		}
		else
		{
			RenderCommand::SetClearColor({ 0.2f, 0.2f, 0.25f, 1 });
			RenderCommand::Clear();
		}
	}

	void Scene::OnUpdateEditor(Timestep deltaTime, EditorCamera& camera)
	{
		// Main rendering
		Renderer2D::BeginScene(camera);
		auto group = m_Registry.group<TransformComponent, SpriteRendererComponent, TagComponent>();
		for (auto entity : group)
		{
			auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
			bool enabled = group.get<TagComponent>(entity).Enabled;
			if (enabled)
				Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int)entity);
		}
		Renderer2D::EndScene();
	}

	void Scene::OnRuntimeStart()
	{
		m_Box2DWorld = new b2World({ 0.0f, -9.8f });

		auto view = m_Registry.view<Rigidbody2DComponent, TagComponent>();
		for (auto e : view)
		{
			Entity entity = Entity(e, this);
			if (entity.GetComponent<TagComponent>().Enabled)
			{
				auto& transform = entity.GetComponent<TransformComponent>();
				auto& rb2D = entity.GetComponent<Rigidbody2DComponent>();

				// Body
				b2BodyDef bodyDef;
				bodyDef.type = Rigidbody2DTypeToBox2DType(rb2D.BodyType);
				bodyDef.position.Set(transform.Translation.x, transform.Translation.y);
				bodyDef.angle = transform.GetRotationEuler().z;
				bodyDef.linearDamping = rb2D.LinearDrag;
				bodyDef.angularDamping = rb2D.AngularDrag;
				bodyDef.fixedRotation = rb2D.FixedRotation;
				bodyDef.gravityScale = rb2D.GravityScale;
				b2Body* entityBody = m_Box2DWorld->CreateBody(&bodyDef);
				rb2D.RuntimeBody = entityBody;
				b2MassData massData;
				massData.mass = rb2D.Mass;
				entityBody->SetMassData(&massData);

				b2FixtureDef fixtureDef;
				fixtureDef.density = rb2D.Mass;
				fixtureDef.friction = rb2D.Friction;
				fixtureDef.restitution = rb2D.Restitution;
				fixtureDef.restitutionThreshold = rb2D.RestitutionThreshold;
				fixtureDef.filter.categoryBits = 0;

				// Box Collider
				b2PolygonShape box;
				b2Vec2 size = { transform.Scale.x, transform.Scale.y};
				b2Vec2 offset = { 0.0f, 0.0f };
				float angle = 0.0f;
				if (entity.HasComponent<BoxCollider2DComponent>()) // TODO: Reformat this
				{
					auto& b2D = entity.GetComponent<BoxCollider2DComponent>();

					fixtureDef.filter.categoryBits = b2D.CollisionLayer;
					size.x = b2D.Size.x * size.x;
					size.y = b2D.Size.y * size.y;
					offset.x = b2D.Offset.x;
					offset.y = b2D.Offset.y;
				}
				box.SetAsBox(size.x / 2, size.y / 2, offset, angle);
				fixtureDef.shape = &box;
				entityBody->CreateFixture(&fixtureDef);
			}
		}
	}

	void Scene::OnRuntimeStop()
	{
		delete m_Box2DWorld;
		m_Box2DWorld = nullptr;
	}

	void Scene::OnViewportResize(uint32_t width, uint32_t height)
	{
		m_ViewportWidth = width;
		m_ViewportHeight = height;

		auto view = m_Registry.view<CameraComponent>();
		for (auto entity : view)
		{
			auto& cameraComponent = view.get<CameraComponent>(entity);
			if (!cameraComponent.FixedAspectRatio)
			{
				cameraComponent.Camera.SetViewportSize(width, height);
			}
		}
	}

	Entity Scene::GetPrimaryCameraEntity()
	{
		auto view = m_Registry.view<CameraComponent>();
		for (auto entity : view)
		{
			if (view.get<CameraComponent>(entity).Primary)
				return Entity(entity, this);
		}
		return {};
	}

	template<typename T>
	void Scene::OnComponentAdded(Entity entity, T& component)
	{
		static_assert<false>;
	}

	template<>
	void Scene::OnComponentAdded<IDComponent>(Entity entity, IDComponent& component)
	{

	}

	template<>
	void Scene::OnComponentAdded<TagComponent>(Entity entity, TagComponent& component)
	{

	}

	template<>
	void Scene::OnComponentAdded<RelationshipComponent>(Entity entity, RelationshipComponent& component)
	{

	}

	template<>
	void Scene::OnComponentAdded<TransformComponent>(Entity entity, TransformComponent& component)
	{

	}

	template<>
	void Scene::OnComponentAdded<SpriteRendererComponent>(Entity entity, SpriteRendererComponent& component)
	{

	}

	template<>
	void Scene::OnComponentAdded<CameraComponent>(Entity entity, CameraComponent& component)
	{
		if (m_ViewportWidth > 0 && m_ViewportHeight > 0)
			component.Camera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
	}

	template<>
	void Scene::OnComponentAdded<Rigidbody2DComponent>(Entity entity, Rigidbody2DComponent& component)
	{

	}

	template<>
	void Scene::OnComponentAdded<BoxCollider2DComponent>(Entity entity, BoxCollider2DComponent& component)
	{

	}

	template<>
	void Scene::OnComponentAdded<NativeScriptComponent>(Entity entity, NativeScriptComponent& component)
	{

	}
}