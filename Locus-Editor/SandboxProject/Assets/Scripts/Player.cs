using System;
using System.Runtime;
using Locus;

namespace Sandbox
{
    // Sample class to demonstrate C# API
    public class Player : Entity
    {
        private TransformComponent m_Transform;
        //private Rigidbody2DComponent m_Rigidbody;


        void OnCreate()
        {
            GetComponent<TagComponent>().Tag = "Player Entity";
            m_Transform = GetComponent<TransformComponent>();
            Vec3 transform = new Vec3(2, 2, 0);
            m_Transform.LocalPosition = transform;

            Vec3 cross = Vec3.Cross(new Vec3(0, 1, 0), new Vec3(1, 0, 0));
            Console.WriteLine("Cross of (0, 1, 0) and (1, 0, 0) = {0}, {1}, {2}", cross.x, cross.y, cross.z);
            float distance = Vec3.Distance(new Vec3(10, 5, 0), new Vec3(20, 5, 20));
            Console.WriteLine("Distance between (10, 5, 0) and (20, 5, 20) = {0}", distance);
        }

        void OnUpdate(float deltaTime)
        {
            float speed = 0.5f;
            Vec3 vel = Vec3.Zero;
            vel.x += speed * deltaTime;
            Vec3 newPos = m_Transform.LocalPosition;
            newPos.x += speed * deltaTime;
            m_Transform.LocalPosition = newPos;
            //Console.WriteLine("Player.OnUpdate - {0}", ID);

            /*float speed = 0.01f;
            Vec3 velocity = Vec3.Zero;

            if (Input.IsKeyDown(KeyCode.W))
                velocity.y = 1.0f;
            else if (Input.IsKeyDown(KeyCode.S))
                velocity.y = -1.0f;

            if (Input.IsKeyDown(KeyCode.A))
                velocity.x = -1.0f;
            else if (Input.IsKeyDown(KeyCode.D))
                velocity.x = 1.0f;

            velocity *= speed;

            //m_Rigidbody.ApplyLinearImpulse(velocity.XY, true);

            Vec3 position = m_Transform.Position;
            position += velocity * deltaTime;
            m_Transform.Position = position;*/
        }
    }
}
