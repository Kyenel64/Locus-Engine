// --- ContactListener2D ------------------------------------------------------
// The ContactListener2D class listens for any collisions between any two
//	entities within the active scene. Calls the callback functions
//	BeginContact() and EndContact() depending on the state of collision.
#pragma once

#include <stack>

#include <box2d/box2d.h>

#include "Locus/Scene/Entity.h"

namespace Locus
{
	class ContactListener2D : public b2ContactListener
	{
	public:
		ContactListener2D() = default;
		virtual ~ContactListener2D() = default;

		// Add collision pairs to stack for later execution.
		// Functions should not be called within Contact callbacks since
		//	the callbacks are within the world->Step() function.
		virtual void BeginContact(b2Contact* contact) override;
		virtual void EndContact(b2Contact* contact) override;

		// Execute collision functions.
		void Execute();
	private:
		std::stack<std::pair<Entity, Entity>> m_BeginContacts;
		std::stack<std::pair<Entity, Entity>> m_EndContacts;
	};
}
