#pragma once
#include "Command.h"

#include <iomanip>

namespace Locus
{
	// --- CreateEntityCommand ------------------------------------------------
	class CreateEntityCommand : public Command
	{
	public:
		CreateEntityCommand() = default;
		~CreateEntityCommand() = default;

		CreateEntityCommand(Ref<Scene> activeScene, const std::string& name)
			: m_ActiveScene(activeScene), m_EntityName(name), m_UUID(UUID())
		{
		}

		// Used for duplicating existing entities
		CreateEntityCommand(Ref<Scene> activeScene, const std::string& name, Entity copyEntity)
			: m_ActiveScene(activeScene), m_EntityName(name), m_UUID(UUID()), m_CopyEntity(copyEntity)
		{
			auto view = m_ActiveScene->m_Registry.view<TagComponent>();
			int dupAmount = 0;

			size_t entityNameDupPos = m_EntityName.find_last_of('.'); // TODO: Work with multiple .'s

			if (entityNameDupPos != std::string::npos)
				m_EntityName = m_EntityName.substr(0, entityNameDupPos);

			LOCUS_CORE_INFO(m_EntityName);
			for (auto entity : view)
			{
				std::string tagWithDup = m_ActiveScene->m_Registry.get<TagComponent>(entity).Tag;
				size_t dupExtensionPos = tagWithDup.find_last_of('.');
				std::string tag = tagWithDup.substr(0, dupExtensionPos);
				if (tag.find(m_EntityName) != std::string::npos)
					dupAmount++;
			}
			if (dupAmount)
			{
				std::stringstream ss;
				ss << std::setw(3) << std::setfill('0') << dupAmount;
				m_EntityName.append(".");
				m_EntityName.append(ss.str());
			}
		}

		virtual void Execute() override
		{
			m_Entity = m_ActiveScene->CreateEntityWithUUID(m_UUID, m_EntityName);
			Application::Get().SetIsSavedStatus(false);
		}

		virtual void Undo() override
		{
			m_ActiveScene->DestroyEntity(m_Entity);
			Application::Get().SetIsSavedStatus(false);
		}

		virtual bool Merge(Command* other) override
		{
			return false;
		}

	private:
		Ref<Scene> m_ActiveScene;
		Entity m_Entity;
		Entity m_CopyEntity;
		UUID m_UUID;
		std::string m_EntityName;
	};



	// --- DestroyEntityCommand -----------------------------------------------
	class DestroyEntityCommand : public Command
	{
	public:
		DestroyEntityCommand() = default;
		~DestroyEntityCommand() = default;

		DestroyEntityCommand(Ref<Scene> activeScene, Entity entity)
			: m_ActiveScene(activeScene), m_Entity(entity), m_UUID(UUID())
		{
		}

		virtual void Execute() override
		{
			// Hold component data. Try to make this scalable and not hard coded.
			m_Components.Tag = m_Entity.GetComponent<TagComponent>();
			m_Components.Transform = m_Entity.GetComponent<TransformComponent>();

			if (m_Entity.HasComponent<SpriteRendererComponent>())
			{
				m_Components.SpriteRenderer = m_Entity.GetComponent<SpriteRendererComponent>();
				m_AvailableComponents["SpriteRenderer"] = true;
			}
			if (m_Entity.HasComponent<CameraComponent>())
			{
				m_Components.Camera = m_Entity.GetComponent<CameraComponent>();
				m_AvailableComponents["Camera"] = true;
			}
			if (m_Entity.HasComponent<NativeScriptComponent>())
			{
				m_Components.NativeScript = m_Entity.GetComponent<NativeScriptComponent>();
				m_AvailableComponents["NativeScript"] = true;
			}
			if (m_Entity.HasComponent<Rigidbody2DComponent>())
			{
				m_Components.Rigidbody2D = m_Entity.GetComponent<Rigidbody2DComponent>();
				m_AvailableComponents["Rigidbody2D"] = true;
			}
			if (m_Entity.HasComponent<BoxCollider2DComponent>())
			{
				m_Components.BoxCollider2D = m_Entity.GetComponent<BoxCollider2DComponent>();
				m_AvailableComponents["BoxCollider2D"] = true;
			}

			m_ActiveScene->DestroyEntity(m_Entity);
			Application::Get().SetIsSavedStatus(false);
		}

		virtual void Undo() override
		{
			m_Entity = m_ActiveScene->CreateEntityWithUUID(m_UUID, m_Components.Tag.Tag);

			m_Entity.GetComponent<TransformComponent>().Translation = m_Components.Transform.Translation;
			m_Entity.GetComponent<TransformComponent>().Scale = m_Components.Transform.Scale;
			m_Entity.GetComponent<TransformComponent>().SetRotationEuler(m_Components.Transform.GetRotationEuler());

			if (m_AvailableComponents["SpriteRenderer"])
				m_Entity.AddComponent<SpriteRendererComponent>(m_Components.SpriteRenderer);
			if (m_AvailableComponents["Camera"])
				m_Entity.AddComponent<CameraComponent>(m_Components.Camera);
			if (m_AvailableComponents["NativeScript"])
				m_Entity.AddComponent<NativeScriptComponent>(m_Components.NativeScript);
			if (m_AvailableComponents["Rigidbody2D"])
				m_Entity.AddComponent<Rigidbody2DComponent>(m_Components.Rigidbody2D);
			if (m_AvailableComponents["BoxCollider2D"])
				m_Entity.AddComponent<BoxCollider2DComponent>(m_Components.BoxCollider2D);
			Application::Get().SetIsSavedStatus(false);
		}

		virtual bool Merge(Command* other) override
		{
			return false;
		}

	private:
		Ref<Scene> m_ActiveScene;
		ComponentsList m_Components;
		std::unordered_map<std::string, bool> m_AvailableComponents;
		Entity m_Entity;
		UUID m_UUID;
	};


	// --- AddComponentCommand ------------------------------------------------
	template<typename T>
	class AddComponentCommand : public Command
	{
	public:
		template<typename... Args>
		AddComponentCommand(Entity selectedEntity, Args... args)
			: m_Entity(selectedEntity)
		{
			m_AddFunction = [=]() { m_Entity.AddComponent<T>(args...); };
		}
		~AddComponentCommand() = default;

		virtual void Execute() override
		{
			m_AddFunction();
			Application::Get().SetIsSavedStatus(false);
		}

		virtual void Undo() override
		{
			m_Entity.RemoveComponent<T>();
			Application::Get().SetIsSavedStatus(false);
		}

		virtual bool Merge(Command* other) override
		{
			return false;
		}

	private:
		Entity m_Entity;
		std::function<void()> m_AddFunction;
	};



	// --- RemoveComponentCommand ---------------------------------------------
	template<typename T>
	class RemoveComponentCommand : public Command
	{
	public:
		RemoveComponentCommand(Entity selectedEntity)
			: m_Entity(selectedEntity)
		{
		}

		~RemoveComponentCommand() = default;

		virtual void Execute() override
		{
			m_Component = m_Entity.GetComponent<T>();
			m_Entity.RemoveComponent<T>();
			Application::Get().SetIsSavedStatus(false);
		}

		virtual void Undo() override
		{
			m_Entity.AddComponent<T>(m_Component);
			Application::Get().SetIsSavedStatus(false);
		}

		virtual bool Merge(Command* other) override
		{
			return false;
		}

	private:
		Entity m_Entity;
		T m_Component;
	};
}