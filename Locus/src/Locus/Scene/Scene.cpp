#include "Lpch.h"
#include "Scene.h"

#include <glm/glm.hpp>
#include <box2d/b2_world.h>
#include <box2d/b2_body.h>
#include <box2d/b2_polygon_shape.h>
#include <box2d/b2_circle_shape.h>
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
				Entity newEntity = newScene->CreateEntityWithUUID(entity.GetUUID());
				CopyAllComponents(entity, newEntity);
			});

		// Convert all Entity references to newScene since copying 
		newScene->m_Registry.each([&](auto entityID)
			{
				Entity entity = Entity(entityID, newScene.get());
				auto& tc = entity.GetComponent<TransformComponent>();
				if (tc.Parent != Entity::Null)
					tc.Parent = newScene->GetEntityByUUID(tc.Parent.GetUUID());

				if (entity.HasComponent<ChildComponent>())
				{
					auto& cc = entity.GetComponent<ChildComponent>();
					for (uint32_t i = 0; i < cc.ChildCount; i++)
					{
						cc.ChildEntities[i] = newScene->GetEntityByUUID(cc.ChildEntities[i].GetUUID());
					}
				}
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
		CopyComponent<TagComponent>(from, to);
		CopyComponent<ChildComponent>(from, to);
		CopyComponent<TransformComponent>(from, to);
		CopyComponent<SpriteRendererComponent>(from, to);
		CopyComponent<CircleRendererComponent>(from, to);
		CopyComponent<CameraComponent>(from, to);
		CopyComponent<Rigidbody2DComponent>(from, to);
		CopyComponent<BoxCollider2DComponent>(from, to);
		CopyComponent<CircleCollider2DComponent>(from, to);
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
		entity.GetComponent<TransformComponent>().Self = entity;
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
		entity.GetComponent<TransformComponent>().Self = entity;
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
				transform.LocalPosition = { position.x, position.y , 0.0f };
				transform.SetLocalRotation( {0, 0, body->GetAngle()} );
			}
			
		}

		// --- Rendering 2D ---------------------------------------------------
		// Find first main camera
		SceneCamera* mainCamera = nullptr;
		glm::mat4 cameraTransform;
		{
			auto view = m_Registry.view<TransformComponent, CameraComponent>();
			for (auto e : view)
			{
				Entity entity = Entity(e, this);
				auto& camera = entity.GetComponent<CameraComponent>();
				if (camera.Primary)
				{
					mainCamera = &camera.Camera;
					cameraTransform = GetWorldTransform(entity);
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

			{ // Sprite
				auto group = m_Registry.group<TransformComponent, SpriteRendererComponent, TagComponent>();
				for (auto e : group)
				{
					Entity entity = Entity(e, this);
					bool enabled = entity.GetComponent<TagComponent>().Enabled;
					auto& sprite = entity.GetComponent<SpriteRendererComponent>();
					if (enabled)
						Renderer2D::DrawSprite(GetWorldTransform(entity), sprite, (int)e);
				}
			}
			
			{ // Circle
				auto view = m_Registry.view<TransformComponent, CircleRendererComponent, TagComponent>();
				for (auto e : view)
				{
					Entity entity = Entity(e, this);
					bool enabled = entity.GetComponent<TagComponent>().Enabled;
					auto& circle = entity.GetComponent<CircleRendererComponent>();
					if (enabled)
						Renderer2D::DrawCircle(GetWorldTransform(entity), circle.Color, circle.Thickness, circle.Fade, (int)e);
				}
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

		{ // Sprite
			auto group = m_Registry.group<TransformComponent, SpriteRendererComponent, TagComponent>();
			for (auto e : group)
			{
				Entity entity = Entity(e, this);
				bool enabled = entity.GetComponent<TagComponent>().Enabled;
				auto& sprite = entity.GetComponent<SpriteRendererComponent>();
				if (enabled)
					Renderer2D::DrawSprite(GetWorldTransform(entity), sprite, (int)e);
			}
		}

		{ // Circle
			auto view = m_Registry.view<TransformComponent, CircleRendererComponent, TagComponent>();
			for (auto e : view)
			{
				Entity entity = Entity(e, this);
				bool enabled = entity.GetComponent<TagComponent>().Enabled;
				auto& circle = entity.GetComponent<CircleRendererComponent>();
				if (enabled)
					Renderer2D::DrawCircle(GetWorldTransform(entity), circle.Color, circle.Thickness, circle.Fade, (int)e);
			}
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
				auto& tc = entity.GetComponent<TransformComponent>();
				auto& rb2D = entity.GetComponent<Rigidbody2DComponent>();

				// Body
				b2BodyDef bodyDef;
				bodyDef.type = Rigidbody2DTypeToBox2DType(rb2D.BodyType);
				
				bodyDef.position.Set(tc.LocalPosition.x, tc.LocalPosition.y);
				bodyDef.angle = tc.LocalRotation.z;
				bodyDef.linearDamping = rb2D.LinearDamping;
				bodyDef.angularDamping = rb2D.AngularDamping;
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

				if (entity.HasComponent<BoxCollider2DComponent>())
				{
					auto& b2D = entity.GetComponent<BoxCollider2DComponent>();

					b2Vec2 size = { b2D.Size.x * tc.LocalScale.x, b2D.Size.y * tc.LocalScale.y };
					b2Vec2 offset = { tc.LocalScale.x * b2D.Offset.x, tc.LocalScale.y * b2D.Offset.y };
					float angle = 0.0f; // TODO

					b2PolygonShape box;
					box.SetAsBox(size.x / 2, size.y / 2, offset, angle);
					fixtureDef.shape = &box;
					fixtureDef.filter.categoryBits = b2D.CollisionLayer;
					b2Fixture* fixture = entityBody->CreateFixture(&fixtureDef);
					b2D.RuntimeFixture = fixture;
				}
				else if (entity.HasComponent<CircleCollider2DComponent>())
				{
					auto& c2D = entity.GetComponent<CircleCollider2DComponent>();

					float maxScale = tc.LocalScale.x > tc.LocalScale.y ? tc.LocalScale.x : tc.LocalScale.y;
					b2Vec2 offset = { maxScale * c2D.Offset.x, maxScale * c2D.Offset.y };
					float radius = maxScale * c2D.Radius;

					b2CircleShape circle;
					circle.m_p = offset;
					circle.m_radius = radius;
					fixtureDef.shape = &circle;
					fixtureDef.filter.categoryBits = c2D.CollisionLayer;
					b2Fixture* fixture = entityBody->CreateFixture(&fixtureDef);
					c2D.RuntimeFixture = fixture;
				}
				
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

	Entity Scene::GetEntityByUUID(UUID uuid)
	{
		auto view = m_Registry.view<IDComponent>();
		for (auto e : view)
		{
			Entity entity = Entity(e, this);
			if (entity.GetComponent<IDComponent>().ID == uuid)
				return entity;
		}

		return Entity::Null;
	}

	glm::mat4 Scene::GetWorldTransform(Entity entity)
	{
		glm::mat4 transform(1.0f);
		auto& tc = entity.GetComponent<TransformComponent>();

		if (tc.Parent != Entity::Null)
			transform = GetWorldTransform(tc.Parent);

		return transform * tc.GetLocalTransform();
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
	void Scene::OnComponentAdded<TransformComponent>(Entity entity, TransformComponent& component)
	{

	}

	template<>
	void Scene::OnComponentAdded<ChildComponent>(Entity entity, ChildComponent& component)
	{

	}

	template<>
	void Scene::OnComponentAdded<SpriteRendererComponent>(Entity entity, SpriteRendererComponent& component)
	{

	}

	template<>
	void Scene::OnComponentAdded<CircleRendererComponent>(Entity entity, CircleRendererComponent& component)
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
	void Scene::OnComponentAdded<CircleCollider2DComponent>(Entity entity, CircleCollider2DComponent& component)
	{

	}

	template<>
	void Scene::OnComponentAdded<NativeScriptComponent>(Entity entity, NativeScriptComponent& component)
	{

	}
}