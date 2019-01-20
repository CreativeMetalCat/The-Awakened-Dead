#pragma once
#include "Object.h"
#include "fmod.hpp"
#include "Player.h"

class SoundSourceObject :public Object
{
protected:

public:

	//id to record know what channel of the state it's using
	int sound_channel_id = -1;
	
	//Used to toggle if sound should be played
	bool sound_is_active = false;

	bool sound_is_looped = false;

	//name of sound
	//used to find name in the Game's resources
	std::string sound_name = "";

	//sound itsefl if needed
	//written only if state does so
	FMOD::Sound*sound = NULL;

	//assigned by state or other loading component if game's resources contain sound file
	//USE ONLY IF THERE IS CHANCE OF MISSNG FILE OR OTHER WAY OF NOT BEENG ABLE TO LOAD
	bool have_sound = false;

	
	int max_distance = 1000;

	int min_distance = 100;

	SoundSourceObject(sf::Vector2f pos, std::string sound_name, bool sound_is_looped = false,int max_distance = 1000,int min_distance = 100) :Object(pos),
		sound_name(sound_name),
		max_distance(max_distance),
		min_distance(min_distance),
		sound_is_looped(sound_is_looped)
	{

	}

	SoundSourceObject(sf::Vector2f pos, FMOD::Sound*sound, bool sound_is_looped = false,int max_distance = 1000, int min_distance = 100) :Object(pos),
		sound(sound),
		max_distance(max_distance),
		min_distance(min_distance),
		sound_is_looped(sound_is_looped)
	{

	}

	//put that in the lamba expression or use as OnCollision of base object
	void onCollision(Object*object, b2Fixture *fixtureA, b2Fixture *fixtureB)
	{
		if (dynamic_cast<Player*>(object)&&!sound_is_active)
		{
			sound_is_active = true;
		}
	}

	//put that in the lamba expression or use as LeftCollision of base object
	void leftCollision(Object*object, b2Fixture *fixtureA, b2Fixture *fixtureB)
	{
		if (dynamic_cast<Player*>(object) && sound_is_active)
		{
			sound_is_active = false;
		}
	}
};