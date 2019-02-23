#pragma once

#ifndef AMMO_OBJECT_HEADER
#define AMMO_OBJECT_HEADER
#endif // !AMMO_OBJECT_HEADER

#include "Weapon.h"
#include "SceneActor.h"
#include "Player.h"
#include "ammo_data.h"



class ammo_pickup_object :public SceneActor
{
private:

protected:

public:
	//type of ammo that this object is using
	ammo_data Data;

	ammo_pickup_object(ammo_data Data, sf::Vector2f position, sf::Sprite sprite, int area_id = 0) :SceneActor(position, sprite, area_id), Data(Data)
	{

	}

	ammo_pickup_object(ammo_data Data, sf::Vector2f position, sf::Sprite sprite, float width, float height, int area_id = 0) :SceneActor(position, sprite, width, height, area_id), Data(Data)
	{

	}

	ammo_pickup_object(ammo_data Data, sf::Sprite sprite, int area_id = 0) :SceneActor(sprite, area_id), Data(Data)
	{

	}

	void onCollision(Object*object, b2Fixture *fixtureA, b2Fixture *fixtureB, Context*&context, std::string stateName)
	{

		if (Player*p = dynamic_cast<Player*>(object))
		{

			p->AddAmmo(Data);
			std::string text;
			if (Data.ammo_type == AMMO_TYPE_PISTOL) { text = "+ " + std::to_string(Data.clip_amount) + " Pistol Clip"; }
			if (Data.ammo_type == AMMO_TYPE_RIFLE) { text = "+ " + std::to_string(Data.clip_amount) + " Rifle Clip"; }
			if (Data.ammo_type == AMMO_TYPE_SHOTGUN) { text = "+ " + std::to_string(Data.clip_amount) + " Shotgun Ammo"; }


			context->game->GetStateByName(stateName)->AddMessage(new GUI::Message(sf::Vector2f(150, 300), text, sf::Color::White, context->game->Resources->getFontResourceDataByName("calibri")->font, 40, context->game->Resources->getTextureResourceDataByName("textBoxTexture1")->texture, 5.f), sf::Vector2f( 150, 300));
			context->game->GetStateByName(stateName)->objectsToDestroy->push_back(this);
		}

	}

	void leftCollision(Object*object, b2Fixture *fixtureA, b2Fixture *fixtureB, Context*&context, std::string stateName)
	{
		if (Player*p = dynamic_cast<Player*>(object))
		{
			
		}
	}
};
