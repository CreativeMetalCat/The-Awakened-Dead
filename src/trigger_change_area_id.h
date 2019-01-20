#pragma once

#include "Object.h"
#include "SceneTile.h"

//used to change object's current area_id value
class trigger_change_area_id :public Object
{
protected:

public:
	//set in loading
	int area_to_change_to = 0;

	//set when object changes id 
	int previous_area = 0;


	trigger_change_area_id(sf::Vector2f pos, float width, float height, int area_to_change_to = 0,int area_id=0) :Object(pos, width, height, area_id), area_to_change_to(area_to_change_to)
	{

	}

	void onCollision(Object*object, b2Fixture* fixtureA, b2Fixture* fixtureB)
	{
		if (!dynamic_cast<SceneTile*>(object))
		{
			object->area_id = area_to_change_to;
			if (this->bodyIsSensor)
			{
				CollidingObjects->push_back(object);
			}
		}
	}

	void leftCollision(Object*object, b2Fixture* fixtureA, b2Fixture *fixtureB)
	{
		if (!dynamic_cast<SceneTile*>(object))
		{
			if (!CollidingObjects->empty())
			{
				CollidingObjects->erase(std::find(CollidingObjects->begin(), CollidingObjects->end(), object));
			}
		}
	}
};