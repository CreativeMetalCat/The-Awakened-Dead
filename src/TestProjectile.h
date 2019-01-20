#pragma once

#include "Projectile.h"
#include "Animation.h"
#include "Player.h"
#include "npc_zombie.h"
#include "State.h"
class proje :public projectile
{
public:
	proje(sf::Vector2f position, float Height, float Width, float maxDistance, float Speed, sf::Sprite sprite) :projectile(position, Height, Width, MaxDistance, Speed, sprite)
	{
		collision.width = Width;
		collision.height = Height;
	}
	virtual void projectileOnCollision(Object*&object, Context*&context, std::string stateName) override
	{

		if (PropPhysics*pp = dynamic_cast<PropPhysics*>(object))
		{
			//mass1 - mass 2(m=minus)
			//big name. but that's physics what can i do
			b2Vec2 m1mm2bythisVelandDividedBym1plusm2 = b2Vec2
			(
				((object->body->GetMass() - this->body->GetMass())*object->body->GetLinearVelocity()).x* (object->body->GetMass() + this->body->GetMass()),
				((object->body->GetMass() - this->body->GetMass())*object->body->GetLinearVelocity()).y* (object->body->GetMass() + this->body->GetMass())
			);
			b2Vec2 impulse = object->body->GetMass()*m1mm2bythisVelandDividedBym1plusm2;

			object->applyImpulse(impulse);
		}

		Animation::Animation blood_a = Animation::Animation("blood_a");
		blood_a.FrameIndexes->push_back(Animation::CellIndex(0, 0));
		blood_a.FrameIndexes->push_back(Animation::CellIndex(0, 1));
		blood_a.FrameIndexes->push_back(Animation::CellIndex(0, 2));
		blood_a.FrameIndexes->push_back(Animation::CellIndex(0, 3));
		blood_a.FrameIndexes->push_back(Animation::CellIndex(0, 4));
		blood_a.FrameIndexes->push_back(Animation::CellIndex(0, 5));
		blood_a.FrameIndexes->push_back(Animation::CellIndex(0, 6));
		blood_a.FrameIndexes->push_back(Animation::CellIndex(0, 7));

		if (dynamic_cast<projectile*>(object)) { return; }
		if (dynamic_cast<Player*>(object))
		{
			return;
		}
		if (dynamic_cast<SceneTile*>(object))
		{
			return;
		}
		this->CollidingObjects->push_back(object);


		sf::Vector2f diff;

		//Use location where bullet hit rather then hitted object's location(looks more realistic)
		diff.x = this->body->GetPosition().x - this->Origin.x;
		diff.y = this->body->GetPosition().y - this->Origin.y;


		float a = static_cast<float>((atan2(diff.x, diff.y)*(180 / M_PI)));
		Decal*blood;
		if (npc_zombie*z = dynamic_cast<npc_zombie*>(object))
		{
			z->onDamage(100.f, this, context, stateName);
		}
		else
		{


			blood = new Decal(sf::Vector2f(this->body->GetPosition().x, this->body->GetPosition().y), 0.05f, true, 0.3f, 512, 512, sf::Sprite(context->game->Resources->getTextureResourceDataByName("blood_a_anim")->texture), 100, 100);
			blood->Anim->sprite.setRotation(-a - 270.f);
			blood->Init();
			blood->animations->push_back(blood_a);
			blood->SetAnimation("blood_a");
			context->game->GetStateByName(stateName)->StateObjects->push_back(blood);
		}
	}

	virtual void projectileOnLeftCollision(Object*&object, Context*&context, std::string stateName)override
	{

		if (!this->CollidingObjects->empty())
		{
			if (std::find(this->CollidingObjects->begin(), this->CollidingObjects->end(), object) != this->CollidingObjects->end())
			{
				this->CollidingObjects->erase(std::find(this->CollidingObjects->begin(), this->CollidingObjects->end(), object));
			}
		}
	}
};
