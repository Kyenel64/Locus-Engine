#include "Lpch.h"
#include "Scene.h"

#include <glm/glm.hpp>
#include <box2d/b2_world.h>
#include <box2d/b2_body.h>
#include <box2d/b2_polygon_shape.h>
#include <box2d/b2_circle_shape.h>
#include <box2d/b2_fixture.h>

#include "Locus/Renderer/Renderer.h"
#include "Locus/Renderer/Renderer2D.h"
#include "Locus/Renderer/Renderer3D.h"
#include "Locus/Renderer/RenderCommand.h"
#include "Locus/Renderer/EditorCamera.h"
#include "Locus/Scene/Components.h"
#include "Locus/Scene/Entity.h"
#include "Locus/Scripting/ScriptEngine.h"
#include "Locus/Physics2D/ContactListener2D.h"
#include "Locus/Physics2D/PhysicsUtils.h"
#include "Locus/Resource/TextureManager.h"

namespace Locus
{
	Scene::Scene()
	{
		m_ContactListener = CreateRef<ContactListener2D>();
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		return CreateEntityWithUUID(UUID(), name);
	}

	Entity Scene::CreateEntityWithUUID(UUID uuid, const std::string& name)
	{
		Entity entity = Entity(m_Registry.create(), this);
		entity.AddComponent<IDComponent>(uuid);
		entity.AddComponent<TransformComponent>();
		entity.GetComponent<TransformComponent>().Self = uuid;
		auto& tag = entity.AddComponent<TagComponent>();
		tag.Tag = name.empty() ? "Entity" : name;
		tag.Group = "Default";
		tag.Enabled = true;
		m_Entities[uuid] = entity;
		return entity;
	}

	void Scene::DestroyEntity(Entity entity)
	{
		LOCUS_CORE_ASSERT(entity.IsValid(), "DestroyEntity(): Entity is not valid!");

		if (entity.GetComponent<TransformComponent>().Parent)
		{
			UUID parentUUID = entity.GetComponent<TransformComponent>().Parent;
			Entity parentEntity = GetEntityByUUID(parentUUID);
			auto& parentCC = parentEntity.GetComponent<ChildComponent>();
			parentCC.ChildEntities.erase(std::find(parentCC.ChildEntities.begin(), parentCC.ChildEntities.end(), entity.GetUUID()));
			parentCC.ChildCount--;

			if (parentCC.ChildCount == 0)
				parentEntity.RemoveComponent<ChildComponent>();
		}

		DestroyChildren(entity);

		m_Entities.erase(m_Entities.find(entity.GetUUID()));
		m_Registry.destroy(entity);
	}

	void Scene::DestroyChildren(Entity entity)
	{
		if (entity.HasComponent<ChildComponent>())
		{
			for (UUID childUUID : entity.GetComponent<ChildComponent>().ChildEntities)
			{
				Entity childEntity = GetEntityByUUID(childUUID);
				DestroyChildren(childEntity);
				m_Entities.erase(childUUID);
				m_Registry.destroy(childEntity);
			}
		}
	}

