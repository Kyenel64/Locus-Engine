using System;
using System.Runtime;
using Locus;

namespace Sandbox
{
    // Sample player controller class demonstrating basic movement and Entity referencing.
    public class Player : Entity
    {
        public float MoveSpeed = 5.0f;

        private Entity Camera;
        void OnCreate()
        {
            Camera = Find("Main Camera");
        }

        void OnUpdate(float deltaTime)
        {
            Vec3 velocity = Vec3.Zero;

            if (Input.IsKeyPressed(KeyCode.W))
                velocity.y = 1.0f;
            else if (Input.IsKeyPressed(KeyCode.S))
                velocity.y = -1.0f;

            if (Input.IsKeyPressed(KeyCode.A))
                velocity.x = -1.0f;
            else if (Input.IsKeyPressed(KeyCode.D))
                velocity.x = 1.0f;

            velocity *= MoveSpeed;
            Transform.Position += velocity * deltaTime;
            Camera.Transform.Position = Transform.Position;
        }
    }
}
