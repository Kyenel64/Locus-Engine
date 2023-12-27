// --- EntityCommands ---------------------------------------------------------
// Commands related to entities.
// Contains:
//	CreateEntityCommand
//	CreateChildEntityCommand
//	DestroyEntityCommand
//	DuplicateEntityCommand
//	AddComponentCommand
//	RemoveComponentCommand
#pragma once
#include "Command.h"

#include <iomanip>
#include <stack>
#include <queue>

namespace Locus
{
	// Used to store entity data before destroying the entity from the registry.
	// It is important we store the actual entity id so relationship components don't
	// point to a destroyed entity.
	static void SaveEntityData(Ref<ComponentData> data, Entity entity)
	{
		data->ID = CreateRef<IDComponent>(entity.GetComponent<IDComponent>());
		data->Tag = CreateRef<TagComponent>(entity.GetComponent<TagComponent>());
		data->Transform = CreateRef<TransformComponent>(entity.GetComponent<TransformComponent>());
		if (entity.HasComponent<ChildComponent>())
			data->Child = CreateRef<ChildComponent>(entity.GetComponent<ChildComponent>());
		if (entity.HasComponent<SpriteRendererComponent>())
			data->SpriteRenderer = CreateRef<SpriteRendererComponent>(entity.GetComponent<SpriteRendererComponent>());
		if (entity.HasComponent<CircleRendererComponent>())
			data->CircleRenderer = CreateRef<CircleRendererComponent>(entity.GetComponent<CircleRendererComponent>());
		if (entity.HasComponent<CubeRendererComponent>())
			data->CubeRenderer = CreateRef<CubeRendererComponent>(entity.GetComponent<CubeRendererComponent>());
		if (entity.HasComponent<CameraComponent>())
			data->Camera = CreateRef<CameraComponent>(entity.GetComponent<CameraComponent>());
		if (entity.HasComponent<Rigidbody2DComponent>())
			data->Rigidbody2D = CreateRef<Rigidbody2DComponent>(entity.GetComponent<Rigidbody2DComponent>());
		if (entity.HasComponent<BoxCollider2DComponent>())
			data->BoxCollider2D = CreateRef<BoxCollider2DComponent>(entity.GetComponent<BoxCollider2DComponent>());
		if (entity.HasComponent<CircleCollider2DComponent>())
			data->CircleCollider2D = CreateRef<CircleCollider2DComponent>(entity.GetComponent<CircleCollider2DComponent>());
		if (entity.HasComponent<ScriptComponent>())
			data->Script = CreateRef<ScriptComponent>(entity.GetComponent<ScriptComponent>());
	}
	
	// Assigns component data to entity. ID component is redundant since entities require ID to be created.
	static void LoadEntityData(Ref<ComponentData> data, Entity entity)
	{
		entity.GetComponent<IDComponent>() = *data->ID;
		entity.GetComponent<TagComponent>() = *data->Tag;
		entity.GetComponent<TransformComponent>() = *data->Transform;
		if (data->Child)
			entity.AddComponent<ChildComponent>(*data->Child);
		if (data->SpriteRenderer)
			entity.AddComponent<SpriteRendererComponent>(*data->SpriteRenderer);
		if (data->CircleRenderer)
			entity.AddComponent<CircleRendererComponent>(*data->CircleRenderer);
		if (data->CubeRenderer)
			entity.AddComponent<CubeRendererComponent>(*data->CubeRenderer);
		if (data->Camera)
			entity.AddComponent<CameraComponent>(*data->Camera);
		if (data->Rigidbody2D)
			entity.AddComponent<Rigidbody2DComponent>(*data->Rigidbody2D);
		if (data->BoxCollider2D)
			entity.AddComponent<BoxCollider2DComponent>(*data->BoxCollider2D);
		if (data->CircleCollider2D)
			entity.AddComponent<CircleCollider2DComponent>(*data->CircleCollider2D);
		if (data->Script)
			entity.AddComponent<ScriptComponent>(*data->Script);
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
			m_ActiveScene->CreateEntityWithUUID(m_UUID, m_EntityName);
			CommandHistory::SetEditorSavedStatus(false);
		}

