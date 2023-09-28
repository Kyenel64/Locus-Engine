using System;
using System.Runtime.CompilerServices;
using System.Runtime.Remoting.Messaging;

namespace Locus
{

    public static class InternalCalls
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Log(float val);
    }
    public class CSharpTest
    {
        public void OnUpdate()
        {
            Console.WriteLine("CSharpTest::OnUpdate()");
        }
    }

    public class Entity
    {
        UInt32 m_EntityID = 1929234923;
        public void PrintID()
        {
            Console.WriteLine(m_EntityID);
            InternalCalls.Log(120);
        }

        public void PrintAString(string str)
        {
            Console.WriteLine(str);
        }
    }
}
