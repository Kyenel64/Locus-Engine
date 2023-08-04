#pragma once
#include "Command.h"

#include "SideA/Core/Log.h"

#include "SideA/Scene/Scene.h"
#include "SideA/Core/Application.h"

namespace SideA
{
	struct ComponentsList
	{
		TagComponent Tag;
		TransformComponent Transform;
		SpriteRendererComponent SpriteRenderer;
		CameraComponent Camera;
		NativeScriptComponent NativeScript;
	};



	// --- CreateEntityCommand ------------------------------------------------
	class CreateEntityCommand : public Command
	{
	public:
		CreateEntityCommand() = default;

		CreateEntityCommand(Ref<Scene> activeScene, const std::string& name)
			: m_ActiveScene(activeScene), m_EntityName(name)
		{
		}

		virtual void Execute() override
		{
			m_Entity = m_ActiveScene->CreateEntity(m_EntityName);
			Application::Get().SetIsSavedStatus(false);
		}

		virtual void Undo() override
		{
			m_ActiveScene->DestroyEntity(m_Entity);
			Application::Get().SetIsSavedStatus(false);
		}

	private:
		Ref<Scene> m_ActiveScene;
		Entity m_Entity;
		std::string m_EntityName;
	};



	// --- DestroyEntityCommand -----------------------------------------------
	class DestroyEntityCommand : public Command
	{
	public:
		DestroyEntityCommand() = default;

		DestroyEntityCommand(Ref<Scene> activeScene, Entity entity)
			: m_ActiveScene(activeScene), m_Entity(entity)
		{
		}

		virtual void Execute() override
		{
			// Hold component data. Try to make this scalable and not hard coded.
			if (m_Entity.HasComponent<TagComponent>())
			{
				m_Components.Tag = m_Entity.GetComponent<TagComponent>();
				m_AvailableComponents["Tag"] = true;
			}
			if (m_Entity.HasComponent<TransformComponent>())
			{
				m_Components.Transform = m_Entity.GetComponent<TransformComponent>();
				m_AvailableComponents["Transform"] = true;
			}
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

			m_ActiveScene->DestroyEntity(m_Entity);
			Application::Get().SetIsSavedStatus(false);
		}

		virtual void Undo() override
		{
			// TODO: UUIDs. Currently this will create a new entity id and wont be able to add
			//				components to this when undoing/redoing.
			m_Entity = m_ActiveScene->CreateEntity(m_Components.Tag.Tag);

			m_Entity.GetComponent<TransformComponent>().Translation = m_Components.Transform.Translation;
			m_Entity.GetComponent<TransformComponent>().Scale = m_Components.Transform.Scale;
			m_Entity.GetComponent<TransformComponent>().SetRotationEuler(m_Components.Transform.GetRotationEuler());

			if (m_AvailableComponents["SpriteRenderer"])
				m_Entity.AddComponent<SpriteRendererComponent>(m_Components.SpriteRenderer);
			if (m_AvailableComponents["Camera"])
				m_Entity.AddComponent<CameraComponent>(m_Components.Camera);
			if (m_AvailableComponents["NativeScript"])
				m_Entity.AddComponent<NativeScriptComponent>(m_Components.NativeScript);
			Application::Get().SetIsSavedStatus(false);
		}

	private:
		Ref<Scene> m_ActiveScene;
		ComponentsList m_Components;
		std::unordered_map<std::string, bool> m_AvailableComponents;
		Entity m_Entity;
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

	private:
		Entity m_Entity;
		T m_Component;
	};
}