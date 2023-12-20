using Locus;
using System;
using System.Diagnostics.Contracts;

namespace Sandbox
{
	public class PlayerController2D : Entity
	{
		public Rigidbody2DComponent rb2d;
		public float moveSpeed = 5.0f;
		public float jumpSpeed = 4.0f;
		private bool canJump = false;
		public float timer = 0.0f;

		void OnCreate()
		{
			rb2d = GetComponent<Rigidbody2DComponent>();
		}

		void OnUpdate(float deltaTime)
		{
			timer += deltaTime;
			// Horizontal movement
			if (Input.IsKeyHeld(KeyCode.A))
				rb2d.Velocity = new Vec2(-moveSpeed, rb2d.Velocity.y);
			else if (Input.IsKeyHeld(KeyCode.D))
				rb2d.Velocity = new Vec2(moveSpeed, rb2d.Velocity.y);

			//Color col = GetComponent<CircleRendererComponent>().Color;

			// Jump
			if (Input.IsKeyPressed(KeyCode.Space))
			{
				if (canJump)
					rb2d.AddLinearImpulse(new Vec2(rb2d.Velocity.x, jumpSpeed));
			}
			
			// Fire projectile
			if (Input.IsKeyPressed(KeyCode.Left))
				FireProjectile(new Vec2(-2, 0));
			else if (Input.IsKeyPressed(KeyCode.Right))
				FireProjectile(new Vec2(2, 0));
			
			// Teleport back to start
			if (Input.IsKeyPressed(KeyCode.T))
				rb2d.Position = new Vec2(0, 3);
			
			Entity projectile = Find("Projectile");
		}

		public override void OnCollisionBegin(Entity entity)
		{
			canJump = true;

			if (entity.Group == "Enemy")
				Destroy(this);
		}

		public override void OnCollisionEnd(Entity entity)
		{
			canJump = false;
		}

		private void FireProjectile(Vec2 direction)
		{
			Entity projectile = CreateEntity("Projectile");
			projectile.Transform.Position = new Vec3(Transform.Position.x + direction.x, Transform.Position.y + direction.y, 0.0f);
			projectile.AddComponent<CircleRendererComponent>();
			Rigidbody2DComponent projectileRb2D = projectile.AddComponent<Rigidbody2DComponent>();
			projectileRb2D.IsBullet = true;
			projectile.AddComponent<CircleCollider2DComponent>();

			projectile.Transform.Scale = new Vec3(0.2f);
			projectileRb2D.AddLinearImpulse(direction * 200.0f);
		}
	}
}
