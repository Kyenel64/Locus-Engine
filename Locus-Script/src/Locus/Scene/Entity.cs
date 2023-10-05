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
            get => InternalCalls.TagComponent_GetTag(ID);
            set => InternalCalls.TagComponent_SetTag(ID, value);
        }
        /// <summary>
        /// The enabled status of the entity.
        /// </summary>
        public bool Enabled
        {
            get => InternalCalls.TagComponent_GetEnabled(ID);
            set => InternalCalls.TagComponent_SetEnabled(ID, value);
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
    }
}
