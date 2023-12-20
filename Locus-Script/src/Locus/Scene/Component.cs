// --- Component -------------------------------------------------------------
// Contains all the component classes. 

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
	/// Position, rotation, and scale of the entity.
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
		/// World transform matrix of the entity relative to the scene.
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
		/// Local position of the entity.
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
		/// Local euler rotation of the entity in degrees.
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
		/// Local scale of the entity.
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
	/// Renders a 2D quad or sprite if a texture is attached.
	/// </summary>
	public class SpriteRendererComponent : Component
	{
		/// <summary>
		/// Color of the sprite. Becomes a tint if a texture is attached.
		/// </summary>
		public Color Color
		{
			get
			{
				InternalCalls.SpriteRendererComponent_GetColor(Entity.ID, out Color result);
				return result;
			}
			set => InternalCalls.SpriteRendererComponent_SetColor(Entity.ID, ref value);
		}
		/// <summary>
		/// Tiling factor of the attached texture.
		/// </summary>
		public float TilingFactor
		{
			get => InternalCalls.SpriteRendererComponent_GetTilingFactor(Entity.ID);
			set => InternalCalls.SpriteRendererComponent_SetTilingFactor(Entity.ID, value);
		}
	}



	// --- Circle Renderer Component ------------------------------------------
	/// <summary>
	/// Renders a 2D circle.
	/// </summary>
	public class CircleRendererComponent : Component
	{
		/// <summary>
		/// Color of the circle.
		/// </summary>
		public Color Color
		{
			get
			{
				InternalCalls.CircleRendererComponent_GetColor(Entity.ID, out Color result);
				return result;
			}
			set => InternalCalls.CircleRendererComponent_SetColor(Entity.ID, ref value);
		}
		/// <summary>
		/// Inner thickness of the circle. 0 displays nothing and 1 displays a full circle.
		/// </summary>
		public float Thickness
		{
			get => InternalCalls.CircleRendererComponent_GetThickness(Entity.ID);
			set => InternalCalls.CircleRendererComponent_SetThickness(Entity.ID, value);
		}
		/// <summary>
		/// The blur of the circle. 
		/// </summary>
		public float Fade
		{
			get => InternalCalls.CircleRendererComponent_GetFade(Entity.ID);
			set => InternalCalls.CircleRendererComponent_SetFade(Entity.ID, value);
		}
	}



	// --- Rigidbody2D Component ----------------------------------------------
	/// <summary>
	/// The 2D physics body. 
	/// </summary>
	public class Rigidbody2DComponent : Component
	{
		/// <summary>
		/// The physical behaviour of the rigidbody. Static, Dynamic, or Kinematic.
		/// </summary>
		public RigidbodyType BodyType
		{
			get
			{
				int internalType = InternalCalls.Rigidbody2DComponent_GetBodyType(Entity.ID);
				return (RigidbodyType)internalType;
			}
			set
			{
				int bodyType = (int)value;
				InternalCalls.Rigidbody2DComponent_SetBodyType(Entity.ID, bodyType);
			}
		}
		/// <summary>
		/// Mass of the body.
		/// </summary>
		public float Mass
		{
			get => InternalCalls.Rigidbody2DComponent_GetMass(Entity.ID);
			set => InternalCalls.Rigidbody2DComponent_SetMass(Entity.ID, value);
		}
		/// <summary>
		/// Affects the speed of the entity's movement affected by gravity. 
		/// </summary>
		public float GravityScale
		{
			get => InternalCalls.Rigidbody2DComponent_GetGravityScale(Entity.ID);
			set => InternalCalls.Rigidbody2DComponent_SetGravityScale(Entity.ID, value);
		}
		/// <summary>
		/// The rate at which the world velocity is reduced on the entity.
		/// </summary>
		public float LinearDamping
		{
			get => InternalCalls.Rigidbody2DComponent_GetLinearDamping(Entity.ID);
			set => InternalCalls.Rigidbody2DComponent_SetLinearDamping(Entity.ID, value);
		}
		/// <summary>
		/// The rate at which the angular velocity is reduced on the entity.
		/// </summary>
		public float AngularDamping
		{
			get => InternalCalls.Rigidbody2DComponent_GetAngularDamping(Entity.ID);
			set => InternalCalls.Rigidbody2DComponent_SetAngularDamping(Entity.ID, value);
		}
		/// <summary>
		/// Whether the entity can rotate. Constraint along the z-axis.
		/// </summary>
		public bool FixedRotation
		{
			get => InternalCalls.Rigidbody2DComponent_GetFixedRotation(Entity.ID);
			set => InternalCalls.Rigidbody2DComponent_SetFixedRotation(Entity.ID, value);
		}
		/// <summary>
		/// Whether to enable continuous collision detection. 
		/// This prevents fast moving dynamic bodies to clip through other dynamic bodies.
		/// </summary>
		public bool IsBullet
		{
			get => InternalCalls.Rigidbody2DComponent_GetIsBullet(Entity.ID);
			set => InternalCalls.Rigidbody2DComponent_SetIsBullet(Entity.ID, value);
		}
		/// <summary>
		/// Position of the rigidbody. 
		/// Note: Position cannot be set through the transform component if a rigidbody is attached.
		/// </summary>
		public Vec2 Position
		{
			get
			{
				InternalCalls.Rigidbody2DComponent_GetPosition(Entity.ID, out Vec2 result);
				return result;
			}
			set => InternalCalls.Rigidbody2DComponent_SetPosition(Entity.ID, ref value);
		}
		/// <summary>
		/// Velocity of the rigidbody.
		/// </summary>
		public Vec2 Velocity
		{
			get
			{
				InternalCalls.Rigidbody2DComponent_GetVelocity(Entity.ID, out Vec2 result);
				return result;
			}
			set => InternalCalls.Rigidbody2DComponent_SetVelocity(Entity.ID, ref value);
		}

		/// <summary>
		/// Adds a force to the rigidbody. 
		/// </summary>
		public void AddForce(Vec2 force)
		{
			InternalCalls.Rigidbody2DComponent_AddForce(Entity.ID, ref force);
		}
		/// <summary>
		/// Adds an impluse to the rigidbody. Equal to the change in momentum.
		/// </summary>
		public void AddLinearImpulse(Vec2 impulse)
		{
			InternalCalls.Rigidbody2DComponent_AddLinearImpulse(Entity.ID, ref impulse);
		}
	}



	// --- CircleCollider -----------------------------------------------------
	/// <summary>
	/// 2D circular collision area. 
	/// </summary>
	public class CircleCollider2DComponent : Component
	{

	}

}
