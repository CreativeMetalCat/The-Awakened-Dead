#pragma once

#include "MovingPawn.h"
#include "State.h"
#include "Player.h"

#include <iostream>

class npc_zombie_base :public MovingPawn
{
protected:
	//index of current animation
	size_t animIndex = 0;

	bool IsDead = false;

public:
	float Health = 100.f;


	virtual void onDamage(float damage, Object*object, Context*&context, std::string stateName)
	{
		if (!IsDead)
		{
			Health -= damage;

			if (Health <= 0)
			{
				IsDead = true;
			}
		}
	}
	virtual void leftCollision(Object*&object, Context*&context, std::string stateName)
	{

	}

	virtual void onCollision(Object*&object, Context*&context, std::string stateName)
	{
		if (dynamic_cast<Player*>(object))
		{
			sf::Vector2f diff;
			diff.x = object->GetObjectPosition().x - this->GetObjectPosition().x;
			diff.y = object->GetObjectPosition().y - this->GetObjectPosition().x;

			this->SetObjectRotation((atan2(diff.y, diff.x)*(180 / M_PI)));
		}
	}

	npc_zombie_base(sf::Vector2f position, float speed, float width, float height) :MovingPawn(speed, position, width, height)
	{
		collision.width = width;
		collision.height = height;
	}

	

	sf::Vector2f GetObjectPosition() override
	{
		return sf::Vector2f(collision.left, collision.top);
	}

	virtual void Draw(sf::RenderWindow*&window)override
	{
		if (IsDead != true)
		{
			
		}


		if (!Children->empty())
		{
			for (size_t i = 0; i < this->Children->size(); i++)
			{
				Children->at(i)->Draw(window);
			}
		}
	}

	void Init()override
	{

	}

	void Update(sf::Time dt)
	{
		this->UpdateMovement(dt);
	}
};