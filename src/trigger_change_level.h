#pragma once

#include "Object.h"
#include "Player.h"
#include "State.h"
#include "Game.h"

class TriggerChangeLevel :public Object
{
public:
	std::string level_name = "";
	TriggerChangeLevel(sf::Vector2f position, std::string level_name, float width, float height, int area_id = 0) :Object(position, width, height, area_id),level_name(level_name)
	{
		
	}

	//currently worlks only for player
	void onCollision(Object*object, b2Fixture *fixtureA, b2Fixture *fixtureB, Context*&context, std::string stateName)
	{
		if (dynamic_cast<Player*>(object))
		{
			context->game->GetStateByName(stateName)->current_map = level_name;
			context->game->GetStateByName(stateName)->_map_is_loaded = false;
		}
	}

	void leftCollision(Object*object, b2Fixture *fixtureA, b2Fixture *fixtureB)
	{

	}
};