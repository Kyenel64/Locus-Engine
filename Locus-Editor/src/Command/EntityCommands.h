#pragma once
#include "Command.h"

#include <iomanip>
#include <stack>
#include <queue>

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
			: m_ActiveScene(activeScene), m_Entity(entity), m_Graveyard(m_ActiveScene->GetGraveyard())
		{
		}

		virtual void Execute() override
		{
			DestroyChildren(m_Entity);

			if (m_Entity.GetComponent<TransformComponent>().Parent != Entity::Null)
			{
				Entity parent = m_Entity.GetComponent<TransformComponent>().Parent;
				auto& parentCC = parent.GetComponent<ChildComponent>();
				parentCC.ChildEntities.erase(std::find(parentCC.ChildEntities.begin(), parentCC.ChildEntities.end(), m_Entity));
				parentCC.ChildCount--;

				if (parentCC.ChildCount == 0)
					parent.RemoveComponent<ChildComponent>();
			}

			m_OldEntity = m_Graveyard->AddEntity(m_Entity);
			m_ActiveScene->DestroyEntity(m_Entity);
			Application::Get().SetIsSavedStatus(false);
		}

		virtual void Undo() override
		{
			m_Entity = m_Graveyard->MoveEntityToScene(m_OldEntity, m_ActiveScene);
			if (!m_Entity.HasComponent<ChildComponent>())
				m_Entity.AddComponent<ChildComponent>();

			while (!m_ChildEntities.empty())
			{
				m_Graveyard->MoveEntityToScene(m_ChildEntities.top(), m_ActiveScene);
				m_ChildEntities.pop();
			}

			if (m_Entity.GetComponent<TransformComponent>().Parent != Entity::Null)
			{
				Entity parent = m_Entity.GetComponent<TransformComponent>().Parent;
				if (!parent.HasComponent<ChildComponent>())
					parent.AddComponent<ChildComponent>();
				auto& parentCC = parent.GetComponent<ChildComponent>();
				parentCC.ChildEntities.push_back(m_Entity);
				parentCC.ChildCount++;
			}

			Application::Get().SetIsSavedStatus(false);
		}

		virtual bool Merge(Command* other) override
		{
			return false;
		}

	private:
		void DestroyChildren(Entity entity)
		{
			if (entity.HasComponent<ChildComponent>())
			{
				for (auto childEntity : entity.GetComponent<ChildComponent>().ChildEntities)
				{
					DestroyChildren(childEntity);
					m_ChildEntities.push((entt::entity)childEntity);
					m_Graveyard->AddEntity(childEntity);
					m_ActiveScene->DestroyEntity(childEntity);
				}
			}
			
		}

	private:
		Ref<Scene> m_ActiveScene;
		Ref<Graveyard> m_Graveyard;
		Entity m_Entity;
		entt::entity m_OldEntity;
		std::stack<entt::entity> m_ChildEntities;

		enum class DeletionCase { FirstChild = 0, MiddleChild = 1, LastChild = 2, Parent = 3 };
		DeletionCase m_DeletionCase;
	};



	// --- DuplicateEntityCommand ------------------------------------------------
	class DuplicateEntityCommand : public Command
	{
	public:
		DuplicateEntityCommand() = default;
		~DuplicateEntityCommand() = default;

		DuplicateEntityCommand(Ref<Scene> activeScene, const std::string& name, Entity copyEntity)
			: m_ActiveScene(activeScene), m_EntityName(name), m_UUID(UUID()), m_CopyEntity(copyEntity), m_Graveyard(m_ActiveScene->m_Graveyard)
		{
			ProcessEntityName();

			m_Entity = m_ActiveScene->CreateEntityWithUUID(m_Entity, m_UUID);
			m_ActiveScene->CopyAllComponents(m_CopyEntity, m_Entity);

			// Dont want to have m_CopyEntity's children.
			if (m_CopyEntity.HasComponent<ChildComponent>())
				m_Entity.RemoveComponent<ChildComponent>();

			// Overriding copied data
			auto& tc = m_Entity.GetComponent<TransformComponent>();
			tc.Self = m_Entity;
			m_Entity.GetComponent<TagComponent>().Tag = m_EntityName;

			m_OldEntity = m_Graveyard->AddEntity(m_Entity);

			CreateChildren(m_CopyEntity, m_Entity);
			m_ActiveScene->DestroyEntity(m_Entity);

		}

		virtual void Execute() override
		{
			
			m_Entity = m_Graveyard->MoveEntityToScene(m_OldEntity, m_ActiveScene);
			auto& tc = m_Entity.GetComponent<TransformComponent>();
			// Add entity to parent's child component
			if (tc.Parent != Entity::Null)
			{
				auto& parentCC = tc.Parent.GetComponent<ChildComponent>();
				parentCC.ChildEntities.push_back(m_Entity);
				parentCC.ChildCount++;
			}

			while (!m_ChildEntities.empty())
			{
				m_Graveyard->MoveEntityToScene(m_ChildEntities.top(), m_ActiveScene);
				m_ChildEntities.pop();
			}

			Application::Get().SetIsSavedStatus(false);
		}

		virtual void Undo() override
		{
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
				for (auto childEntity : from.GetComponent<ChildComponent>().ChildEntities)
				{
					Entity newChildEntity = m_ActiveScene->CreateEntityWithUUID(UUID());
					auto& cc = to.GetComponent<ChildComponent>();
					m_ActiveScene->CopyAllComponents(childEntity, newChildEntity);
					newChildEntity.GetComponent<TransformComponent>().Self = newChildEntity;
					newChildEntity.GetComponent<TransformComponent>().Parent = to;
					cc.ChildEntities.push_back(newChildEntity);
					cc.ChildCount++;
					m_ChildEntities.push(m_Graveyard->AddEntity(newChildEntity));
					CreateChildren(childEntity, newChildEntity);
					m_ActiveScene->DestroyEntity(newChildEntity);
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
		Entity m_CopyEntity;
		entt::entity m_OldEntity;
		UUID m_UUID;
		std::string m_EntityName;
		std::stack<entt::entity> m_ChildEntities;
		Ref<Graveyard> m_Graveyard;
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