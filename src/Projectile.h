#pragma once


#include "PropPhysics.h"
#include "npc_test_turret.h"
#include "State.h"
#include "Animation.h"
#include "Decal.h"
#include "SceneTile.h"
#include <iostream>
class projectile :public SceneActor
{

protected:
	sf::Vector2<float>difference;
	float travelledDistance = 0.f;
	bool IsDone = false;
	//called in Launch(...)
	bool IsInitialized = false;

	b2Vec2 Velocity;

public:
	sf::Vector2<float>Origin;
	float MaxDistance = 1000.0f;
	float Speed = 10.0f;
	sf::Vector2<float>Destination;

	projectile(sf::Vector2f position, float Height, float Width, float maxDistance, float Speed, sf::Sprite sprite) :SceneActor(position, sprite), MaxDistance(maxDistance), Speed(Speed)
	{
		collision.width = Width;
		collision.height = Height;
	}
	void Move(sf::Vector2f vec)override
	{
		collision.left += vec.x;
		collision.top += vec.y;
		sprite.move(vec);
	}

	virtual void projectileOnCollision(Object*&object, Context*&context, std::string stateName)
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


		sf::Vector2f diff;

		//Use location where bullet hit rather then hitted object's location(looks more realistic)
		diff.x = this->body->GetPosition().x - this->Origin.x;
		diff.y = this->body->GetPosition().y - this->Origin.y;


		float a = static_cast<float>((atan2(diff.x, diff.y)*(180 / M_PI)));
		Decal*blood;
		/*if (dynamic_cast<npc_zombie*>(object))
		{


			blood = new Decal(sf::Vector2f(this->body->GetPosition().x, this->body->GetPosition().y), 0.05f, true, 0.3f, 512, 512, sf::Sprite(context->game->Resources->getTextureResourceDataByName("blood_a_anim")->texture), 100, 100);
			blood->Anim->sprite.setRotation(-a - 270.f);
			blood->Init();
			blood->animations->push_back(blood_a);
			blood->SetAnimation("blood_a");
			context->game->GetStateByName(stateName)->StateObjects->push_back(blood);
		}
		else
		{


			blood = new Decal(sf::Vector2f(this->body->GetPosition().x, this->body->GetPosition().y), 0.05f, true, 0.3f, 512, 512, sf::Sprite(context->game->Resources->getTextureResourceDataByName("blood_a_anim")->texture), 100, 100);
			blood->Anim->sprite.setRotation(-a - 270.f);
			blood->Init();
			blood->animations->push_back(blood_a);
			blood->SetAnimation("blood_a");
			context->game->GetStateByName(stateName)->StateObjects->push_back(blood);
		}*/
	}

	virtual void projectileOnLeftCollision(Object*&object, Context*&context, std::string stateName)
	{

		if (!this->CollidingObjects->empty())
		{
			if (std::find(this->CollidingObjects->begin(), this->CollidingObjects->end(), object) != this->CollidingObjects->end())
			{
				this->CollidingObjects->erase(std::find(this->CollidingObjects->begin(), this->CollidingObjects->end(), object));
			}
		}
	}

	sf::Vector2f GetObjectPosition()override
	{
		if (IsInitialized)
		{
			return sf::Vector2f(body->GetPosition().x, body->GetPosition().y);
		}
		else
		{
			return { 0.f,0.f };
		}
	}
	void Launch(sf::Vector2<float> destination, sf::Vector2<float> origin, b2World&world, b2Filter filter)
	{
		IsDone = false;
		this->SetObjectPosition(origin);

		Origin = origin;
		difference = destination - origin;
		if (IsInitialized) { world.DestroyBody(this->body); }

		b2BodyDef def;
		def.position.Set(origin.x + this->GetObjectRectangle().width / 2, origin.y + this->GetObjectRectangle().height / 2);
		def.type = b2BodyType::b2_dynamicBody;
		this->body = world.CreateBody(&def);

		b2PolygonShape shape;
		shape.SetAsBox(this->GetObjectRectangle().width / 2, this->GetObjectRectangle().height / 2);

		b2FixtureDef TriggerFixture;
		TriggerFixture.filter = filter;
		TriggerFixture.density = 0.f;
		TriggerFixture.shape = &shape;
		TriggerFixture.isSensor = 1;

		body->CreateFixture(&TriggerFixture);
		body->SetUserData(this);



		/*sf::Vector2f diff;
		diff.x = event.mouseMove.x - player->GetObjectPosition().x;
		diff.y = event.mouseMove.y - player->GetObjectPosition().y;*/


		//degs
		float angle = (atan2(difference.y, difference.x));
		float rads = (angle*(180 / M_PI));
		float vx = Speed * std::cos(rads);
		float vy = Speed * std::sin(rads);
		std::cout << rads << std::endl;
		body->SetLinearVelocity((b2Vec2(vx * 10, vy * 10)));
		Velocity = (b2Vec2(vx * 100, vy * 100));
		IsInitialized = true;
	}

	//angle is in rads
	void Launch(float angle, sf::Vector2<float>origin, b2World&world, b2Filter filter)
	{
		Origin = origin;
		IsDone = false;
		this->SetObjectPosition(origin);
		this->Init();

		if (IsInitialized) { world.DestroyBody(this->body); }
		b2BodyDef def;
		def.position.Set(origin.x + this->GetObjectRectangle().width / 2, origin.y + this->GetObjectRectangle().height / 2);
		def.type = b2BodyType::b2_dynamicBody;
		this->body = world.CreateBody(&def);
		def.bullet = true;

		b2PolygonShape shape;
		shape.SetAsBox(this->GetObjectRectangle().width / 2, this->GetObjectRectangle().height / 2);

		b2FixtureDef TriggerFixture;
		TriggerFixture.filter = filter;
		TriggerFixture.density = 0.f;
		TriggerFixture.shape = &shape;
		TriggerFixture.isSensor = 1;

		body->CreateFixture(&TriggerFixture);
		body->SetUserData(this);



		/*sf::Vector2f diff;
		diff.x = event.mouseMove.x - player->GetObjectPosition().x;
		diff.y = event.mouseMove.y - player->GetObjectPosition().y;*/





		float vx = Speed * std::cos(angle);
		float vy = Speed * std::sin(angle);
		std::cout << angle << std::endl;
		body->SetLinearVelocity((b2Vec2(vx * 500, vy * 500)));
		Velocity = (b2Vec2(vx * 100, vy * 100));
		IsInitialized = true;
	}

	void Update(sf::Time dt) override
	{
		if (IsInitialized)
		{

			/*		if (IsDone==false)
					{*/
			body->SetLinearVelocity(Velocity);
			/*this->Move(Speed*difference*(20.0f / dt.asMicroseconds()));*/
			this->sprite.setPosition(sf::Vector2f(body->GetPosition().x, body->GetPosition().y));
			travelledDistance += Speed * (20.0f / dt.asMilliseconds());
			/*this->Move(Speed*difference*(1.0f / dt.asMilliseconds()));
			travelledDistance += Speed * (1.0f / dt.asMilliseconds());*/

			/*if (MaxDistance < 0.f) { MaxDistance = 500.f; }
			if (travelledDistance > MaxDistance)
			{
				IsDone = true;
				travelledDistance = 0;
				body->SetLinearVelocity(b2Vec2(0,0));
			}*/
			//}
		}
	}
	void  SetObjectPosition(sf::Vector2f pos) override
	{
		collision.left = pos.x;
		collision.top = pos.y;

		sprite.setPosition(pos);

	}
};