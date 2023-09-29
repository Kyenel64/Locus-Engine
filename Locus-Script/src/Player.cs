using System;

using Locus;

namespace Test
{
    public class TestClass : Entity
    {
        void OnCreate()
        {
            Console.WriteLine("TestClass::OnCreate()");
        }

        void OnUpdate(float deltaTime)
        {

        }
    }
}

public class TestNoNamespace : Entity
{
    void OnCreate()
    {
        Console.WriteLine("TestNoNamespace::OnCreate()");
    }

    void OnUpdate(float deltaTime)
    {

    }
}

namespace Sandbox
{
    // Sample class to demonstrate C# API
    public class Player : Entity
    {
        //private TransformComponent m_Transform;
        //private Rigidbody2DComponent m_Rigidbody;


        void OnCreate()
        {
            Console.WriteLine("Player.OnCreate - {0}", ID);
            //m_Transform = GetComponent<TransformComponent>();
            //m_Rigidbody = GetComponent<Rigidbody2DComponent>();
            if (HasComponent<IDComponent>())
            {
                Console.WriteLine("Player has ID Component!");
            }
        }

        void OnUpdate(float deltaTime)
        {
            Console.WriteLine("Player.OnUpdate - {0}", ID);

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