		virtual void Undo() override
		{
			m_ActiveScene->DestroyEntity(m_ActiveScene->GetEntityByUUID(m_UUID));
			CommandHistory::SetEditorSavedStatus(false);
		}

		virtual bool Merge(Command* other) override
		{
			return false;
		}

	private:
		Ref<Scene> m_ActiveScene;
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
			: m_ActiveScene(activeScene), m_EntityName(name), m_UUID(UUID()), m_ParentUUID(parentEntity.GetUUID())
		{
		}

		virtual void Execute() override
		{
			// Create entity and set parent
			Entity entity = m_ActiveScene->CreateEntityWithUUID(m_UUID, m_EntityName);
			entity.GetComponent<TransformComponent>().Parent = m_ParentUUID;

			// Add entity to parent's child component
			Entity parentEntity = m_ActiveScene->GetEntityByUUID(m_ParentUUID);
			if (!parentEntity.HasComponent<ChildComponent>())
				parentEntity.AddComponent<ChildComponent>();
			auto& parentCC = parentEntity.GetComponent<ChildComponent>();

			parentCC.ChildCount++;
			parentCC.ChildEntities.push_back(m_UUID);

			CommandHistory::SetEditorSavedStatus(false);
		}

		virtual void Undo() override
		{
			Entity entity = m_ActiveScene->GetEntityByUUID(m_UUID);
			m_ActiveScene->DestroyEntity(entity);

			CommandHistory::SetEditorSavedStatus(false);
		}

		virtual bool Merge(Command* other) override
		{
			return false;
		}

