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

        // --- Vec3 -----------------------------------------------------------
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Vec3_Cross(Vec3 v1, Vec3 v2, out Vec3 output);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float Vec3_Distance(Vec3 v1, Vec3 v2);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float Vec3_Length(Vec3 vec);

        // --- Tag Component --------------------------------------------------
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static string TagComponent_GetTag(ulong id);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TagComponent_SetTag(ulong id, string newTag);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool TagComponent_GetEnabled(ulong id);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TagComponent_SetEnabled(ulong id, bool newEnabled);

        // --- Transform Component --------------------------------------------
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_GetLocalPosition(ulong id, out Vec3 output);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_SetLocalPosition(ulong id, ref Vec3 newVec3);

    }
}
