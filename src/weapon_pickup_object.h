#pragma once


#include "Weapon.h"
#include "SceneActor.h"
#include "Player.h"
#include "Weapon.h"


class weapon_pickup_object :public SceneActor
{
private:

protected:
	//if set to true:
	// will be picked it up even if object that is pick up has weapon with this type
	//used 
	bool m_force_pickup = false;
public:
	//weapon that will be picked up
	Weapon*weapon;

	int ammo_in_the_weapon = 0;

	weapon_pickup_object(Weapon*w, sf::Vector2f position, sf::Sprite sprite, float width, float height, bool force_pickup = true, int area_id = 0) :SceneActor(position, sprite, width, height, area_id), ammo_in_the_weapon(ammo_in_the_weapon), weapon(w), m_force_pickup(force_pickup)
	{

	}

	weapon_pickup_object(Weapon*w, sf::Sprite sprite, int ammo_in_the_weapon = 0, bool force_pickup =true,int area_id = 0) :SceneActor(sprite, area_id), ammo_in_the_weapon(ammo_in_the_weapon), m_force_pickup(force_pickup), weapon(w)
	{

	}

	void onCollision(Object*object, b2Fixture *fixtureA, b2Fixture *fixtureB, Context*&context, std::string stateName)
	{
		if (weapon != NULL)
		{
			weapon->ammoInTheClip = ammo_in_the_weapon;
			weapon->clips = 0;
		}
		if (Player*p = dynamic_cast<Player*>(object))
		{
			
			if (!m_force_pickup)
			{
				if (!p->weapons->empty())
				{
					//check if player already has weapon of this type
					for (size_t i = 0; i < p->weapons->size(); i++)
					{
						if (p->weapons->at(i)->weaponType == this->weapon->weaponType)
						{
							//if does - return;
							return;
						}
					}
					//if doesn't add 
					p->weapons->push_back(this->weapon);
				}
			}
			else 
			{
				//add weapon 
				p->weapons->push_back(this->weapon);
			}
			
		}

	}

	void leftCollision(Object*object, b2Fixture *fixtureA, b2Fixture *fixtureB, Context*&context, std::string stateName)
	{
		if (Player*p = dynamic_cast<Player*>(object))
		{
			context->game->GetStateByName(stateName)->objectsToDestroy->push_back(this);
		}
	}
	void Draw(sf::RenderWindow*&window)
	{
		window->draw(weapon->sprite);
	}
	void Draw(sf::RenderWindow&window)
	{
		window.draw(weapon->sprite);
	}
	~weapon_pickup_object()
	{
		weapon->release();
	}
};