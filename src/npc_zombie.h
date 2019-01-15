#pragma once
#include "State.h"

#include "MovingPawn.h"
#include "Decal.h"


#include "Player.h"
#include "Animation.h"

class npc_zombie :public MovingPawn
{
protected:
	//index of current animation
	size_t animIndex = 0;

	bool IsDead = false;

	void UpdateSprites()
	{
		if (currentAnimation != NULL)
		{

			currentAnimation->CurrentSprite.setRotation(RotationAngle);
		}
		else
		{

		}
	}
public:
	float Health = 100.f;

	Animation::SpritesAnimation*currentAnimation = nullptr;
	Animation::SpritesAnimationsContainer*spritesAnimations = new Animation::SpritesAnimationsContainer();

	virtual void onDamage(float damage, Object*object, Context*&context, std::string stateName)
	{
		if (!IsDead)
		{
			Health -= damage;

			if (Health <= 0)
			{
				context->game->GetStateByName(stateName)->worldIsPaused = true;


				IsDead = true;
				Animation::Animation rhand_anim = Animation::Animation("rhand");
				rhand_anim.FrameIndexes->push_back(Animation::CellIndex(0, 0));


				Decal*rhand = new Decal(sf::Vector2f(this->body->GetPosition().x + 60, this->body->GetPosition().y), 0.05f, true, 20.0f, 123, 53, sf::Sprite(context->game->Resources->getTextureResourceDataByName("zombie_right_hand")->texture), 51, 27);
				b2PolygonShape shape;
				shape.SetAsBox(rhand->GetObjectRectangle().width / 2, rhand->GetObjectRectangle().height / 2);

				b2BodyDef rightHandDef;
				rightHandDef.position.Set(rhand->GetObjectPosition().x + rhand->GetObjectRectangle().width / 2, rhand->GetObjectPosition().y + rhand->GetObjectRectangle().height / 2);
				rightHandDef.type = b2BodyType::b2_dynamicBody;

				b2FixtureDef rhandFixture;
				rhandFixture.density = 1.f;
				rhandFixture.shape = &shape;

				rhand->animations->push_back(rhand_anim);
				rhand->SetAnimation("rhand");

				rhand->applyImpulse(b2Vec2(100, 0));


				context->game->GetStateByName(stateName)->CreatePhysicsObject(rhand, rightHandDef, rhandFixture, true);

				Decal*lhand = new Decal(sf::Vector2f(this->body->GetPosition().x, this->body->GetPosition().y + 30), 0.05f, true, 20.0f, 123, 53, sf::Sprite(context->game->Resources->getTextureResourceDataByName("zombie_left_hand")->texture), 51, 27);

				b2BodyDef leftHandDef;
				leftHandDef.position.Set(lhand->GetObjectPosition().x + lhand->GetObjectRectangle().width / 2, lhand->GetObjectPosition().y + lhand->GetObjectRectangle().height / 2);
				leftHandDef.type = b2BodyType::b2_dynamicBody;

				b2FixtureDef lhandFixture;
				lhandFixture.density = 1.f;
				lhandFixture.shape = &shape;

				lhand->animations->push_back(rhand_anim);
				lhand->SetAnimation("rhand");

				context->game->GetStateByName(stateName)->CreatePhysicsObject(lhand, leftHandDef, lhandFixture, 50.f);


				/*Decal*meat_chunk = new Decal(sf::Vector2f(this->body->GetPosition().x, this->body->GetPosition().y), 0.05f, true, 20.0f, 35, 65, sf::Sprite(context->game->Resources->getTextureResourceDataByName("meat_chunk")->texture), 100, 100);
				meat_chunk->Init();
				meat_chunk->animations->push_back(rhand_anim);
				meat_chunk->SetAnimation("rhand");
				context->game->GetStateByName(stateName)->StateObjects->push_back(meat_chunk);*/

				context->game->GetStateByName(stateName)->worldIsPaused = false;
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
			std::cout << "found player" << std::endl;
			sf::Vector2f diff;
			diff.x = object->GetObjectPosition().x - this->GetObjectPosition().x;
			diff.y = object->GetObjectPosition().y - this->GetObjectPosition().x;

			this->SetObjectRotation((atan2(diff.y, diff.x)*(180 / M_PI)));
		}
	}

	npc_zombie(sf::Vector2f position, float speed, float width, float height) :MovingPawn(speed, position, width, height)
	{
		collision.width = width;
		collision.height = height;
	}

	bool SetAnimation(std::string name)
	{
		if (!spritesAnimations->animations->empty())
		{
			for (size_t i = 0; i < spritesAnimations->animations->size(); i++)
			{
				if (spritesAnimations->animations->at(i)->Name == name)
				{
					spritesAnimations->AnimIndex = i;
					currentAnimation = spritesAnimations->animations->at(i);
					return true;
				}
			}
		}
		return false;
	}

	sf::Vector2f GetObjectPosition() override
	{
		return sf::Vector2f(collision.left, collision.top);
	}

	virtual void Draw(sf::RenderWindow*&window)override
	{
		if (IsDead != true)
		{
			if (currentAnimation != NULL)
			{
				window->draw(this->currentAnimation->CurrentSprite);
			}
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
		if (!spritesAnimations->animations->empty())
		{
			for (size_t in = 0; in < spritesAnimations->animations->size(); in++)
			{
				if (!spritesAnimations->animations->at(in)->Sprites->empty())
				{
					for (size_t i = 0; i < spritesAnimations->animations->at(in)->Sprites->size(); i++)
					{
						sf::Vector2f scale;
						scale.x = collision.width / spritesAnimations->animations->at(in)->Sprites->at(i).getTexture()->getSize().x;
						scale.y = collision.height / spritesAnimations->animations->at(in)->Sprites->at(i).getTexture()->getSize().y;

						spritesAnimations->animations->at(in)->Sprites->at(i).setRotation(RotationAngle);
						spritesAnimations->animations->at(in)->Sprites->at(i).setScale(scale);
						spritesAnimations->animations->at(in)->Sprites->at(i).setOrigin(sf::Vector2f(spritesAnimations->animations->at(in)->Sprites->at(i).getTextureRect().width / 2, spritesAnimations->animations->at(in)->Sprites->at(i).getTextureRect().height / 2));
					}
				}
			}
		}

	}

	void Update(sf::Time dt)
	{
		SetAnimation("skeleton_idle");
		if (physBodyInitialized)
		{
			this->collision.left = body->GetPosition().x;
			this->collision.top = body->GetPosition().y;
		}
		if (currentAnimation != NULL)
		{
			currentAnimation->UpdateAnimation(dt);
			currentAnimation->CurrentSprite.setPosition(sf::Vector2f(body->GetPosition().x, body->GetPosition().y));
			currentAnimation->CurrentSprite.setRotation(RotationAngle);
		}
		this->UpdateMovement(dt);
	}
};