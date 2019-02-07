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

	//Gets name of sound for object's movement
	//This function is not obligatory and made for simplicity of code
	std::string GetMaterialTypeMovementSoundName()const
	{
		if (MaterialType == MAT_TYPE_PLASTIC_BARREL)
		{
			if (this->SoundDependsOnMass)
			{
				if (mass >= 100.f)
				{

					return MAT_TYPE_PLASTIC_BARREL_HARD_NAME;
				}
				else
				{
					return MAT_TYPE_PLASTIC_BARREL_LIGHT_NAME;
				}
			}
		}

		else if (MaterialType == MAT_TYPE_PLASTIC_BOX)
		{
			if (this->SoundDependsOnMass)
			{
				if (mass >= 100.f)
				{

					return MAT_TYPE_PLASTIC_BOX_HARD_NAME;
				}
				else
				{
					return MAT_TYPE_PLASTIC_BOX_LIGHT_NAME;
				}
			}
		}

		else if (MaterialType == MAT_TYPE_WOOD_CRATE)
		{
			if (this->SoundDependsOnMass)
			{
				if (mass >= 100.f)
				{
					return MAT_TYPE_WOOD_CRATE_HARD_NAME;
				}
				else
				{
					return MAT_TYPE_WOOD_CRATE_LIGHT_NAME;
				}
			}

		}

		else if (MaterialType == MAT_TYPE_WOOD_PLANK)
		{
			if (this->SoundDependsOnMass)
			{
				if (mass >= 100.f)
				{
					return MAT_TYPE_WOOD_PLANK_HARD_NAME;
				}
				else
				{
					return MAT_TYPE_WOOD_PLANK_LIGHT_NAME;
				}
			}
		}

		else if (MaterialType == MAT_TYPE_WOOD_BOX)
		{
			if (this->SoundDependsOnMass)
			{
				if (mass >= 100.f)
				{
					return MAT_TYPE_WOOD_BOX_HARD_NAME;
				}
				else
				{
					return MAT_TYPE_WOOD_BOX_LIGHT_NAME;
				}
			}
		}

		else
		{
			std::cout << "Warning: Physics object doesn't have material type set. Bug are known to appear" << std::endl;
			return "";
		}
	}

	//Gets name of sound for object's impact
	//This function is not obligatory and made for simplicity of code
	std::string getMaterialTypeImpactSoundName()const
	{
		std::string filename = "";

		if (MaterialType == MAT_TYPE_PLASTIC_BARREL)
		{
			std::string name = MAT_TYPE_PLASTIC_BARREL_IMPACT_BULLET_SOUND_NAME;
			filename = name + std::to_string(m_get_random_number(1, 3));
		}

		else if (MaterialType == MAT_TYPE_PLASTIC_BOX)
		{
			std::string name = MAT_TYPE_PLASTIC_BOX_IMPACT_BULLET_SOUND_NAME;
			filename = name + std::to_string(m_get_random_number(1, 5));
		}

		else if (MaterialType == MAT_TYPE_WOOD_CRATE)
		{
			std::string name = MAT_TYPE_WOOD_CRATE_IMPACT_BULLET_SOUND_NAME;
			filename = name + std::to_string(m_get_random_number(1, 4));
		}

		else if (MaterialType == MAT_TYPE_WOOD_PLANK)
		{
			std::string name = MAT_TYPE_WOOD_PLANK_IMPACT_BULLET_SOUND_NAME;
			filename = name + std::to_string(m_get_random_number(1, 4));
		}

		else if (MaterialType == MAT_TYPE_WOOD_BOX)
		{
			std::string name = MAT_TYPE_WOOD_BOX_IMPACT_BULLET_SOUND_NAME;
			filename = name + std::to_string(m_get_random_number(1, 4));
		}

		else
		{
			std::cout << "Warning: Physics object doesn't have material type set. Bug are known to appear" << std::endl;
		}
		return filename;
	}
};