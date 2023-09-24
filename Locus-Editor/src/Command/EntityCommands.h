#pragma once
#include "Command.h"

#include <iomanip>
#include <stack>
#include <queue>

namespace Locus
{

	static void SaveEntityData(Ref<ComponentData> data, Entity entity)
	{
		data->EntityID = entity;
		data->ID = entity.GetComponent<IDComponent>();
		data->Tag = entity.GetComponent<TagComponent>();
		data->Transform = entity.GetComponent<TransformComponent>();
		if (entity.HasComponent<ChildComponent>())
		{
			data->Child = entity.GetComponent<ChildComponent>();
			data->Components.push(ComponentType::Child);
		}
		if (entity.HasComponent<SpriteRendererComponent>())
		{
			data->SpriteRenderer = entity.GetComponent<SpriteRendererComponent>();
			data->Components.push(ComponentType::SpriteRenderer);
		}
		if (entity.HasComponent<CircleRendererComponent>())
		{
			data->CircleRenderer = entity.GetComponent<CircleRendererComponent>();
			data->Components.push(ComponentType::CircleRenderer);
		}
		if (entity.HasComponent<CameraComponent>())
		{
			data->Camera = entity.GetComponent<CameraComponent>();
			data->Components.push(ComponentType::Camera);
		}
		if (entity.HasComponent<Rigidbody2DComponent>())
		{
			data->Rigidbody2D = entity.GetComponent<Rigidbody2DComponent>();
			data->Components.push(ComponentType::Rigidbody2D);
		}
		if (entity.HasComponent<BoxCollider2DComponent>())
		{
			data->BoxCollider2D = entity.GetComponent<BoxCollider2DComponent>();
			data->Components.push(ComponentType::BoxCollider2D);
		}
		if (entity.HasComponent<CircleCollider2DComponent>())
		{
			data->CircleCollider2D = entity.GetComponent<CircleCollider2DComponent>();
			data->Components.push(ComponentType::CircleCollider2D);
		}
		if (entity.HasComponent<NativeScriptComponent>())
		{
			data->NativeScript = entity.GetComponent<NativeScriptComponent>();
			data->Components.push(ComponentType::NativeScript);
		}
	}

	static void LoadEntityData(Ref<ComponentData> data, Entity entity)
	{
		entity.AddOrReplaceComponent<TagComponent>(data->Tag);
		entity.AddOrReplaceComponent<TransformComponent>(data->Transform);

		while (!data->Components.empty())
		{
			switch (data->Components.top())
			{
			case ComponentType::Child: entity.AddComponent<ChildComponent>(data->Child); break;
			case ComponentType::SpriteRenderer: entity.AddComponent<SpriteRendererComponent>(data->SpriteRenderer); break;
			case ComponentType::CircleRenderer: entity.AddComponent<CircleRendererComponent>(data->CircleRenderer); break;
			case ComponentType::Camera: entity.AddComponent<CameraComponent>(data->Camera); break;
			case ComponentType::Rigidbody2D: entity.AddComponent<Rigidbody2DComponent>(data->Rigidbody2D); break;
			case ComponentType::BoxCollider2D: entity.AddComponent<BoxCollider2DComponent>(data->BoxCollider2D); break;
			case ComponentType::CircleCollider2D: entity.AddComponent<CircleCollider2DComponent>(data->CircleCollider2D); break;
			case ComponentType::NativeScript: entity.AddComponent<NativeScriptComponent>(data->NativeScript); break;
			}
			data->Components.pop();
		}
	}



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

		virtual void Execute() override
		{
			m_Entity = m_ActiveScene->CreateEntityWithUUID(m_Entity, m_UUID, m_EntityName);
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
		UUID m_UUID;
		std::string m_EntityName;
	};



	// --- CreateChildEntityCommand -------------------------------------------
	class CreateChildEntityCommand : public Command
	{
	public:
		CreateChildEntityCommand() = default;
		~CreateChildEntityCommand() = default;

		CreateChildEntityCommand(Ref<Scene> activeScene, const std::string& name, Entity parentEntity)
			: m_ActiveScene(activeScene), m_EntityName(name), m_UUID(UUID()), m_ParentEntity(parentEntity)
		{
		}

