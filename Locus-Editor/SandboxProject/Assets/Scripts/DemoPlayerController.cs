using System;
using System.Runtime;
using System.Runtime.CompilerServices;
using Locus;

namespace Sandbox
{
    // Sample class to demonstrate C# API
    public class TestClass : Entity
    {
        public float testFloat = 10.0f;
        public float testFloatNoDefault;

        //public double testFloat = 200.0f;
        //public double testFloatNoDefault;
        
        public int testInt = 10;
        public int testIntNoDefault;

        public short testShort;
        public long testLong;

        public bool testBool = true;
        public bool testBoolNoDefault;
        void OnCreate()
        {
            Console.WriteLine("\rtestFloat:{0} ", testFloat);
            Console.WriteLine("\rtestFloatNoDefault:{0} ", testFloatNoDefault);
            Console.WriteLine("\rtestInt:{0} ", testInt);
            Console.WriteLine("\rtestIntNoDefault:{0} ", testIntNoDefault);
        }

        void OnUpdate(float deltaTime)
        {
            
        }
    }
}
