using System;
using System.Runtime.InteropServices;
using Locus;

namespace Sandbox
{
	public class GenerateObstacle : Entity
	{
		public float m_Interval = 3.0f;
		private float m_Timer = 0.0f;
		void OnCreate()
		{

		}

		void OnUpdate(float deltaTime)
		{
			m_Timer += deltaTime;

			if (m_Timer >= m_Interval)
			{
				SpawnObstacle();
				m_Timer = 0.0f;
			}
		}

		public void SpawnObstacle()
		{
			Entity obstacle = CreateEntity("Obstacle");
			obstacle.Group = "Obstacle";
			obstacle.Transform.Position = Transform.Position;
			CircleRendererComponent src = obstacle.AddComponent<CircleRendererComponent>();
			Rigidbody2DComponent rb2d = obstacle.AddComponent<Rigidbody2DComponent>();
			obstacle.AddComponent<CircleCollider2DComponent>();

			src.Color = Color.Red;
			rb2d.AddLinearImpulse(new Vec2(-8.0f, 0));
		}
	}
}