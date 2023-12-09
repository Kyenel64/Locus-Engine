using System;
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
