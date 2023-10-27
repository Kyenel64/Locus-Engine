using System;
using Locus;

namespace Sandbox
{
    // Sample class to demonstrate C# API
    public class CameraController2D : Entity
    {
        public float testValue;
        void OnCreate()
        {
            Console.WriteLine(testValue);
        }

        void OnUpdate(float deltaTime)
        {
            //Console.WriteLine(testValue);
            float speed = 5.0f;
            Vec3 velocity = Vec3.Zero;

            if (Input.IsKeyPressed(KeyCode.W))
                velocity.y = 1.0f;
            else if (Input.IsKeyPressed(KeyCode.S))
                velocity.y = -1.0f;

            if (Input.IsKeyPressed(KeyCode.A))
                velocity.x = -1.0f;
            else if (Input.IsKeyPressed(KeyCode.D))
                velocity.x = 1.0f;

            if (Input.IsKeyPressed(KeyCode.Up))
                velocity.z = -1.0f;
            else if (Input.IsKeyPressed(KeyCode.Down))
                velocity.z = 1.0f;

            velocity *= speed;
            Transform.Position += velocity * deltaTime;

        }
    }
}