	Ref<Scene> Scene::Copy(Ref<Scene> other)
	{
		Ref<Scene> newScene = CreateRef<Scene>();
		newScene->m_SceneName = other->m_SceneName;
		newScene->m_ViewportWidth = other->m_ViewportWidth;
		newScene->m_ViewportHeight = other->m_ViewportHeight;

		other->m_Registry.each([&](auto entityID)
			{
				Entity entity = Entity(entityID, other.get());
				Entity newEntity = newScene->CreateEntityWithUUID(entity.GetUUID());
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
		CopyComponent<TagComponent>(from, to);
		CopyComponent<ChildComponent>(from, to);
		CopyComponent<TransformComponent>(from, to);
		CopyComponent<SpriteRendererComponent>(from, to);
		CopyComponent<CircleRendererComponent>(from, to);
		CopyComponent<CubeRendererComponent>(from, to);
		CopyComponent<MeshRendererComponent>(from, to);
		CopyComponent<PointLightComponent>(from, to);
		CopyComponent<DirectionalLightComponent>(from, to);
		CopyComponent<SpotLightComponent>(from, to);
		CopyComponent<CameraComponent>(from, to);
		CopyComponent<Rigidbody2DComponent>(from, to);
		CopyComponent<BoxCollider2DComponent>(from, to);
		CopyComponent<CircleCollider2DComponent>(from, to);
		CopyComponent<ScriptComponent>(from, to);
	}

	void Scene::OnEditorUpdate(Timestep deltaTime, EditorCamera& camera)
	{
		// --- Lighting ---
		ClearLightingData();
		ProcessPointLights();
		ProcessDirectionalLights();
		ProcessSpotLights();

		// --- Rendering ---
		Renderer::BeginScene(camera);

		// 2D
		Renderer2D::BeginScene(camera);
		DrawSprites();
		DrawCircles();
		Renderer2D::EndScene();
		// 3D
		Renderer3D::BeginScene(camera, this);
		DrawCubes();
		DrawMeshes();
		Renderer3D::EndScene();
		// Grid
		if (camera.GetGridVisibility())
			Renderer3D::DrawGrid();
		Renderer::EndScene();
	}

	void Scene::OnRuntimeUpdate(Timestep deltaTime)
	{
		// --- Physics ---
		{ // Check physics data for changes
			auto view = m_Registry.view<IDComponent>();
			for (auto e : view)
			{
				Entity entity = Entity(e, this);
				UpdatePhysicsData(entity);
			}
		}

		{ // Simulate physics
			m_Box2DWorld->Step(deltaTime, 6, 2); // TODO: paremeterize
			m_ContactListener->Execute();
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
					transform.SetLocalRotation({ 0, 0, glm::degrees(body->GetAngle()) });
				}
			}
		}

		// --- Update C# Scripts ---
		{
			auto view = m_Registry.view<ScriptComponent, TagComponent>();
			for (auto e : view)
			{
				Entity entity = Entity(e, this);
				if (entity.GetComponent<TagComponent>().Enabled)
					ScriptEngine::OnUpdateEntityScript(entity, deltaTime);
			}
		}

		// --- Lighting ---
		ClearLightingData();
		ProcessPointLights();
		ProcessDirectionalLights();
		ProcessSpotLights();

		// --- Rendering ---
		// Find first camera with "Primary" property enabled.
		SceneCamera* mainCamera = nullptr;
		glm::mat4 cameraTransform;
		{
			Entity primaryCameraEntity = GetPrimaryCameraEntity();
			if (primaryCameraEntity.IsValid())
			{
				auto& camera = primaryCameraEntity.GetComponent<CameraComponent>();
				mainCamera = &camera.Camera;
				mainCamera->SetViewportSize(m_ViewportWidth, m_ViewportHeight);
				cameraTransform = GetWorldTransform(primaryCameraEntity);
			}
		}
		if (mainCamera)
		{
			Renderer::BeginScene(*mainCamera, cameraTransform);

			// 2D
			Renderer2D::BeginScene(*mainCamera, cameraTransform);
			DrawSprites();
			DrawCircles();
			Renderer2D::EndScene();
			// 3D
			Renderer3D::BeginScene(*mainCamera, cameraTransform, this);
			DrawCubes();
			Renderer3D::EndScene();

			Renderer::EndScene();
		}
		else
		{
			RenderCommand::SetClearColor({ 0.2f, 0.2f, 0.25f, 1 });
			RenderCommand::Clear();
		}
	}

