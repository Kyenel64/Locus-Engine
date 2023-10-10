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

        void Test1()
        {
            Tag = "Player Entity";
            m_Transform = GetComponent<TransformComponent>();
            Vec3 position = new Vec3(2, 2, 0);
            m_Transform.Position = position;

            Vec3 cross = Vec3.Cross(new Vec3(0, 1, 0), new Vec3(1, 0, 0));
            Console.WriteLine("Cross of (0, 1, 0) and (1, 0, 0) = {0}, {1}, {2}", cross.x, cross.y, cross.z);
            float distance = Vec3.Distance(new Vec3(10, 5, 0), new Vec3(20, 5, 20));
            Console.WriteLine("Distance between (10, 5, 0) and (20, 5, 20) = {0}", distance);
            Entity.CreateEntity("New Entity");
            Debug.Log();

            Transform.EulerRotation = new Vec3(0, 0, 45);
            Console.WriteLine(Transform.EulerRotation.z);

            Transform.Scale = new Vec3(10, 1, 1);
            Console.WriteLine(Transform.Scale.x);
        }
        void Test2()
        {
            // Setting the world transform from local coordinates.
            Transform.Position = new Vec3(2, 2, 0) * Transform.WorldToLocal;
        }

        void OnCreate()
        {
            //Test1();
            Test2();

        }

        void OnUpdate(float deltaTime)
        {

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

            velocity *= speed;
            Transform.Position += velocity * deltaTime;
        }
    }
}
