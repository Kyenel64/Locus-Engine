using System;
using System.Runtime;
using Locus;

namespace Sandbox
{
	public class ChangingColor : Entity
	{
		public float MoveSpeed = 5.0f;

		void OnCreate()
		{
			GetComponent<SpriteRendererComponent>().Color = new Color(1, 1, 0);
		}

		void OnUpdate(float deltaTime)
		{
			GetComponent<CircleRendererComponent>().Color = new Color(1, 1, 0);
			//GetComponent<SpriteRendererComponent>().Color = new Color(1, 1, 0);
		}
	}
}
