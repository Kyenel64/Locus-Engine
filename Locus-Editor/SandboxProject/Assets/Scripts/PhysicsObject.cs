using System;
using System.Runtime;
using Locus;

namespace Sandbox
{
    // Sample player controller class demonstrating basic movement and Entity referencing.
    public class PhysicsObject : Entity
    {
        public Rigidbody2DComponent rb2d;
        public float moveSpeed = 50.0f;
        private bool canJump = false;
        void OnCreate()
        {
            rb2d = GetComponent<Rigidbody2DComponent>();
        }

        void OnUpdate(float deltaTime)
        {
            if (Input.IsKeyPressed(KeyCode.A))
                rb2d.AddForce(new Vec2(-moveSpeed, 0));
            else if (Input.IsKeyPressed(KeyCode.D))
                rb2d.AddForce(new Vec2(moveSpeed, 0));

            if (Input.IsKeyPressed(KeyCode.Space))
            {
                if (canJump)
                    rb2d.AddLinearImpulse(new Vec2(0, 3));
            }

            if (Input.IsKeyPressed(KeyCode.T))
                rb2d.SetPosition(new Vec2(0, 1));
        }

        public override void OnCollisionBegin(Entity entity)
        {
            Console.WriteLine("{0} Collided with {1}", Tag, entity.Tag);
            CircleRendererComponent crc = GetComponent<CircleRendererComponent>();
            crc.Color = Color.Blue;
            crc.Thickness = 0.3f;
            crc.Fade = 0.5f;
            canJump = true;
        }

        public override void OnCollisionEnd(Entity entity)
        {
            Console.WriteLine("{0} collision ended with {1}", Tag, entity.Tag);
            CircleRendererComponent crc = GetComponent<CircleRendererComponent>();
            crc.Color = Color.Yellow;
            crc.Thickness = 1.0f;
            crc.Fade = 0.005f;
            canJump = false;
        }
    }
}
