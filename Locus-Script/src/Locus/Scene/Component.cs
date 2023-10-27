// --- Components -------------------------------------------------------------
using System;
using System.Runtime.Remoting.Messaging;

namespace Locus
{
    // --- Component ----------------------------------------------------------
    /// <summary>
    /// Base class for all components.
    /// </summary>
    public abstract class Component
    {
        /// <summary>
        /// The entity this component is attached to.
        /// </summary>
        public Entity Entity { get; internal set; }
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
        public Mat4 LocalTransform
        {
            get
            {
                InternalCalls.TransformComponent_GetLocalTransform(Entity.ID, out Mat4 result);
                return result;
            }
        }
        /// <summary>
        /// World transform matrix of the entity relative to its parent. Can be used to convert from local space to world space.
        /// </summary>
        public Mat4 WorldTransform
        {
            get
            {
                InternalCalls.TransformComponent_GetWorldTransform(Entity.ID, out Mat4 result);
                return result;
            }
        }
        /// <summary>
        /// Position of the entity local to its parent.
        /// </summary>
        public Vec3 Position
        {
            get
            {
                InternalCalls.TransformComponent_GetLocalPosition(Entity.ID, out Vec3 result);
                return result;
            }
            set => InternalCalls.TransformComponent_SetLocalPosition(Entity.ID, ref value);
        }
        /// <summary>
        /// Rotation of the entity local to its parent. Represented in euler angles.
        /// </summary>
        public Vec3 EulerRotation
        {
            get
            {
                InternalCalls.TransformComponent_GetLocalRotationEuler(Entity.ID, out Vec3 result);
                return result;
            }
            set => InternalCalls.TransformComponent_SetLocalRotationEuler(Entity.ID, ref value);
        }
        /// <summary>
        /// Scale of the entity local to its parent.
        /// </summary>
        public Vec3 Scale
        {
            get
            {
                InternalCalls.TransformComponent_GetLocalScale(Entity.ID, out Vec3 result);
                return result;
            }
            set => InternalCalls.TransformComponent_SetLocalScale(Entity.ID, ref value);
        }
        /// <summary>
        /// Matrix to convert world space coordinates to local space.
        /// </summary>
        public Mat4 WorldToLocal
        {
            get
            {
                InternalCalls.TransformComponent_GetWorldToLocal(Entity.ID, out Mat4 result);
                return result;
            }
        }
    }



    // --- Sprite Renderer Component ------------------------------------------
    /// <summary>
    /// Sprite renderer component.
    /// </summary>
    public class SpriteRendererComponent : Component
    {

    }
}
