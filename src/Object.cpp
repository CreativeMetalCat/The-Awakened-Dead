#include "Object.h"

void Object::applyImpulse(b2Vec2 impulse)
{
	_impulse = impulse;
	_impulseApplied = false;
}