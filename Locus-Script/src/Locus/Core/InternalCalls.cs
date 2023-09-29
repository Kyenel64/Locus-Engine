using System;
using System.Runtime.CompilerServices;


namespace Locus
{
    // This is where we link C# declarations with C++ definitions.
    // Eg. Log(float val) is defined in C++. It can be a different name in C++ since we link
    // the two functions together with mono.
    public class InternalCalls
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Log(float val);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Entity_HasComponent(ulong id, Type componentType);

    }
}
