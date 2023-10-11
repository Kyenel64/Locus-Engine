using System;
using System.Runtime;
using System.Runtime.CompilerServices;
using Locus;

namespace Sandbox
{
    // Sample class to demonstrate C# API
    public class DemoPlayerController : Entity
    {
        private Vec3 velocity;
        public Vec3 acceleration;
        public Vec3 originalPosition;
        public Vec3 newPosition;
        public Vec3 position;

        private Vec3 downVector = new Vec3(0, -1, 0);
        void OnCreate()
        {
            originalPosition = Transform.Position;
            position = Transform.Position;
        }

        void OnUpdate(float deltaTime)
        {
            acceleration = (downVector / 0.001f);
            position += acceleration;
            Transform.Position = position;
        }
    }
}
