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

        public double testDouble = 200.0f;
        public double testDoubleNoDefault;
        
        public int testInt = 10;
        public int testIntNoDefault;

        public short testShort;
        public long testLong;

        public bool testBool = true;
        public bool testBoolNoDefault;

        public char testChar;

        public Vec3 testVec3;

        public Entity otherEntity;
        void OnCreate()
        {
            Console.WriteLine("\rtestFloat:{0} ", testFloat);
            Console.WriteLine("\rtestFloatNoDefault:{0} ", testFloatNoDefault);
            Console.WriteLine("\rtestInt:{0} ", testInt);
            Console.WriteLine("\rtestIntNoDefault:{0} ", testIntNoDefault);

            otherEntity = Find("Other Entity");
            Console.WriteLine(otherEntity.Tag);
        }

        void OnUpdate(float deltaTime)
        {
            //Console.WriteLine("\rtestChar:{0} ", testChar);
            //Console.WriteLine("testVec3: {0}, {1}, {2}", testVec3.x, testVec3.y, testVec3.z);
        }
    }
}
