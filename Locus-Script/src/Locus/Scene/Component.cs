// --- Components -------------------------------------------------------------
using System;

namespace Locus
{
    // --- Component (Base) ---------------------------------------------------
    /// <summary>
    /// Base class for all components.
    /// </summary>
    public class Component
    {
        /// <summary>
        /// The entity this component is attached to.
        /// </summary>
        public Entity Entity { get; internal set; }
    }



    // --- Tag Component ------------------------------------------------------
    public class TagComponent : Component
    {
        public string Tag 
        { 
            get => InternalCalls.TagComponent_GetTag(Entity.ID); 
            set => InternalCalls.TagComponent_SetTag(Entity.ID, value); 
        }

        public bool Enabled
        {
            get => InternalCalls.TagComponent_GetEnabled(Entity.ID); 
            set => InternalCalls.TagComponent_SetEnabled(Entity.ID, value);
        }
    }



    // --- Transform Component ------------------------------------------------
    /// <summary>
    /// Transform component.
    /// </summary>
    public class TransformComponent : Component
    {
        /// <summary>
        /// Local transform matrix of the entity relative to its parent.
        /// </summary>
        public Mat4 LocalTransform;
        /// <summary>
        /// World transform matrix of the entity relative to its parent.
        /// </summary>
        public Mat4 WorldTransform;
        /// <summary>
        /// Local position of the entity relative to its parent.
        /// </summary>
        public Vec3 LocalPosition
        {
            get
            {
                InternalCalls.TransformComponent_GetLocalPosition(Entity.ID, out Vec3 result);
                return result;
            }
            set => InternalCalls.TransformComponent_SetLocalPosition(Entity.ID, ref value);
        }
        /// <summary>
        /// Local rotation of the entity relative to its parent.
        /// </summary>
        public Vec3 LocalRotation;
        /// <summary>
        /// Local scale of the entity relative to its parent.
        /// </summary>
        public Vec3 LocalScale;
        /// <summary>
        /// World position of the entity relative to its parent.
        /// </summary>
        public Vec3 WorldPosition;
        /// <summary>
        /// World rotation of the entity relative to its parent.
        /// </summary>
        public Vec3 WorldRotation;
        /// <summary>
        /// World scale of the entity relative to its parent.
        /// </summary>
        public Vec3 WorldScale; 

    }



    // --- Sprite Renderer Component ------------------------------------------
    public class SpriteRendererComponent : Component
    {

    }
}