		virtual void Execute() override
		{
			m_Entity = m_ActiveScene->CreateEntityWithUUID(m_Entity, m_UUID, m_EntityName);
			
			if (!m_ParentEntity.HasComponent<ChildComponent>())
				m_ParentEntity.AddComponent<ChildComponent>();

			auto& parentCC = m_ParentEntity.GetComponent<ChildComponent>();
			parentCC.ChildEntities.push_back(m_Entity);
			parentCC.ChildCount++;
			m_Entity.GetComponent<TransformComponent>().Parent = m_ParentEntity;
			Application::Get().SetIsSavedStatus(false);
		}

		virtual void Undo() override
		{
			auto& parentCC = m_ParentEntity.GetComponent<ChildComponent>();
			parentCC.ChildEntities.erase(std::find(parentCC.ChildEntities.begin(), parentCC.ChildEntities.end(), m_Entity));
			parentCC.ChildCount--;

			if (parentCC.ChildCount == 0)
				m_ParentEntity.RemoveComponent<ChildComponent>();

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
		Entity m_ParentEntity = Entity::Null;
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
			: m_ActiveScene(activeScene), m_Entity(entity)
		{
			m_EntityData = CreateRef<ComponentData>();
		}

		virtual void Execute() override
		{
			// Remove entity from parent's child component
			if (m_Entity.GetComponent<TransformComponent>().Parent != Entity::Null)
			{
				Entity parent = m_Entity.GetComponent<TransformComponent>().Parent;
				auto& parentCC = parent.GetComponent<ChildComponent>();
				parentCC.ChildEntities.erase(std::find(parentCC.ChildEntities.begin(), parentCC.ChildEntities.end(), m_Entity));
				parentCC.ChildCount--;

				if (parentCC.ChildCount == 0)
					parent.RemoveComponent<ChildComponent>();
			}

			SaveEntityData(m_EntityData, m_Entity);
			SaveChildEntityData(m_Entity);

			DestroyChildren(m_Entity);
			m_ActiveScene->DestroyEntity(m_Entity);
			Application::Get().SetIsSavedStatus(false);
		}

		virtual void Undo() override
		{
			m_Entity = m_ActiveScene->CreateEntityWithUUID(m_EntityData->EntityID, m_EntityData->ID.ID);
			LoadEntityData(m_EntityData, m_Entity);

			if (m_Entity.GetComponent<TransformComponent>().Parent != Entity::Null)
			{
				Entity parent = m_Entity.GetComponent<TransformComponent>().Parent;
				if (!parent.HasComponent<ChildComponent>())
					parent.AddComponent<ChildComponent>();
				auto& parentCC = parent.GetComponent<ChildComponent>();
				parentCC.ChildEntities.push_back(m_Entity);
				parentCC.ChildCount++;
			}

			while (!m_ChildEntityData.empty())
			{
				Ref<ComponentData> childData = m_ChildEntityData.top();
				Entity childEntity = m_ActiveScene->CreateEntityWithUUID(childData->EntityID, childData->ID.ID);
				LoadEntityData(childData, childEntity);
				m_ChildEntityData.pop();
			}
			Application::Get().SetIsSavedStatus(false);
		}

		virtual bool Merge(Command* other) override
		{
			return false;
		}

	private:
		void SaveChildEntityData(Entity entity)
		{
			if (entity.HasComponent<ChildComponent>())
			{
				for (Entity childEntity : entity.GetComponent<ChildComponent>().ChildEntities)
				{
					Ref<ComponentData> data = CreateRef<ComponentData>();
					SaveEntityData(data, childEntity);
					m_ChildEntityData.push(data);
					SaveChildEntityData(childEntity);
				}
			}
		}
		void DestroyChildren(Entity entity)
		{
			if (entity.HasComponent<ChildComponent>())
			{
				for (auto childEntity : entity.GetComponent<ChildComponent>().ChildEntities)
				{
					DestroyChildren(childEntity);
					m_ActiveScene->DestroyEntity(childEntity);
				}
			}
		}

	private:
		Ref<Scene> m_ActiveScene;
		Entity m_Entity;
		Ref<ComponentData> m_EntityData;
		std::stack<Ref<ComponentData>> m_ChildEntityData;
	};