	void Scene::OnPhysicsUpdate(Timestep deltaTime, EditorCamera& camera)
	{
		// --- Physics ---
		{ // Check physics data for changes
			auto view = m_Registry.view<IDComponent>();
			for (auto e : view)
			{
				Entity entity = Entity(e, this);
				UpdatePhysicsData(entity);
			}
		}
		{ // Simulate physics
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
					transform.SetLocalRotation({ 0, 0, glm::degrees(body->GetAngle()) });
				}
			}
		}

		// --- Lighting ---
		ClearLightingData();
		ProcessPointLights();
		ProcessDirectionalLights();
		ProcessSpotLights();

		// --- Rendering ---
		Renderer::BeginScene(camera);

		// 2D
		Renderer2D::BeginScene(camera);
		DrawSprites();
		DrawCircles();
		Renderer2D::EndScene();
		// 3D
		Renderer3D::BeginScene(camera, this);
		DrawCubes();
		Renderer3D::EndScene();
		// Grid
		if (camera.GetGridVisibility())
			Renderer3D::DrawGrid();

		Renderer::EndScene();
	}

	void Scene::OnRuntimeStart()
	{	
		// --- Physics ---
		{
			m_Box2DWorld = new b2World({ 0.0f, -9.8f });
			m_Box2DWorld->SetContactListener(m_ContactListener.get());

			auto view = m_Registry.view<TagComponent>();
			for (auto e : view)
			{
				Entity entity = Entity(e, this);
				if (entity.GetComponent<TagComponent>().Enabled)
					CreatePhysicsData(entity);
			}
		}

		// --- C# Scripts ---
		{
			auto view = m_Registry.view<ScriptComponent, TagComponent, IDComponent>();
			for (auto e : view)
			{
				Entity entity = Entity(e, this);
				auto& sc = entity.GetComponent<ScriptComponent>();
				if (entity.GetComponent<TagComponent>().Enabled)
				{
					ScriptEngine::OnCreateEntityScript(entity);
				}
			}
		}
	}

	void Scene::OnPhysicsStart()
	{
		// --- Physics ---
		{
			m_Box2DWorld = new b2World({ 0.0f, -9.8f });

			auto view = m_Registry.view<IDComponent>();
			for (auto e : view)
			{
				Entity entity = Entity(e, this);
				if (entity.GetComponent<TagComponent>().Enabled)
					CreatePhysicsData(entity);
			}
		}
	}

	void Scene::OnRuntimeStop()
	{
		delete m_Box2DWorld;
		m_Box2DWorld = nullptr;
	}

	void Scene::OnPhysicsStop()
	{
		delete m_Box2DWorld;
		m_Box2DWorld = nullptr;
	}

	void Scene::OnRuntimePause(Timestep deltaTime)
	{
		// --- Lighting ---
		ClearLightingData();
		ProcessPointLights();
		ProcessDirectionalLights();
		ProcessSpotLights();

		// --- Rendering ---
		// Find first camera with "Primary" property enabled.
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
					mainCamera->SetViewportSize(m_ViewportWidth, m_ViewportHeight);
					cameraTransform = GetWorldTransform(entity);
					break;
				}
			}
		}
		if (mainCamera)
		{
			Renderer::BeginScene(*mainCamera, cameraTransform);

			// 2D
			Renderer2D::BeginScene(*mainCamera, cameraTransform);
			DrawSprites();
			DrawCircles();
			Renderer2D::EndScene();
			// 3D
			Renderer3D::BeginScene(*mainCamera, cameraTransform, this);
			DrawCubes();
			Renderer3D::EndScene();

			Renderer::EndScene();
		}
		else
		{
			RenderCommand::SetClearColor({ 0.2f, 0.2f, 0.25f, 1 });
			RenderCommand::Clear();
		}
	}

	void Scene::OnPhysicsPause(Timestep deltaTime, EditorCamera& camera)
	{
		// --- Lighting ---
		ClearLightingData();
		ProcessPointLights();
		ProcessDirectionalLights();
		ProcessSpotLights();

		// --- Rendering ---
		Renderer::BeginScene(camera);

		// 2D
		Renderer2D::BeginScene(camera);
		DrawSprites();
		DrawCircles();
		Renderer2D::EndScene();
		// 3D
		Renderer3D::BeginScene(camera, this);
		DrawCubes();
		Renderer3D::EndScene();
		// Grid
		if (camera.GetGridVisibility())
			Renderer3D::DrawGrid();
		Renderer::EndScene();
	}

	void Scene::OnPreviewUpdate(Entity entity)
	{
		// --- Lighting ---
		ClearLightingData();
		ProcessPointLights();
		ProcessDirectionalLights();
		ProcessSpotLights();

		// --- Rendering ---
		SceneCamera& camera = entity.GetComponent<CameraComponent>().Camera;
		glm::mat4 cameraTransform = GetWorldTransform(entity);

		Renderer::BeginScene(camera, cameraTransform);

		// 2D
		Renderer2D::BeginScene(camera, cameraTransform);
		DrawSprites();
		DrawCircles();
		Renderer2D::EndScene();
		// 3D
		Renderer3D::BeginScene(camera, cameraTransform, this);
		DrawCubes();
		Renderer3D::EndScene();

		Renderer::EndScene();
	}

	void Scene::ClearLightingData()
	{
		for (int i = 0; i < 16; i++)
		{
			m_SceneLighting.PointLights[i].Position = glm::vec4(0.0f);
			m_SceneLighting.PointLights[i].Color = glm::vec4(0.0f);
			m_SceneLighting.PointLights[i].Intensity = 0.0f;
			m_SceneLighting.PointLights[i].Enabled = false;
			m_SceneLighting.DirectionalLights[i].Direction = glm::vec4(0.0f);
			m_SceneLighting.DirectionalLights[i].Color = glm::vec4(0.0f);
			m_SceneLighting.DirectionalLights[i].Intensity = 0.0f;
			m_SceneLighting.DirectionalLights[i].Enabled = false;
			m_SceneLighting.SpotLights[i].Position = glm::vec4(0.0f);
			m_SceneLighting.SpotLights[i].Direction = glm::vec4(0.0f);
			m_SceneLighting.SpotLights[i].Color = glm::vec4(0.0f);
			m_SceneLighting.SpotLights[i].Intensity = 0.0f;
			m_SceneLighting.SpotLights[i].CutOff = 0.0f;
			m_SceneLighting.SpotLights[i].OuterCutOff = 0.0f;
			m_SceneLighting.SpotLights[i].Enabled = false;
		}
	}

	void Scene::ProcessPointLights()
	{
		int index = 0;
		auto view = m_Registry.view<TransformComponent, PointLightComponent, TagComponent>();
		for (auto e : view)
		{
			Entity entity = Entity(e, this);
			auto& pointLight = entity.GetComponent<PointLightComponent>();
			if (entity.GetComponent<TagComponent>().Enabled)
			{
				glm::mat4 worldTransform = GetWorldTransform(entity);
				m_SceneLighting.PointLights[index].Position = { worldTransform[3].x, worldTransform[3].y, worldTransform[3].z, 1.0f };
				m_SceneLighting.PointLights[index].Color = pointLight.Color;
				m_SceneLighting.PointLights[index].Intensity = pointLight.Intensity;
				m_SceneLighting.PointLights[index].Enabled = true;

				index++;
			}
		}
	}

	void Scene::ProcessDirectionalLights()
	{
		int index = 0;
		auto view = m_Registry.view<TransformComponent, DirectionalLightComponent, TagComponent>();
		for (auto e : view)
		{
			Entity entity = Entity(e, this);
			auto& directionalLight = entity.GetComponent<DirectionalLightComponent>();
			if (entity.GetComponent<TagComponent>().Enabled)
			{
				// Calculate the forward vector of the directional light.
				glm::quat worldRotationQuat;
				glm::mat4 worldTransform = GetWorldTransform(entity);
				Math::Decompose(worldTransform, glm::vec3(), worldRotationQuat, glm::vec3());
				glm::vec3 worldRotationEuler = glm::eulerAngles(worldRotationQuat);
				glm::vec4 lightDirection = { cos(worldRotationEuler.x) * sin(worldRotationEuler.y), -sin(worldRotationEuler.x), cos(worldRotationEuler.x) * cos(worldRotationEuler.y), 1.0f };

				m_SceneLighting.DirectionalLights[index].Direction = lightDirection;
				m_SceneLighting.DirectionalLights[index].Color = directionalLight.Color;
				m_SceneLighting.DirectionalLights[index].Intensity = directionalLight.Intensity;
				m_SceneLighting.DirectionalLights[index].Enabled = true;

				index++;
			}
		}
	}

	void Scene::ProcessSpotLights()
	{
		int index = 0;
		auto view = m_Registry.view<TransformComponent, SpotLightComponent, TagComponent>();
		for (auto e : view)
		{
			Entity entity = Entity(e, this);
			auto& directionalLight = entity.GetComponent<SpotLightComponent>();
			if (entity.GetComponent<TagComponent>().Enabled)
			{
				// Calculate the forward vector of the directional light.
				glm::quat worldRotationQuat;
				glm::mat4 worldTransform = GetWorldTransform(entity);
				Math::Decompose(worldTransform, glm::vec3(), worldRotationQuat, glm::vec3());
				glm::vec3 worldRotationEuler = glm::eulerAngles(worldRotationQuat);
				glm::vec4 lightDirection = { cos(worldRotationEuler.x) * sin(worldRotationEuler.y), -sin(worldRotationEuler.x), cos(worldRotationEuler.x) * cos(worldRotationEuler.y), 1.0f };

				m_SceneLighting.SpotLights[index].Position = { worldTransform[3].x, worldTransform[3].y, worldTransform[3].z, 1.0f };
				m_SceneLighting.SpotLights[index].Direction = lightDirection;
				m_SceneLighting.SpotLights[index].Color = directionalLight.Color;
				m_SceneLighting.SpotLights[index].Intensity = directionalLight.Intensity;
				m_SceneLighting.SpotLights[index].CutOff = cos(glm::radians(directionalLight.CutOff));
				m_SceneLighting.SpotLights[index].OuterCutOff = cos(glm::radians(directionalLight.OuterCutOff));
				m_SceneLighting.SpotLights[index].Enabled = true;

				index++;
			}
		}
	}

	void Scene::DrawSprites()
	{
		auto view = m_Registry.view<TransformComponent, SpriteRendererComponent, TagComponent>();
		for (auto e : view)
		{
			Entity entity = Entity(e, this);
			auto& sprite = entity.GetComponent<SpriteRendererComponent>();
			if (entity.GetComponent<TagComponent>().Enabled)
				Renderer2D::DrawSprite(GetWorldTransform(entity), sprite, (int)e);
		}
	}

	void Scene::DrawCircles()
	{
		auto view = m_Registry.view<TransformComponent, CircleRendererComponent, TagComponent>();
		for (auto e : view)
		{
			Entity entity = Entity(e, this);
			auto& circle = entity.GetComponent<CircleRendererComponent>();
			if (entity.GetComponent<TagComponent>().Enabled)
				Renderer2D::DrawCircle(GetWorldTransform(entity), circle.Color, circle.Thickness, circle.Fade, (int)e);
		}
	}

	void Scene::DrawCubes()
	{
		auto view = m_Registry.view<TransformComponent, CubeRendererComponent, TagComponent>();
		for (auto e : view)
		{
			Entity entity = Entity(e, this);
			auto& cube = entity.GetComponent<CubeRendererComponent>();
			Ref<Material> material = MaterialManager::GetMaterial(cube.Material);
			if (entity.GetComponent<TagComponent>().Enabled)
				Renderer3D::DrawCube(GetWorldTransform(entity), material, (int)e);
		}
	}

	void Scene::DrawMeshes()
	{
		auto view = m_Registry.view<TransformComponent, MeshRendererComponent, TagComponent>();
		for (auto e : view)
		{
			Entity entity = Entity(e, this);
			auto& mrc = entity.GetComponent<MeshRendererComponent>();
			Ref<Model> model = ModelManager::GetModel(mrc.Model);
			Ref<Material> material = MaterialManager::GetMaterial(mrc.Material);
			if (entity.GetComponent<TagComponent>().Enabled)
				Renderer3D::DrawModel(GetWorldTransform(entity), model ? model->GetVertexArray() : nullptr, material, (int)e);
		}
	}

	void Scene::CreatePhysicsData(Entity entity)
	{
		auto& tc = entity.GetComponent<TransformComponent>();

		b2Body* entityBody = nullptr;
		float mass = -1.0f;

		// If entity has a collider and rigidbody
		if (entity.HasComponent<Rigidbody2DComponent>())
		{
			auto& rb2D = entity.GetComponent<Rigidbody2DComponent>();
			mass = rb2D.Mass;

			// Body
			b2BodyDef bodyDef;
			bodyDef.type = Utils::Rigidbody2DTypeToBox2DType(rb2D.BodyType);
			bodyDef.position.Set(tc.LocalPosition.x, tc.LocalPosition.y);
			bodyDef.angle = glm::radians(tc.LocalRotation.z);
			bodyDef.linearDamping = rb2D.LinearDamping;
			bodyDef.angularDamping = rb2D.AngularDamping;
			bodyDef.fixedRotation = rb2D.FixedRotation;
			bodyDef.gravityScale = rb2D.GravityScale;
			bodyDef.bullet = rb2D.IsBullet;
			bodyDef.userData.pointer = (uintptr_t)entity.GetUUID();
			entityBody = m_Box2DWorld->CreateBody(&bodyDef);
			rb2D.RuntimeBody = entityBody;
			b2MassData massData = {};
			massData.mass = rb2D.Mass;
			massData.I = entityBody->GetInertia();
			massData.center = entityBody->GetLocalCenter();
			entityBody->SetMassData(&massData);
		}
		else
		{
			// If entity has a collider but no rigidbody
			if (entity.HasComponent<BoxCollider2DComponent>() || entity.HasComponent<CircleCollider2DComponent>())
			{
				mass = 0.0f;

				b2BodyDef bodyDef;
				bodyDef.type = b2BodyType::b2_staticBody;
				bodyDef.position.Set(tc.LocalPosition.x, tc.LocalPosition.y);
				bodyDef.angle = glm::radians(tc.LocalRotation.z);
				bodyDef.userData.pointer = (uintptr_t)entity.GetUUID();
				entityBody = m_Box2DWorld->CreateBody(&bodyDef);
			}
		}

		if (entity.HasComponent<BoxCollider2DComponent>()) // TODO: Check memory leak
		{
			auto& b2D = entity.GetComponent<BoxCollider2DComponent>();
			
			b2Vec2 size = { b2D.Size.x * tc.LocalScale.x, b2D.Size.y * tc.LocalScale.y };
			b2Vec2 offset = { tc.LocalScale.x * b2D.Offset.x, tc.LocalScale.y * b2D.Offset.y };
			float angle = 0.0f; // TODO

			b2PolygonShape box;
			box.SetAsBox(size.x / 2, size.y / 2, offset, angle);
			b2FixtureDef fixtureDef; // Move this to collider
			fixtureDef.density = mass; // TODO: Calculate mass from body and implement multiple fixtures per body.
			fixtureDef.friction = b2D.Friction;
			fixtureDef.restitution = b2D.Restitution;
			fixtureDef.restitutionThreshold = b2D.RestitutionThreshold;
			fixtureDef.shape = &box;
			fixtureDef.filter.categoryBits = b2D.CollisionCategory;
			fixtureDef.filter.maskBits = b2D.CollisionMask;
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
			b2FixtureDef fixtureDef; // Move this to collider
			fixtureDef.density = mass;
			fixtureDef.friction = c2D.Friction;
			fixtureDef.restitution = c2D.Restitution;
			fixtureDef.restitutionThreshold = c2D.RestitutionThreshold;
			circle.m_p = offset;
			circle.m_radius = radius;
			fixtureDef.shape = &circle;
			fixtureDef.filter.categoryBits = c2D.CollisionCategory;
			fixtureDef.filter.maskBits = c2D.CollisionMask;
			b2Fixture* fixture = entityBody->CreateFixture(&fixtureDef);
			c2D.RuntimeFixture = fixture;
		}
	}

	void Scene::UpdatePhysicsData(Entity entity)
	{
		auto& tc = entity.GetComponent<TransformComponent>();

		// Create data if physics data doesn't exist
		if (entity.HasComponent<Rigidbody2DComponent>())
		{
			if (entity.GetComponent<Rigidbody2DComponent>().RuntimeBody == nullptr)
				CreatePhysicsData(entity);
		}
		if (entity.HasComponent<BoxCollider2DComponent>())
		{
			if (entity.GetComponent<BoxCollider2DComponent>().RuntimeFixture == nullptr)
				CreatePhysicsData(entity);
		}
		if (entity.HasComponent<CircleCollider2DComponent>())
		{
			if (entity.GetComponent<CircleCollider2DComponent>().RuntimeFixture == nullptr)
				CreatePhysicsData(entity);
		}

		// Profile performance vs not checking each property for a change.
		if (entity.HasComponent<Rigidbody2DComponent>())
		{
			auto& rb2D = entity.GetComponent<Rigidbody2DComponent>();
			b2Body* runtimeBody = (b2Body*)rb2D.RuntimeBody;
			
			if (runtimeBody->GetType() != Utils::Rigidbody2DTypeToBox2DType(rb2D.BodyType))
				runtimeBody->SetType(Utils::Rigidbody2DTypeToBox2DType(rb2D.BodyType));

			if (runtimeBody->GetMass() != rb2D.Mass)
			{
				b2MassData massData = {};
				massData.mass = rb2D.Mass;
				massData.I = runtimeBody->GetInertia();
				massData.center = runtimeBody->GetLocalCenter();
				runtimeBody->SetMassData(&massData);
			}

			if (runtimeBody->GetGravityScale() != rb2D.GravityScale)
				runtimeBody->SetGravityScale(rb2D.GravityScale);
			if (runtimeBody->GetLinearDamping() != rb2D.LinearDamping)
				runtimeBody->SetLinearDamping(rb2D.LinearDamping);
			if (runtimeBody->GetAngularDamping() != rb2D.AngularDamping)
				runtimeBody->SetAngularDamping(rb2D.AngularDamping);
			if (runtimeBody->IsFixedRotation() != rb2D.FixedRotation)
				runtimeBody->SetFixedRotation(rb2D.FixedRotation);
			if (runtimeBody->IsBullet() != rb2D.IsBullet)
				runtimeBody->SetBullet(rb2D.IsBullet);
		}

		if (entity.HasComponent<BoxCollider2DComponent>())
		{
			auto& b2D = entity.GetComponent<BoxCollider2DComponent>();

			b2Vec2 size = { b2D.Size.x * tc.LocalScale.x, b2D.Size.y * tc.LocalScale.y };
			b2Vec2 offset = { tc.LocalScale.x * b2D.Offset.x, tc.LocalScale.y * b2D.Offset.y };
			float angle = 0.0f; // TODO
			float mass = entity.HasComponent<Rigidbody2DComponent>() ? entity.GetComponent<Rigidbody2DComponent>().Mass : 0.0f;

			b2Fixture* runtimeFixture = (b2Fixture*)b2D.RuntimeFixture;
			b2PolygonShape* shape = (b2PolygonShape*)runtimeFixture->GetShape();

			// TODO: Collision jittery when changed during runtime
			shape->SetAsBox(size.x / 2, size.y / 2, offset, angle);
			// Check if shape is changed
			/*b2PolygonShape thisShape;
			thisShape.SetAsBox(size.x / 2, size.y / 2, offset, angle);
			if (thisShape.m_vertices[0] != shape->m_vertices[0] || thisShape.m_vertices[1] != shape->m_vertices[1]
				|| thisShape.m_vertices[2] != shape->m_vertices[2] || thisShape.m_vertices[3] != shape->m_vertices[3])
			{
				for (int i = 0; i < 4; i++)
					shape->m_vertices[i] = thisShape.m_vertices[i];
			}*/
			if (runtimeFixture->GetDensity() != mass)
				runtimeFixture->SetDensity(mass);
			if (runtimeFixture->GetFriction() != b2D.Friction)
				runtimeFixture->SetFriction(b2D.Friction);
			if (runtimeFixture->GetRestitution() != b2D.Restitution)
				runtimeFixture->SetRestitution(b2D.Restitution);
			if (runtimeFixture->GetRestitutionThreshold() != b2D.RestitutionThreshold)
				runtimeFixture->SetRestitutionThreshold(b2D.RestitutionThreshold);
			if (runtimeFixture->GetFilterData().categoryBits != b2D.CollisionCategory || runtimeFixture->GetFilterData().maskBits != b2D.CollisionMask)
			{
				b2Filter filter;
				filter.categoryBits = b2D.CollisionCategory;
				filter.maskBits = b2D.CollisionMask;
				runtimeFixture->SetFilterData(filter);
			}
		}

		if (entity.HasComponent<CircleCollider2DComponent>())
		{
			auto& c2D = entity.GetComponent<CircleCollider2DComponent>();

			float maxScale = tc.LocalScale.x > tc.LocalScale.y ? tc.LocalScale.x : tc.LocalScale.y;
			b2Vec2 offset = { maxScale * c2D.Offset.x, maxScale * c2D.Offset.y };
			float radius = maxScale * c2D.Radius;
			float mass = entity.HasComponent<Rigidbody2DComponent>() ? entity.GetComponent<Rigidbody2DComponent>().Mass : 0.0f;

			b2Fixture* runtimeFixture = (b2Fixture*)c2D.RuntimeFixture;
			b2CircleShape* shape = (b2CircleShape*)runtimeFixture->GetShape();
			
			if (shape->m_radius != radius)
				shape->m_radius = radius;
			if (shape->m_p != offset)
				shape->m_p = offset;
			if (runtimeFixture->GetDensity() != mass)
				runtimeFixture->SetDensity(mass);
			if (runtimeFixture->GetFriction() != c2D.Friction)
				runtimeFixture->SetFriction(c2D.Friction);
			if (runtimeFixture->GetRestitution() != c2D.Restitution)
				runtimeFixture->SetRestitution(c2D.Restitution);
			if (runtimeFixture->GetRestitutionThreshold() != c2D.RestitutionThreshold)
				runtimeFixture->SetRestitutionThreshold(c2D.RestitutionThreshold);
			if (runtimeFixture->GetFilterData().categoryBits != c2D.CollisionCategory || runtimeFixture->GetFilterData().maskBits != c2D.CollisionMask)
			{
				b2Filter filter;
				filter.categoryBits = c2D.CollisionCategory;
				filter.maskBits = c2D.CollisionMask;
				runtimeFixture->SetFilterData(filter);
			}
		}
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
		for (auto e : view)
		{
			Entity entity = Entity(e, this);
			if (entity.GetComponent<CameraComponent>().Primary)
				return entity;
		}
		return Entity::Null;
	}

	Entity Scene::GetEntityByUUID(UUID uuid)
	{
		if (m_Entities.find(uuid) != m_Entities.end())
			return m_Entities[uuid];
		return Entity::Null;
	}

	glm::mat4 Scene::GetWorldTransform(Entity entity)
	{
		glm::mat4 transform(1.0f);
		auto& tc = entity.GetComponent<TransformComponent>();

		if (tc.Parent)
			transform = GetWorldTransform(GetEntityByUUID(tc.Parent));

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
	void Scene::OnComponentAdded<CubeRendererComponent>(Entity entity, CubeRendererComponent& component)
	{

	}

	template<>
	void Scene::OnComponentAdded<MeshRendererComponent>(Entity entity, MeshRendererComponent& component)
	{

	}

	template<>
	void Scene::OnComponentAdded<PointLightComponent>(Entity entity, PointLightComponent& component)
	{

	}

	template<>
	void Scene::OnComponentAdded<DirectionalLightComponent>(Entity entity, DirectionalLightComponent& component)
	{

	}

	template<>
	void Scene::OnComponentAdded<SpotLightComponent>(Entity entity, SpotLightComponent& component)
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
	void Scene::OnComponentAdded<ScriptComponent>(Entity entity, ScriptComponent& component)
	{

	}
}
