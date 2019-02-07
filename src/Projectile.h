#pragma once


#include "PropPhysics.h"
#include "npc_test_turret.h"

#include "State.h"
#include "Animation.h"
#include "Decal.h"
#include "SceneTile.h"
#include "SolidObject.h"


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
	//Object that launched projectile
	//e.g. Player,Solder,Zombie, Wall
	Object*owner = NULL;


	//get if object should be deleted
	bool getIsDone()const { return IsDone; }

	sf::Vector2<float>Origin;
	float MaxDistance = 1000.0f;
	float Speed = 500.0f;
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
		if (object == owner)
		{
			return;
		}
		object->onDamage(20.f, this, context, stateName);

		if (PropPhysics*pp = dynamic_cast<PropPhysics*>(object))
		{
			int channel_id = 0;

			if (pp->getMaterialTypeImpactSoundName() != "")
			{
				context->game->PlaySound(pp->getMaterialTypeImpactSoundName());
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
		difference = destination - origin;
		

		IsDone = false;
		this->SetObjectPosition(origin);

		Origin = origin;
		IsDone = false;
		this->SetObjectPosition(origin);
		this->Init();

		if (IsInitialized) { world.DestroyBody(this->body); }
		b2BodyDef def;
		def.position.Set((origin.x + this->GetObjectRectangle().width / 2), (origin.y + this->GetObjectRectangle().height / 2));
		def.type = b2BodyType::b2_dynamicBody;
		this->body = world.CreateBody(&def);
		def.bullet = false;

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
		//temp action
		RotationAngle = angle;

		Origin = origin;
		IsDone = false;
		this->SetObjectPosition(origin);
		this->Init();

		if (IsInitialized) { world.DestroyBody(this->body); }
		b2BodyDef def;
		def.position.Set((origin.x + this->GetObjectRectangle().width / 2), (origin.y + this->GetObjectRectangle().height / 2));
		def.type = b2BodyType::b2_dynamicBody;
		this->body = world.CreateBody(&def);
		def.bullet = false;

		b2PolygonShape shape;
		shape.SetAsBox(this->GetObjectRectangle().width / 2, this->GetObjectRectangle().height / 2);

		b2FixtureDef TriggerFixture;
		TriggerFixture.filter = filter;
		TriggerFixture.density = 0.f;
		TriggerFixture.shape = &shape;
		TriggerFixture.isSensor = 1;

		body->CreateFixture(&TriggerFixture);
		body->SetUserData(this);

		float vx = Speed * std::cos(angle);
		float vy = Speed * std::sin(angle);
		std::cout << angle << std::endl;
		/*body->ApplyLinearImpulseToCenter(b2Vec2(vx * 100, vy * 100), true);*/
		this->applyImpulse(b2Vec2(vx * 1, vy *1 ));
		physBodyInitialized = true;

		//body->SetLinearVelocity((b2Vec2(vx * 500, vy * 500)));
		Velocity = (b2Vec2(vx*100, vy*100));
		IsInitialized = true;
	}

	void Update(sf::Time dt) override
	{
		if (IsDone) { cleanUpObject(); }
		if (IsInitialized)
		{
			
			if (body != nullptr && physBodyInitialized)
			{
				if (!_impulseApplied)
				{
					int dt_asmill = dt.asMilliseconds();
					b2Vec2 imp = b2Vec2(_impulse.x*dt.asMilliseconds(), _impulse.y*dt.asMilliseconds());
					body->ApplyLinearImpulse(imp, body->GetPosition(), true);
					_impulseApplied = true;
				}

				this->sprite.move(Speed*sf::Vector2f(Velocity.x, Velocity.y)*(20.0f / dt.asMicroseconds()));
				this->body->SetTransform(b2Vec2(sprite.getPosition().x, sprite.getPosition().y), RotationAngle);

				travelledDistance += Speed * (20.0f / dt.asMilliseconds());
				if (travelledDistance >= MaxDistance)
				{
					this->IsDone = true;
				}
			}
		}
	}

	void cleanUpObject()
	{
		if (physBodyInitialized)
		{

			this->body->GetWorld()->Step(0, 0, 0);
			if (!this->body->GetWorld()->IsLocked())
			{
				this->body->GetWorld()->DestroyBody(this->body);
				physBodyInitialized = false;
			}
		}
	}

	void  SetObjectPosition(sf::Vector2f pos) override
	{
		collision.left = pos.x;
		collision.top = pos.y;

		sprite.setPosition(pos);

	}
};