	// --- DuplicateEntityCommand ------------------------------------------------
	class DuplicateEntityCommand : public Command
	{
	public:
		DuplicateEntityCommand() = default;
		~DuplicateEntityCommand() = default;

		DuplicateEntityCommand(Ref<Scene> activeScene, Entity copyEntity)
			: m_ActiveScene(activeScene), m_CopyEntity(copyEntity)
		{
			ProcessEntityName();
			m_EntityData = CreateRef<ComponentData>();

			// Initially create an entity and store its data.
			m_Entity = m_ActiveScene->CreateEntityWithUUID(UUID(), m_EntityName, m_CopyEntity.GetComponent<TagComponent>().Enabled);
			m_ActiveScene->CopyAllComponents(m_CopyEntity, m_Entity);
			if (m_Entity.HasComponent<ChildComponent>())
				m_Entity.RemoveComponent<ChildComponent>();

			CreateChildren(m_CopyEntity, m_Entity);

			SaveEntityData(m_EntityData, m_Entity);
			m_ActiveScene->DestroyEntity(m_Entity);
		}

		virtual void Execute() override
		{
			m_Entity = m_ActiveScene->CreateEntityWithUUID(m_EntityData->EntityID, m_EntityData->ID.ID);
			LoadEntityData(m_EntityData, m_Entity);
			// Overriding copied data
			auto& tc = m_Entity.GetComponent<TransformComponent>();
			tc.Self = m_Entity;
			m_Entity.GetComponent<TagComponent>().Tag = m_EntityName;
			// Add entity to parent's child component
			if (tc.Parent != Entity::Null)
			{
				auto& parentCC = tc.Parent.GetComponent<ChildComponent>();
				parentCC.ChildEntities.push_back(m_Entity);
				parentCC.ChildCount++;
			}

			while (!m_ChildEntityData.empty())
			{
				Ref<ComponentData> childData = m_ChildEntityData.top();
				Entity childEntity = m_ActiveScene->CreateEntityWithUUID(childData->EntityID, childData->ID.ID);
				LoadEntityData(childData, childEntity);
				m_ChildEntityData.pop();
			}

			Application::Get().SetIsSavedStatus(false);
		}

		virtual void Undo() override
		{
			// Remove entity from parent's child component
			if (m_Entity.GetComponent<TransformComponent>().Parent != Entity::Null)
			{
				Entity parent = m_Entity.GetComponent<TransformComponent>().Parent;
				auto& parentCC = parent.GetComponent<ChildComponent>();
				parentCC.ChildEntities.erase(std::find(parentCC.ChildEntities.begin(), parentCC.ChildEntities.end(), m_Entity));
				parentCC.ChildCount--;

				if (parentCC.ChildCount == 0)
					parent.RemoveComponent<ChildComponent>();
			}

			SaveEntityData(m_EntityData, m_Entity);
			SaveChildEntityData(m_Entity);

			DestroyChildren(m_Entity);
			m_ActiveScene->DestroyEntity(m_Entity);

			Application::Get().SetIsSavedStatus(false);
		}

		virtual bool Merge(Command* other) override
		{
			return false;
		}

