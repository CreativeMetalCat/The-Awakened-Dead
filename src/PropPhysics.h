#pragma once

#include "SceneActor.h"
#include "MaterialTypes.h"

//@description
//physical object that can be pushed around 
//@e.g.
//chair, table,box
//@note: altrought every object based on Object class can be used in similar way it's better to have class specificly for that
//because it's simplier to update one class and cheking errors there then changing base class and wathcing all project crash
class PropPhysics :public SceneActor
{
protected:
	b2Vec2 _tempVec = b2Vec2(0, 0);
public:

	bool SoundDependsOnMass = true;
	int MaterialType = MAT_TYPE_GENERIC;
	bool isMoving = false;

	//required for stopping and managing sounds of physical objects
	int  StateSoundChannelId = -1;

	float mass = 1.f;
	PropPhysics(sf::Vector2f position, sf::Sprite sprite, float width, float height, int MaterialType = MAT_TYPE_GENERIC, bool SoundDependsOnMass = true, int  area_id = 0) :SceneActor(position, sprite, width, height, area_id), MaterialType(MaterialType), SoundDependsOnMass(SoundDependsOnMass)
	{
		this->sprite = sprite;
		sprite.setPosition(position);

	}
	PropPhysics(sf::Sprite sprite) :SceneActor(sprite)
	{
		this->sprite = sprite;
	}
	PropPhysics() {}

	PropPhysics(sf::Vector2f position, sf::Sprite sprite, float width, float height, float mass = 1.f, int MaterialType = MAT_TYPE_GENERIC, bool SoundDependsOnMass = true, int area_id = 0) :SceneActor(position, sprite, width, height, area_id), mass(mass), MaterialType(MaterialType), SoundDependsOnMass(SoundDependsOnMass)
	{
		this->sprite = sprite;
		sprite.setPosition(position);

	}
	PropPhysics(sf::Sprite sprite, float mass = 1.f, int MaterialType = MAT_TYPE_GENERIC, bool SoundDependsOnMass = true, int area_id = 0) :SceneActor(sprite, area_id), mass(mass), MaterialType(MaterialType), SoundDependsOnMass(SoundDependsOnMass)
	{
		this->sprite = sprite;
	}
	PropPhysics(float mass = 1.f, int MaterialType = MAT_TYPE_GENERIC, bool SoundDependsOnMass = true, int area_id = 0) : mass(mass), MaterialType(MaterialType), SoundDependsOnMass(SoundDependsOnMass) {}

	void Init()override
	{
		sf::Vector2f scale;
		scale.x = collision.width / sprite.getTextureRect().width;
		scale.y = collision.height / sprite.getTextureRect().height;
		sprite.setScale(scale);
		sprite.setPosition(sf::Vector2f(this->collision.left, this->collision.top));
		sprite.setOrigin(sprite.getTextureRect().width / 2, sprite.getTextureRect().height / 2);
	}

	//function is better than lamba expression in this case
	virtual void onCollision(Object*&object, Context*&context, std::string stateName)
	{
		if (bodyIsSensor)
		{
			CollidingObjects->push_back(object);
		}
		//b2Vec2 objectImpulse = b2Vec2(object->body->GetLinearVelocity().x*object->body->GetMass(), object->body->GetLinearVelocity().y*object->body->GetMass());
		//b2Vec2 thisImpulse = b2Vec2(this->body->GetLinearVelocity().x*this->body->GetMass(), this->body->GetLinearVelocity().y*this->body->GetMass());
		////mass1 - mass 2(m=minus)
		////big name. but that's physics what can i do
		//b2Vec2 m1mm2bythisVelandDividedBym1plusm2 = b2Vec2
		//(
		//	((this->body->GetMass() - object->body->GetMass())*this->body->GetLinearVelocity()).x* (this->body->GetMass() + object->body->GetMass()),
		//	((this->body->GetMass() - object->body->GetMass())*this->body->GetLinearVelocity()).y* (this->body->GetMass() + object->body->GetMass())
		//);
		//b2Vec2 impulse = this->body->GetMass()*m1mm2bythisVelandDividedBym1plusm2;

		//this->applyImpulse(impulse);
	}
	void Update(sf::Time dt) override
	{
		if (body != nullptr && physBodyInitialized)
		{
			if (!_impulseApplied)
			{

				b2Vec2 imp = b2Vec2(_impulse.x*dt.asMilliseconds(), _impulse.y*dt.asMilliseconds());
				/*b2Vec2 p = body->GetWorldPoint(body->GetPosition());*/
				body->ApplyLinearImpulse(imp, body->GetPosition(), true);
				_impulseApplied = true;
			}
			if (!_velocitySet)
			{
				body->SetLinearVelocity(b2Vec2(_velocity));
				_tempVec = _velocity;
				_velocitySet = true;
			}

			this->collision.left = body->GetPosition().x;
			this->collision.top = body->GetPosition().y;

			sprite.setPosition(sf::Vector2f(body->GetPosition().x/* + this->sprite.getTextureRect().width / 2*/, body->GetPosition().y /*+ this->sprite.getTextureRect().height / 2*/));
			sprite.setRotation(body->GetAngle());

			float velX = this->body->GetLinearVelocity().x;
			float velY = this->body->GetLinearVelocity().y;
			if (this->body->GetLinearVelocity().x != 0.f)
			{
				if (this->body->GetLinearVelocity().x > 0.f)
				{
					velX = this->body->GetLinearVelocity().x - 0.1f*dt.asSeconds();
					if (velX < 0.f&&velX > -0.03f)
					{
						velX = 0;
					}
				}
				if (this->body->GetLinearVelocity().x < 0.f)
				{
					velX = this->body->GetLinearVelocity().x + 0.1f*dt.asSeconds();

					if (velX > 0.f&&velX < 0.03f)
					{
						velX = 0;
					}
				}
			}
			if (this->body->GetLinearVelocity().y != 0.f)
			{
				if (this->body->GetLinearVelocity().y > 0.f)
				{
					velY = this->body->GetLinearVelocity().y - 0.1f*dt.asSeconds();

					if (velY < 0.f&&velY > -0.03f)
					{
						velY = 0;
					}
				}
				if (this->body->GetLinearVelocity().y < 0.f)
				{
					velY = this->body->GetLinearVelocity().y + 0.1f*dt.asSeconds();
					if (velY > 0.f&&velY < 0.03f)
					{
						velY = 0;
					}
				}
			}
			body->SetLinearVelocity(b2Vec2(velX, velY));




			/*if (this->body->GetLinearVelocity().x != 0.f && this->body->GetLinearVelocity().y != 0.f)
			{
				b2Vec2 vel = b2Vec2(this->body->GetLinearVelocity().x - 9.8f*dt.asSeconds(), this->body->GetLinearVelocity().y - 9.8f*dt.asSeconds());
				_tempVec = vel;
				body->SetLinearVelocity(b2Vec2(vel));
			}*/
			/*b2Vec2 vel =b2Vec2(_tempVec.x - 9.8f*dt.asSeconds(), _tempVec.y - 9.8f*dt.asSeconds());
			_tempVec = vel;
			body->SetLinearVelocity(b2Vec2(vel));*/
		}
	}
	//function is better than lamba expression in this case
	virtual void leftCollision(Object*&object, Context*&context, std::string stateName)
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