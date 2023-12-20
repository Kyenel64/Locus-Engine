// --- InternalCalls ----------------------------------------------------------
// This is where internal C# methods are linked to the corresponding C++
//  definitions.
// To return a class, add an out parameter.
using System;
using System.Runtime.CompilerServices;

namespace Locus
{
	internal class InternalCalls
	{
		// --- Debug ---
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void DebugLog(float val);

		// --- Entity ---
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static ulong Entity_CreateEntity();

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static bool Entity_HasComponent(ulong id, Type componentType);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void Entity_AddComponent(ulong id, Type componentType);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static ulong Entity_Find(string tag);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void Entity_Destroy(ulong id);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static string Entity_GetTag(ulong id);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void Entity_SetTag(ulong id, string newTag);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static string Entity_GetGroup(ulong id);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void Entity_SetGroup(ulong id, string newTag);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static bool Entity_GetEnabled(ulong id);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void Entity_SetEnabled(ulong id, bool newEnabled);

		// --- Vec2 ---
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static float Vec2_Distance(Vec2 v1, Vec2 v2);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static float Vec2_Length(Vec2 vec);

		// --- Vec3 ---
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void Vec3_Cross(Vec3 v1, Vec3 v2, out Vec3 output);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static float Vec3_Distance(Vec3 v1, Vec3 v2);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static float Vec3_Length(Vec3 vec);

		// --- Vec4 ---
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static float Vec4_Distance(Vec4 v1, Vec4 v2);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static float Vec4_Length(Vec4 vec);

		// --- Transform Component ---
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void TransformComponent_GetLocalTransform(ulong id, out Mat4 output);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void TransformComponent_GetWorldTransform(ulong id, out Mat4 output);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void TransformComponent_GetLocalPosition(ulong id, out Vec3 output);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void TransformComponent_SetLocalPosition(ulong id, ref Vec3 newPos);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void TransformComponent_GetLocalRotationEuler(ulong id, out Vec3 output);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void TransformComponent_SetLocalRotationEuler(ulong id, ref Vec3 newRot);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void TransformComponent_GetLocalScale(ulong id, out Vec3 output);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void TransformComponent_SetLocalScale(ulong id, ref Vec3 newScale);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void TransformComponent_GetWorldToLocal(ulong id, out Mat4 output);

		// --- Sprite Renderer Component ---
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void SpriteRendererComponent_GetColor(ulong id, out Color output);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void SpriteRendererComponent_SetColor(ulong id, ref Color newColor);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static float SpriteRendererComponent_GetTilingFactor(ulong id);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void SpriteRendererComponent_SetTilingFactor(ulong id, float newTF);

		// --- Circle Renderer Component ---
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void CircleRendererComponent_GetColor(ulong id, out Color output);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void CircleRendererComponent_SetColor(ulong id, ref Color newColor);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static float CircleRendererComponent_GetThickness(ulong id);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void CircleRendererComponent_SetThickness(ulong id, float newThickness);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static float CircleRendererComponent_GetFade(ulong id);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void CircleRendererComponent_SetFade(ulong id, float newFade);

		// --- Rigidbody2D Component ---
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static int Rigidbody2DComponent_GetBodyType(ulong id);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void Rigidbody2DComponent_SetBodyType(ulong id, int newBodyType);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static float Rigidbody2DComponent_GetMass(ulong id);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void Rigidbody2DComponent_SetMass(ulong id, float newMass);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static float Rigidbody2DComponent_GetGravityScale(ulong id);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void Rigidbody2DComponent_SetGravityScale(ulong id, float newGravityScale);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static float Rigidbody2DComponent_GetLinearDamping(ulong id);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void Rigidbody2DComponent_SetLinearDamping(ulong id, float newLinearDamping);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static float Rigidbody2DComponent_GetAngularDamping(ulong id);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void Rigidbody2DComponent_SetAngularDamping(ulong id, float newAngularDamping);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static bool Rigidbody2DComponent_GetFixedRotation(ulong id);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void Rigidbody2DComponent_SetFixedRotation(ulong id, bool newFixedRotation);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static bool Rigidbody2DComponent_GetIsBullet(ulong id);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void Rigidbody2DComponent_SetIsBullet(ulong id, bool newIsBullet);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void Rigidbody2DComponent_AddForce(ulong id, ref Vec2 force);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void Rigidbody2DComponent_AddLinearImpulse(ulong id, ref Vec2 impulse);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void Rigidbody2DComponent_GetPosition(ulong id, out Vec2 output);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void Rigidbody2DComponent_SetPosition(ulong id, ref Vec2 newPos);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void Rigidbody2DComponent_GetVelocity(ulong id, out Vec2 output);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void Rigidbody2DComponent_SetVelocity(ulong id, ref Vec2 newVelocity);

		// --- Input ---
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static bool Input_IsKeyPressed(KeyCode key);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static bool Input_IsKeyHeld(KeyCode key);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static bool Input_IsKeyReleased(KeyCode key);
	}
}
