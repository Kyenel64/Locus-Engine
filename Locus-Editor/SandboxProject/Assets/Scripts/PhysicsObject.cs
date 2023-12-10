﻿using System;
using System.Runtime;
using Locus;

namespace Sandbox
{
    // Sample player controller class demonstrating basic movement and Entity referencing.
    public class PhysicsObject : Entity
    {
        void OnCreate()
        {

        }

        void OnUpdate(float deltaTime)
        {

        }

        public override void OnCollisionBegin(Entity entity)
        {
            Console.WriteLine("{0} Collided with {1}", Tag, entity.Tag);
            CircleRendererComponent crc = GetComponent<CircleRendererComponent>();
            crc.Color = Color.Blue;
            crc.Thickness = 0.3f;
            crc.Fade = 0.5f;

            Rigidbody2DComponent rb2d = GetComponent<Rigidbody2DComponent>();

            rb2d.Mass = 50.0f;
            Console.WriteLine(rb2d.Mass);

            rb2d.GravityScale = 10.0f;
            Console.WriteLine(rb2d.GravityScale);

            rb2d.LinearDamping = 100.0f;
            Console.WriteLine(rb2d.LinearDamping);

            rb2d.AngularDamping = 200.0f;
            Console.WriteLine(rb2d.AngularDamping);
        }

        public override void OnCollisionEnd(Entity entity)
        {
            Console.WriteLine("{0} collision ended with {1}", Tag, entity.Tag);
            CircleRendererComponent crc = GetComponent<CircleRendererComponent>();
            crc.Color = Color.Yellow;
            crc.Thickness = 1.0f;
            crc.Fade = 0.005f;
        }
    }
}
