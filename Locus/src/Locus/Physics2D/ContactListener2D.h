// --- ContactListener2D ------------------------------------------------------
// The ContactListener2D class listens for any collisions between any two
//	entities within the active scene. Calls the callback functions
//	BeginContact() and EndContact() depending on the state of collision.
#pragma once

#include <box2d/box2d.h>

namespace Locus
{
	class ContactListener2D : public b2ContactListener
	{
	public:
		ContactListener2D() = default;
		virtual ~ContactListener2D() = default;

		virtual void BeginContact(b2Contact* contact) override;
		virtual void EndContact(b2Contact* contact) override;
	private:

	};
}
