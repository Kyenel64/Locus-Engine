// --- Entity -----------------------------------------------------------------
using System;

namespace Locus
{
	/// <summary>
	/// Base class for all entities. 
	/// </summary>
	public class Entity
	{
		// --- Properties ---
		/// <summary> The unique identifier for the entity. </summary>
		public readonly ulong ID;
		/// <summary> Name of the entity. </summary>
		public string Tag
		{
			get => InternalCalls.Entity_GetTag(ID);
			set => InternalCalls.Entity_SetTag(ID, value);
		}
		/// <summary> The group/layer of the entity. </summary>
		public string Group
		{
			get => InternalCalls.Entity_GetGroup(ID);
			set => InternalCalls.Entity_SetGroup(ID, value);
		}
		/// <summary> The enabled state of the entity. </summary>
		public bool Enabled
		{
			get => InternalCalls.Entity_GetEnabled(ID);
			set => InternalCalls.Entity_SetEnabled(ID, value);
		}
		/// <summary> The transform component of the entity. </summary>
		public TransformComponent Transform
		{
			get => GetComponent<TransformComponent>();
		}

		// --- Static Properties ---
		/// <summary> Returns a null entity with ID = 0.</summary>
		public static Entity Null = new Entity(0);

		// --- Constructors ---
		/// <summary> Creates an entity with ID = 0. </summary>
		public Entity()
		{
			ID = 0;
		}

		internal Entity(ulong id)
		{
			ID = id;
		}

		// --- Public Methods ---
		/// <summary>
		/// Returns true if entity has component T. 
		/// </summary>
		public bool HasComponent<T>() where T : Component
		{
			Type componentType = typeof(T);
			return InternalCalls.Entity_HasComponent(ID, componentType);
		}
		/// <summary>
		/// Gets the component T attached to the entity. Returns null if entity does not contain the component.
		/// </summary>
		public T GetComponent<T>() where T : Component, new()
		{
			if (!HasComponent<T>())
				return null;

			T component = new T() { Entity = this };
			return component;
		}
		/// <summary>
		/// Adds component T to the entity.
		/// </summary>
		public T AddComponent<T>() where T : Component, new()
		{
			T component = new T() { Entity = this };
			Type componentType = typeof(T);
			InternalCalls.Entity_AddComponent(ID, componentType);
			return component;
		}

		// --- Static Methods ---
		/// <summary>
		/// Creates a new entity. Optionally defines the tag of the entity.
		/// </summary>
		public static Entity CreateEntity(string tag = "Empty Entity")
		{
			Entity entity = new Entity(InternalCalls.Entity_CreateEntity());
			entity.Tag = tag;
			return entity;
		}
		/// <summary>
		/// Destroys an entity.
		/// </summary>
		public static void Destroy(Entity entity)
		{
			InternalCalls.Entity_Destroy(entity.ID);
		}
		/// <summary>
		/// Searches and returns an entity by tag. Returns null if entity is not found.
		/// </summary>
		public static Entity Find(string tag)
		{
			Entity entity = new Entity(InternalCalls.Entity_Find(tag));
			if (entity.ID == 0)
				return null;
			return entity;
		}

		// --- Collision Callbacks ---
		internal void OnCollisionBeginInternal(ulong id)
		{
			OnCollisionBegin(new Entity(id));
		}
		/// <summary>
		/// Called whenever the entity begins a collision with another entity.
		/// </summary>
		public virtual void OnCollisionBegin(Entity entity) {}

		internal void OnCollisionEndInternal(ulong id)
		{
			OnCollisionEnd(new Entity(id));
		}
		/// <summary>
		/// Called whenever the entity ends a collision with another entity.
		/// </summary>
		public virtual void OnCollisionEnd(Entity entity) {}
	}
}
