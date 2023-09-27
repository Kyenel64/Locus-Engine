using System;
using System.Runtime.CompilerServices;

namespace Locus
{
    public class CSharpTest
    {
        public void OnUpdate()
        {
            Console.WriteLine("CSharpTest::OnUpdate()");
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CSharpLog();
    }

    public class Entity
    {
        UInt32 m_EntityID = 1929234923;
        public void PrintID()
        {
            Console.WriteLine(m_EntityID);
        }

        public void PrintAString(string str)
        {
            Console.WriteLine(str);
        }
    }
}
