// --- Entity -----------------------------------------------------------------
using System;
using System.Runtime.Versioning;

namespace Locus
{
    /// <summary>
    /// Base class for all entities. 
    /// </summary>
    public class Entity
    {
        // --- Properties ---

        /// <summary>
        /// UUID of the entity.
        /// </summary>
        public readonly ulong ID; // UUID
        /// <summary>
        /// Name of the entity.
        /// </summary>
        public string Tag
        {
            get => InternalCalls.Entity_GetTag(ID);
            set => InternalCalls.Entity_SetTag(ID, value);
        }
        /// <summary>
        /// The enabled status of the entity.
        /// </summary>
        public bool Enabled
        {
            get => InternalCalls.Entity_GetEnabled(ID);
            set => InternalCalls.Entity_SetEnabled(ID, value);
        }
        /// <summary>
        /// The transform component of the entity.
        /// </summary>
        public TransformComponent Transform
        {
            get => GetComponent<TransformComponent>();
        }


        // --- Constructors ---
        public Entity()
        {

        }

        internal Entity(ulong id)
        {
            ID = id;
        }

        // --- Public Methods ---

        /// <summary>
        /// Checks if entity has component of type T. 
        /// </summary>
        public bool HasComponent<T>() where T : Component, new()
        {
            Type componentType = typeof(T);
            return InternalCalls.Entity_HasComponent(ID, componentType);
        }
        /// <summary>
        /// Gets the component of type T attached to the entity. Returns null if not found.
        /// </summary>
        public T GetComponent<T>() where T : Component, new()
        {
            if (!HasComponent<T>())
                return null;

            T component = new T() { Entity = this };
            return component;
        }
        /// <summary>
        /// Adds the component of type T to the entity.
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
        /// Creates an empty entity. Optionally sets the tag of the newly created entity.
        /// </summary>
        public static Entity CreateEntity(string tag = "Empty Entity")
        {
            Entity entity = new Entity(InternalCalls.CreateEntity());
            entity.Tag = tag;
            return entity;
        }
        /// <summary>
        /// Searches for an entity with the given tag.
        /// </summary>
        public static Entity Find(string tag)
        {
            Entity entity = new Entity(InternalCalls.Entity_Find(tag));
            return entity;
        }


        // --- Collision Methods ---
        internal void OnCollisionBeginInternal(ulong id)
        {
            OnCollisionBegin(new Entity(id));
        }
        public virtual void OnCollisionBegin(Entity entity) {}

        internal void OnCollisionEndInternal(ulong id)
        {
            OnCollisionEnd(new Entity(id));
        }
        public virtual void OnCollisionEnd(Entity entity) {}

    }
}
