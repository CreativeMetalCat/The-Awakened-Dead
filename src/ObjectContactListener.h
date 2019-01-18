#pragma once

#include <Box2D.h>

class ObjectContactListener :public b2ContactListener
{
private:

protected:

public:
	void BeginContact(b2Contact*contact)
	{
		void* bodyUserDataA = contact->GetFixtureA()->GetBody()->GetUserData();
		void* bodyUserDataB = contact->GetFixtureB()->GetBody()->GetUserData();
		if (bodyUserDataA&&bodyUserDataB)
		{
			static_cast<Object*>(bodyUserDataA)->OnCollision(static_cast<Object*>(bodyUserDataB), contact->GetFixtureA(), contact->GetFixtureB());
			static_cast<Object*>(bodyUserDataB)->OnCollision(static_cast<Object*>(bodyUserDataA), contact->GetFixtureA(), contact->GetFixtureB());
		}
	}
	void EndContact(b2Contact*contact)
	{
		void* bodyUserDataA = contact->GetFixtureA()->GetBody()->GetUserData();
		void* bodyUserDataB = contact->GetFixtureB()->GetBody()->GetUserData();
		if (bodyUserDataA&&bodyUserDataB)
		{
			static_cast<Object*>(bodyUserDataA)->LeftCollision(static_cast<Object*>(bodyUserDataB), contact->GetFixtureA(), contact->GetFixtureB());
			static_cast<Object*>(bodyUserDataB)->LeftCollision(static_cast<Object*>(bodyUserDataA), contact->GetFixtureA(), contact->GetFixtureB());
		}
	}
};