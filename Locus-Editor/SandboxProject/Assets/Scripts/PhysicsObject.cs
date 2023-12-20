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
		void OnCreate()
		{
		}

		void OnUpdate(float deltaTime)
		{
			Console.WriteLine(ID);
			GetComponent<SpriteRendererComponent>().Color = new Color(1, 1, 0);
			GetComponent<CircleRendererComponent>().Color = new Color(1, 1, 0);
		}
	}
}
