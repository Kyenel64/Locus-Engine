using System;

namespace Locus
{
    public class CSharpTest
    {
        public float val = 5.0f;

        public CSharpTest()
        {
            Console.WriteLine("Created CSharpTest class");
        }

        public void PrintVal()
        {
            Console.WriteLine("Value: {0:F}", val);
        }

        public void PrintValWithParam(float val)
        {
            Console.WriteLine("Value: {0:F}", val);
        }

        public void PrintString(string str)
        {
            Console.WriteLine(str);
        }
    }
}
