#pragma once

#include "Projectile.h"

class knife_attack_projectile :public projectile
{
public:
	knife_attack_projectile(sf::Vector2f position, float Height, float Width, float maxDistance, float Speed):projectile(position,Height,Width,maxDistance,Speed,sf::Sprite())
	{

	}

	void projectileOnCollision(Object*&object, Context*&context, std::string stateName) override
	{
		if (object == owner)
		{
			return;
		}
		object->onDamage(20.f, this, context, stateName);

		if (PropPhysics*pp = dynamic_cast<PropPhysics*>(object))
		{
			int channel_id = -1;

			if (pp->getMaterialTypeImpactSoundName() != "")
			{
				context->game->PlaySound(pp->getMaterialTypeImpactSoundName(), channel_id);
				if (channel_id != -1)
				{
					bool isPlaying = false;
					context->game->Channels->at(channel_id)->isPlaying(&isPlaying);
					if (isPlaying)
					{
						FMOD_VECTOR pos;
						pos.z = 0;
						pos.x = pp->GetObjectPosition().x;
						pos.y = pp->GetObjectPosition().y;

						context->game->Channels->at(channel_id)->set3DAttributes(&pos, 0);
					}
				}
			}

			//mass1 - mass 2(m=minus)
			//big name. but that's physics what can i do
			b2Vec2 m1mm2bythisVelandDividedBym1plusm2 = b2Vec2
			(
				((object->body->GetMass() - this->body->GetMass())*object->body->GetLinearVelocity()).x* (object->body->GetMass() + this->body->GetMass()),
				((object->body->GetMass() - this->body->GetMass())*object->body->GetLinearVelocity()).y* (object->body->GetMass() + this->body->GetMass())
			);
			b2Vec2 impulse = object->body->GetMass()*m1mm2bythisVelandDividedBym1plusm2;

			object->applyImpulse(impulse);

			this->IsDone = true;
		}
		if (npc_test_turret* ntt = dynamic_cast<npc_test_turret*>(object))
		{
			ntt->apllyDamage(1.f, this, context, stateName);
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

		if (dynamic_cast<SceneTile*>(object))
		{
			return;
		}

		if (dynamic_cast<projectile*>(object)) { return; }
		/*if (dynamic_cast<Player*>(object)) { return; }*/
		this->CollidingObjects->push_back(object);

		if (dynamic_cast<SolidObject*>(object))
		{
			IsDone = true;
		}

		sf::Vector2f diff;

		//Use location where bullet hit rather then hitted object's location(looks more realistic)
		diff.x = this->body->GetPosition().x - this->Origin.x;
		diff.y = this->body->GetPosition().y - this->Origin.y;


		float a = static_cast<float>((atan2(diff.x, diff.y)*(180 / M_PI)));
		Decal*blood;
	}
};