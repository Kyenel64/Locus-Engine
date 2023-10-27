// --- InternalCalls ----------------------------------------------------------
// This is where we link C# declarations with C++ definitions using mono.
// Type will return a MonoType*.
// Can't return classes, instead add an out parameter for the output.
using System;
using System.Runtime.CompilerServices;


namespace Locus
{
    internal class InternalCalls
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void DebugLog(float val);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static ulong CreateEntity();

        // --- Entity ---------------------------------------------------------
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Entity_HasComponent(ulong id, Type componentType);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Entity_AddComponent(ulong id, Type componentType);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static ulong Entity_Find(string tag);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static string Entity_GetTag(ulong id);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Entity_SetTag(ulong id, string newTag);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Entity_GetEnabled(ulong id);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Entity_SetEnabled(ulong id, bool newEnabled);

        // --- Vec3 -----------------------------------------------------------
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Vec3_Cross(Vec3 v1, Vec3 v2, out Vec3 output);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float Vec3_Distance(Vec3 v1, Vec3 v2);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float Vec3_Length(Vec3 vec);

        // --- Transform Component --------------------------------------------
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

        // --- Input ----------------------------------------------------------
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Input_IsKeyPressed(KeyCode key);
    }
}
