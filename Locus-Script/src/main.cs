using System;

namespace Locus
{
    public class CSharpTest
    {
        public float val = 5.0f;

        public void PrintVal()
        {
            Console.WriteLine("Value: {0:F}", val);
        }

        private void incrementVal(float value)
        {
            val += value;
        }
    }
}