	private:
		// Process to get proper extension number for duplicate entities. This assigns the right value if 
		// there are gaps in existing extension values. Eg. if .001, .003: adds .002 for the next value.
		// Seems overkill for such a simple task. Try to clean up.
		void ProcessEntityName()
		{
			LOCUS_PROFILE_FUNCTION();
			m_EntityName = m_CopyEntity.GetComponent<TagComponent>().Tag;

			#if 1
			// Extract name without the extension
			size_t extensionPos = m_EntityName.find_last_of('.');
			if (extensionPos != std::string::npos)
				m_EntityName = m_EntityName.substr(0, extensionPos);

			// Store all existing extension values
			std::priority_queue<int, std::vector<int>, std::greater<int>> queue;
			auto view = m_ActiveScene->m_Registry.view<TagComponent>();
			for (auto entity : view)
			{
				std::string tag = m_ActiveScene->m_Registry.get<TagComponent>(entity).Tag;
				extensionPos = tag.find_last_of('.');
				if (tag.find(m_EntityName) != std::string::npos && extensionPos != std::string::npos)
					queue.push(std::stoi(tag.substr(extensionPos + 1, tag.back())));
				else if (tag == m_EntityName && queue.size() && queue.top() != 0)
					queue.push(0);
			}

			// Sets the extension value to the largest extension value + 1. Unless there is a gap in 
			// extension values, then sets the value to fill in those gaps.
			int duplicateValue = 1;
			while (!queue.empty())
			{
				duplicateValue = queue.top() + 1;
				queue.pop();
				if (queue.size() && queue.top() != duplicateValue)
					break;
			}
			#endif

			// Simpler naming but doesnt fill gaps
			#if 0
			size_t extensionPos = m_EntityName.find_last_of('.');
			if (extensionPos != std::string::npos)
				m_EntityName = m_EntityName.substr(0, extensionPos);

			int duplicateValue = 1;
			auto view = m_ActiveScene->m_Registry.view<TagComponent>();
			for (auto entity : view)
			{
				std::string tag = m_ActiveScene->m_Registry.get<TagComponent>(entity).Tag;
				if (tag.find(m_EntityName) != std::string::npos && tag != m_EntityName)
				{
					extensionPos = tag.find_last_of('.');
					int val = std::stoi(tag.substr(extensionPos + 1, tag.back()));
					if (val + 1 > duplicateValue)
						duplicateValue = val + 1;
				}
			}
			#endif


			// Sets formatting of extension. (entity name + ".___") Eg. "EntityName.005"
			std::stringstream ss;
			ss << std::setw(3) << std::setfill('0') << duplicateValue;
			if (m_EntityName.find_last_of('.') == std::string::npos)
				m_EntityName.append("." + ss.str());
			else
				m_EntityName.replace(m_EntityName.find_last_of('.') + 1, 3, ss.str());
		}

		void CreateChildren(Entity from, Entity to)
		{
			if (from.HasComponent<ChildComponent>())
			{
				to.AddComponent<ChildComponent>();
				for (auto copyEntity : from.GetComponent<ChildComponent>().ChildEntities)
				{
					auto copyTag = copyEntity.GetComponent<TagComponent>();
					Entity newEntity = m_ActiveScene->CreateEntityWithUUID(UUID(), copyTag.Tag, copyTag.Enabled);
					m_ActiveScene->CopyAllComponents(copyEntity, newEntity);
					// Dont want to have m_CopyEntity's children.
					if (copyEntity.HasComponent<ChildComponent>())
						newEntity.RemoveComponent<ChildComponent>();
					// Overriding copied data
					auto& tc = newEntity.GetComponent<TransformComponent>();
					tc.Self = newEntity;
					tc.Parent = to;
					auto& parentCC = tc.Parent.GetComponent<ChildComponent>();
					parentCC.ChildEntities.push_back(newEntity);
					parentCC.ChildCount++;
					CreateChildren(copyEntity, newEntity);

					Ref<ComponentData> childData = CreateRef<ComponentData>();
					SaveEntityData(childData, newEntity);
					m_ChildEntityData.push(childData);
					m_ActiveScene->DestroyEntity(newEntity);
				}
			}
		}

		void SaveChildEntityData(Entity entity)
		{
			if (entity.HasComponent<ChildComponent>())
			{
				for (Entity childEntity : entity.GetComponent<ChildComponent>().ChildEntities)
				{
					Ref<ComponentData> data = CreateRef<ComponentData>();
					SaveEntityData(data, childEntity);
					m_ChildEntityData.push(data);
					SaveChildEntityData(childEntity);
				}
			}
		}

		void DestroyChildren(Entity entity)
		{
			if (entity.HasComponent<ChildComponent>())
			{
				for (auto childEntity : entity.GetComponent<ChildComponent>().ChildEntities)
				{
					DestroyChildren(childEntity);
					m_ActiveScene->DestroyEntity(childEntity);
				}
			}
		}


	private:
		Ref<Scene> m_ActiveScene;
		std::string m_EntityName;
		Entity m_Entity;
		Entity m_CopyEntity;
		UUID m_UUID;
		Ref<ComponentData> m_EntityData;
		std::stack<Ref<ComponentData>> m_ChildEntityData;
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