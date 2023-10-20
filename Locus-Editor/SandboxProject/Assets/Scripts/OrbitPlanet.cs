// Example script to simulate orbital motion in 2D.
using Locus;
using System;
using System.Diagnostics;

namespace Sandbox
{
    public class OrbitPlanet : Entity
    {
        public float thisMass = 1.0f;
        public float centerMass = 1000000.0f;

        public float initialForce = 5.0f;

        private Vec3 originalPosition;
        private Vec3 newPosition;

        private Vec3 velocity;
        private Vec3 acceleration;
        private Vec3 position;
        void OnCreate()
        {
            position = GetComponent<TransformComponent>().Position;

            Vec3 dir = new Vec3(0, 0, 0) - GetComponent<TransformComponent>().Position;
            Vec3 intertialForce = Vec3.Cross(new Vec3(0, 0, 1), (dir / Vec3.Length(dir))) * initialForce;

            velocity = intertialForce;
        }

        void OnUpdate(float deltaTime)
        {
            float time = 0.001f;
            originalPosition = position;
            acceleration = (CalculateForceDirection() / thisMass);

            position += (velocity * time + 0.5f * acceleration * time * time);
            newPosition = position;

            velocity = (newPosition - originalPosition) / time;

            position = newPosition;
            GetComponent<TransformComponent>().Position = newPosition;
        }

        Vec3 CalculateForceDirection()
        {
            float distance = Vec3.Distance(position, new Vec3(0, 0, 0));
            if (distance < 0.3f * 0.5f)
            {
                distance = 1.0f;
            }
            float M = thisMass;
            float m = centerMass;
            float G = 6.67430e-11f * 10000000.0f;

            float F = (G * M * m) / (distance * distance);
            Vec3 dir = new Vec3(0, 0, 0) - position;
            return F * (dir / Vec3.Length(dir));
        }
    }
}