	private:
		Ref<Scene> m_ActiveScene;
		UUID m_ParentUUID;
		UUID m_UUID;
		std::string m_EntityName;
	};



	// --- DestroyEntityCommand -----------------------------------------------
	// We need a way to store the data of the destroyed entity so we can
	// recreate the entity when undoing. Stored in the ComponentData struct.
	class DestroyEntityCommand : public Command
	{
	public:
		DestroyEntityCommand() = default;
		~DestroyEntityCommand() = default;

		DestroyEntityCommand(Ref<Scene> activeScene, Entity entity)
			: m_ActiveScene(activeScene), m_UUID(entity.GetUUID())
		{
			m_EntityData = CreateRef<ComponentData>();
		}

		virtual void Execute() override
		{
			Entity entity = m_ActiveScene->GetEntityByUUID(m_UUID);

			// Save data of entity and all its children
			SaveEntityData(m_EntityData, entity);
			SaveChildEntityData(entity);

			m_ActiveScene->DestroyEntity(entity);
			CommandHistory::SetEditorSavedStatus(false);
		}

		virtual void Undo() override
		{
			Entity entity = m_ActiveScene->CreateEntityWithUUID(m_EntityData->ID->ID);
			LoadEntityData(m_EntityData, entity);

			// Add entity to parent's child component
			if (entity.GetComponent<TransformComponent>().Parent)
			{
				UUID parentUUID = entity.GetComponent<TransformComponent>().Parent;
				Entity parentEntity = m_ActiveScene->GetEntityByUUID(parentUUID);
				if (!parentEntity.HasComponent<ChildComponent>())
					parentEntity.AddComponent<ChildComponent>();
				auto& parentCC = parentEntity.GetComponent<ChildComponent>();
				parentCC.ChildEntities.push_back(entity.GetUUID());
				parentCC.ChildCount++;
			}

			// Create all child entities
			while (!m_ChildEntityData.empty())
			{
				Ref<ComponentData> childData = m_ChildEntityData.top();
				Entity childEntity = m_ActiveScene->CreateEntityWithUUID(childData->ID->ID);
				LoadEntityData(childData, childEntity);
				m_ChildEntityData.pop();
			}
			CommandHistory::SetEditorSavedStatus(false);
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
				for (UUID childUUID : entity.GetComponent<ChildComponent>().ChildEntities)
				{
					Entity childEntity = m_ActiveScene->GetEntityByUUID(childUUID);
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
				for (UUID childUUID : entity.GetComponent<ChildComponent>().ChildEntities)
				{
					Entity childEntity = m_ActiveScene->GetEntityByUUID(childUUID);
					DestroyChildren(childEntity);
					m_ActiveScene->DestroyEntity(childEntity);
				}
			}
		}

	private:
		Ref<Scene> m_ActiveScene;
		UUID m_UUID;
		Ref<ComponentData> m_EntityData;
		std::stack<Ref<ComponentData>> m_ChildEntityData;
	};



	// --- DuplicateEntityCommand ------------------------------------------------
	// By far the most complicated command since data and relationships need to be properly
	// stored and assigned. Naming is also overly complicated.
	class DuplicateEntityCommand : public Command
	{
	public:
		DuplicateEntityCommand() = default;
		~DuplicateEntityCommand() = default;

		DuplicateEntityCommand(Ref<Scene> activeScene, Entity copyEntity)
			: m_ActiveScene(activeScene), m_UUID(UUID()), m_CopyUUID(copyEntity.GetUUID())
		{
			ProcessEntityName();
			m_EntityData = CreateRef<ComponentData>();

			// Initially create an entity and store its data. 
			Entity entity = m_ActiveScene->CreateEntityWithUUID(m_UUID, m_EntityName);
			m_ActiveScene->CopyAllComponents(copyEntity, entity);
			// Overriding copied data
			auto& tc = entity.GetComponent<TransformComponent>();
			tc.Self = m_UUID;
			entity.GetComponent<TagComponent>().Tag = m_EntityName;

			if (tc.Parent)
			{
				Entity parentEntity = m_ActiveScene->GetEntityByUUID(tc.Parent);
				auto& parentCC = parentEntity.GetComponent<ChildComponent>();
				parentCC.ChildEntities.push_back(m_UUID);
				parentCC.ChildCount++;
			}
			// Dont want to store the copy entity's children.
			if (entity.HasComponent<ChildComponent>())
				entity.RemoveComponent<ChildComponent>();
			CreateChildren(copyEntity, entity);

			SaveEntityData(m_EntityData, entity);
			DestroyChildren(entity);
			m_ActiveScene->DestroyEntity(entity);
		}

		virtual void Execute() override
		{
			Entity entity = m_ActiveScene->CreateEntityWithUUID(m_UUID);
			LoadEntityData(m_EntityData, entity);

			// Add entity to parent's child component
			auto& tc = entity.GetComponent<TransformComponent>();
			if (tc.Parent)
			{
				Entity parentEntity = m_ActiveScene->GetEntityByUUID(tc.Parent);
				auto& parentCC = parentEntity.GetComponent<ChildComponent>();
				parentCC.ChildEntities.push_back(m_UUID);
				parentCC.ChildCount++;
			}

			while (!m_ChildEntityData.empty())
			{
				Ref<ComponentData> childData = m_ChildEntityData.top();
				Entity childEntity = m_ActiveScene->CreateEntityWithUUID(childData->ID->ID);
				LoadEntityData(childData, childEntity);
				m_ChildEntityData.pop();
			}
			CommandHistory::SetEditorSavedStatus(false);
		}

		virtual void Undo() override
		{
			Entity entity = m_ActiveScene->GetEntityByUUID(m_UUID);

			SaveEntityData(m_EntityData, entity);
			SaveChildEntityData(entity);

			m_ActiveScene->DestroyEntity(entity);

			CommandHistory::SetEditorSavedStatus(false);
		}

		virtual bool Merge(Command* other) override
		{
			return false;
		}

	private:
		// Appends the duplicate number to the name. Works with gaps.
		// Eg. if "Entity.001", "Entity.003": adds "Entity.002" for the next value.
		// TODO: Simplify & optimize
		void ProcessEntityName()
		{
			LOCUS_PROFILE_FUNCTION();
			m_EntityName = m_ActiveScene->GetEntityByUUID(m_CopyUUID).GetComponent<TagComponent>().Tag;

			// Extract name without the extension
			size_t extensionPos = m_EntityName.find_last_of('.');
			if (extensionPos != std::string::npos)
				m_EntityName = m_EntityName.substr(0, extensionPos);

			// Store all existing extension values
			std::priority_queue<int, std::vector<int>, std::greater<int>> queue;
			auto view = m_ActiveScene->GetEntitiesWith<TagComponent>();
			for (auto e : view)
			{
				Entity entity = Entity(e, m_ActiveScene.get());
				std::string tag = entity.GetComponent<TagComponent>().Tag;
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
				for (auto& copyUUID : from.GetComponent<ChildComponent>().ChildEntities)
				{
					Entity copyEntity = m_ActiveScene->GetEntityByUUID(copyUUID);
					auto& copyTag = copyEntity.GetComponent<TagComponent>();
					UUID newUUID = UUID();
					Entity newEntity = m_ActiveScene->CreateEntityWithUUID(newUUID);
					m_ActiveScene->CopyAllComponents(copyEntity, newEntity);
					// Dont want to store the copy entity's children.
					if (copyEntity.HasComponent<ChildComponent>())
						newEntity.RemoveComponent<ChildComponent>();
					// Overriding copied data
					auto& tc = newEntity.GetComponent<TransformComponent>();
					tc.Self = newUUID;
					tc.Parent = to.GetUUID();
					Entity parentEntity = m_ActiveScene->GetEntityByUUID(tc.Parent);
					auto& parentCC = parentEntity.GetComponent<ChildComponent>();
					parentCC.ChildEntities.push_back(newUUID);
					parentCC.ChildCount++;
					CreateChildren(copyEntity, newEntity);

					Ref<ComponentData> childData = CreateRef<ComponentData>();
					SaveEntityData(childData, newEntity);
					m_ChildEntityData.push(childData);
				}
			}
		}

		void SaveChildEntityData(Entity entity)
		{
			if (entity.HasComponent<ChildComponent>())
			{
				for (UUID childUUID : entity.GetComponent<ChildComponent>().ChildEntities)
				{
					Entity childEntity = m_ActiveScene->GetEntityByUUID(childUUID);
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
				for (UUID childUUID : entity.GetComponent<ChildComponent>().ChildEntities)
				{
					Entity childEntity = m_ActiveScene->GetEntityByUUID(childUUID);
					DestroyChildren(childEntity);
					m_ActiveScene->DestroyEntity(childEntity);
				}
			}
		}

	private:
		Ref<Scene> m_ActiveScene;
		std::string m_EntityName;
		UUID m_CopyUUID;
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
		AddComponentCommand(Ref<Scene> activeScene, Entity selectedEntity, Args... args)
			: m_UUID(selectedEntity.GetUUID()), m_ActiveScene(activeScene)
		{
			m_AddFunction = [=]() { m_ActiveScene->GetEntityByUUID(m_UUID).AddComponent<T>(args...); };
		}

		~AddComponentCommand() = default;

		virtual void Execute() override
		{
			m_AddFunction();
			CommandHistory::SetEditorSavedStatus(false);
		}

		virtual void Undo() override
		{
			Entity entity = m_ActiveScene->GetEntityByUUID(m_UUID);
			entity.RemoveComponent<T>();
			CommandHistory::SetEditorSavedStatus(false);
		}

		virtual bool Merge(Command* other) override
		{
			return false;
		}

	private:
		UUID m_UUID;
		Ref<Scene> m_ActiveScene;
		std::function<void()> m_AddFunction;
	};



	// --- RemoveComponentCommand ---------------------------------------------
	template<typename T>
	class RemoveComponentCommand : public Command
	{
	public:
		RemoveComponentCommand(Ref<Scene> activeScene, Entity selectedEntity)
			: m_UUID(selectedEntity.GetUUID()), m_ActiveScene(activeScene)
		{
		}

		~RemoveComponentCommand() = default;

		virtual void Execute() override
		{
			Entity entity = m_ActiveScene->GetEntityByUUID(m_UUID);
			m_Component = entity.GetComponent<T>();
			entity.RemoveComponent<T>();
			CommandHistory::SetEditorSavedStatus(false);
		}

		virtual void Undo() override
		{
			Entity entity = m_ActiveScene->GetEntityByUUID(m_UUID);
			entity.AddComponent<T>(m_Component);
			CommandHistory::SetEditorSavedStatus(false);
		}

		virtual bool Merge(Command* other) override
		{
			return false;
		}

	private:
		UUID m_UUID;
		T m_Component;
		Ref<Scene> m_ActiveScene;
	};
}
