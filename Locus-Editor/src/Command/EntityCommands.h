#pragma once
#include "Command.h"

#include <iomanip>
#include <stack>

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

			auto& entityRC = m_Entity.GetComponent<RelationshipComponent>();
			auto& parentRC = m_ParentEntity.GetComponent<RelationshipComponent>();

			// Set Parent, Next, and Prev entity relationships
			entityRC.Parent = m_ParentEntity;
			if (parentRC.ChildCount == 0)
			{
				parentRC.FirstChild = m_Entity;
			}
			else
			{
				Entity curEntity = parentRC.FirstChild;
				while (curEntity != Entity::Null)
				{
					auto& rc = curEntity.GetComponent<RelationshipComponent>();
					if (rc.Next == Entity::Null)
					{
						rc.Next = m_Entity;
						entityRC.Prev = curEntity;
						break;
					}
					curEntity = rc.Next;
				}
			}
			parentRC.ChildCount++;

			Application::Get().SetIsSavedStatus(false);
		}

		virtual void Undo() override
		{
			auto& parentRC = m_ParentEntity.GetComponent<RelationshipComponent>();
			auto& entityRC = m_Entity.GetComponent<RelationshipComponent>();
			Entity prevEntity = entityRC.Prev;

			if (parentRC.FirstChild == m_Entity)
				parentRC.FirstChild = entityRC.Next;
			else
				prevEntity.GetComponent<RelationshipComponent>().Next = entityRC.Next;

			parentRC.ChildCount--;

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
			: m_ActiveScene(activeScene), m_Entity(entity), m_UUID(UUID()), m_Graveyard(m_ActiveScene->GetGraveyard())
		{
		}

		virtual void Execute() override
		{
			LOCUS_CORE_INFO("Deleting: {0}, ID: {1}", m_Entity.GetComponent<TagComponent>().Tag, (uint32_t)m_Entity);
			m_OldEntity = m_Graveyard->AddEntity(m_Entity);
			auto& entityRC = m_Entity.GetComponent<RelationshipComponent>();

			// Destroy all children
			if (entityRC.ChildCount)
			{
				Entity firstEntity = entityRC.FirstChild;
				DestroyChildEntities(firstEntity);
			}

			ProcessEntityRelationships(m_Entity);

			m_ActiveScene->DestroyEntity(m_Entity);
			Application::Get().SetIsSavedStatus(false);
		}

		virtual void Undo() override
		{
			auto& entityRC = m_Graveyard->m_Registry.get<RelationshipComponent>(m_OldEntity);
			Entity parentEntity = entityRC.Parent;
			Entity prevEntity = entityRC.Prev;
			Entity nextEntity = entityRC.Next;

			m_Entity = m_Graveyard->MoveEntityToScene(m_OldEntity, m_ActiveScene);

			// Recreate all the child entities
			while (!m_DeletedChildEntities.empty())
			{
				m_Graveyard->MoveEntityToScene(m_DeletedChildEntities.top(), m_ActiveScene);
				m_DeletedChildEntities.pop();
			}

			switch (m_DeletionCase)
			{
			case Locus::DestroyEntityCommand::DeletionCase::FirstChild:
			{
				auto& parentRC = parentEntity.GetComponent<RelationshipComponent>();
				if (nextEntity != Entity::Null)
				{
					auto& nextRC = nextEntity.GetComponent<RelationshipComponent>();
					nextRC.Prev = m_Entity;
				}
				parentRC.FirstChild = m_Entity;
				parentRC.ChildCount++;
				break;
			}
			case Locus::DestroyEntityCommand::DeletionCase::MiddleChild:
			{
				auto& parentRC = parentEntity.GetComponent<RelationshipComponent>();
				auto& prevRC = prevEntity.GetComponent<RelationshipComponent>();
				auto& nextRC = nextEntity.GetComponent<RelationshipComponent>();
				prevRC.Next = m_Entity;
				nextRC.Prev = m_Entity;
				parentRC.ChildCount++;
				break;
			}
			case Locus::DestroyEntityCommand::DeletionCase::LastChild:
			{
				auto& parentRC = parentEntity.GetComponent<RelationshipComponent>();
				parentRC.ChildCount++;
				auto& prevRC = prevEntity.GetComponent<RelationshipComponent>();
				prevRC.Next = m_Entity;
				break;
			}
			case Locus::DestroyEntityCommand::DeletionCase::Parent:
				break;
			default:
				break;
			}

			Application::Get().SetIsSavedStatus(false);
		}

		virtual bool Merge(Command* other) override
		{
			return false;
		}

	private:
		void ProcessEntityRelationships(Entity entity)
		{
			auto& entityRC = entity.GetComponent<RelationshipComponent>();
			if (entityRC.Parent != Entity::Null)
			{
				Entity parentEntity = entityRC.Parent;
				auto& parentRC = parentEntity.GetComponent<RelationshipComponent>();

				// Case: First child
				if (parentRC.FirstChild == entity)
				{
					parentRC.FirstChild = entityRC.Next;
					if (entityRC.Next != Entity::Null)
					{
						Entity nextEntity = entityRC.Next;
						nextEntity.GetComponent<RelationshipComponent>().Prev = Entity::Null;
					}
					m_DeletionCase = DeletionCase::FirstChild;
				}

				// Case: Destroy middle child
				if (entityRC.Next != Entity::Null && entityRC.Prev != Entity::Null)
				{
					Entity prevEntity = entityRC.Prev;
					auto& prevRC = prevEntity.GetComponent<RelationshipComponent>();
					prevRC.Next = entityRC.Next;

					Entity nextEntity = entityRC.Next;
					auto& nextRC = nextEntity.GetComponent<RelationshipComponent>();
					nextRC.Prev = entityRC.Prev;

					m_DeletionCase = DeletionCase::MiddleChild;
				}
				// Case: Destroy last child
				if (entityRC.Next == Entity::Null && entityRC.Prev != Entity::Null)
				{
					Entity prevEntity = entityRC.Prev;
					auto& prevRC = prevEntity.GetComponent<RelationshipComponent>();
					prevRC.Next = Entity::Null;
					m_DeletionCase = DeletionCase::LastChild;
				}
				parentRC.ChildCount--;
			}
			else
			{
				m_DeletionCase = DeletionCase::Parent;
			}
		}

		// Recursively move child entities to graveyard and destroy
		void DestroyChildEntities(Entity entity)
		{
			if (entity != Entity::Null)
			{
				auto& entityRC = entity.GetComponent<RelationshipComponent>();
				m_DeletedChildEntities.push(m_Graveyard->AddEntity(entity));

				if (entityRC.Next != Entity::Null)
				{
					Entity nextEntity = entityRC.Next;
					DestroyChildEntities(nextEntity);
				}

				Entity childEntity = entityRC.FirstChild;
				ProcessEntityRelationships(entity);
				DestroyChildEntities(childEntity);
				m_ActiveScene->DestroyEntity(entity);
			}
		}

	private:
		Ref<Scene> m_ActiveScene;
		Ref<Graveyard> m_Graveyard;
		Entity m_Entity;
		entt::entity m_OldEntity;
		std::stack<entt::entity> m_DeletedChildEntities;
		UUID m_UUID;

		enum class DeletionCase { FirstChild = 0, MiddleChild = 1, LastChild = 2, Parent = 3 };
		DeletionCase m_DeletionCase;
	};



	// --- DuplicateEntityCommand ------------------------------------------------
	class DuplicateEntityCommand : public Command
	{
	public:
		DuplicateEntityCommand() = default;
		~DuplicateEntityCommand() = default;

		// Used for duplicating existing entities
		DuplicateEntityCommand(Ref<Scene> activeScene, const std::string& name, Entity copyEntity)
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
			m_ActiveScene->CopyAllComponents(m_CopyEntity, m_Entity);
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