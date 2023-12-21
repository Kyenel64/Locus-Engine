using System;
using System.Runtime.InteropServices;
using Locus;

namespace Sandbox
{
	public class RunnerController : Entity
	{
		public float jumpForce = 8.0f;
		Rigidbody2DComponent rb2d;
		private bool canJump = false;
		void OnCreate()
		{
			rb2d = GetComponent<Rigidbody2DComponent>();
		}

		void OnUpdate(float deltaTime)
		{
			if (Input.IsKeyPressed(KeyCode.Space))
			{
				if (canJump)
				{
					rb2d.AddLinearImpulse(new Vec2(0.0f, jumpForce));
					canJump = false;
				}
			}
		}

		public override void OnCollisionBegin(Entity entity)
		{
			if (entity.Group == "Floor")
				canJump = true;

			if (entity.Group == "Obstacle")
				Destroy(this);
		}
	}
}