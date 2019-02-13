#pragma once

#include "npc_zombie_base.h"
#include "MaterialTypes.h"
#include "ai_node.h"



//the awakened dead zombie
#define PAWN_ZOMBIE_TAD 13


class npc_zombie :public npc_zombie_base
{
private:
	//float attacc = meme.size();
protected:
	//index of current animation
	size_t animIndex = 0;

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

	bool isAttacking = false;

	float timeInAttack = 0.f;

	Object*attackTarget = NULL;

	//used for attacking
	Context*context;

	//used for attacking
	std::string stateName = "";
public:

	ai_node*usedNode = NULL;

	//gets type of the object for the relations
	static int Type() { return PAWN_ZOMBIE_TAD; }

	//use it only if you do not not what object class you may encounter
	//MUST RETURN THE SAME TYPE AS TYPE() METHOD
	virtual int getType()const { return PAWN_ZOMBIE_TAD; }

	

	//body used for attacking
	b2Body* attackBody;

	//test object 
	//object that npc will follow
	Object*target = NULL;

	

	//time that takes for object to end attacking(end animation, sound etc.)
	//damage will be applied on the start of the time
	float attackTime = 0.3f;

	Animation::SpritesAnimation*currentAnimation = nullptr;
	Animation::SpritesAnimationsContainer*spritesAnimations = new Animation::SpritesAnimationsContainer();

	virtual void onDamage(float damage, Object*object, Context*&context, std::string stateName)override
	{
		if (!IsDead)
		{
			context->game->GetStateByName(stateName)->PlaySound("zombie_pain");
			Health -= damage;

			if (Health <= 0)
			{
				this->Pattern->clear();
				this->body->SetLinearVelocity(b2Vec2(0, 0));
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

			

				Decal*blood_splat = new Decal(sf::Vector2f(this->body->GetPosition().x-50, this->body->GetPosition().y-50), 0.05f, true, 20.0f, 133, 100, sf::Sprite(context->game->Resources->getTextureResourceDataByName("blood_splat_1")->texture), 200, 100);
				blood_splat->Init();
				context->game->GetStateByName(stateName)->StateObjects->push_back(blood_splat);

				Decal*meat_chunk = new Decal(sf::Vector2f(this->body->GetPosition().x, this->body->GetPosition().y), 0.05f, true, 20.0f, 35, 65, sf::Sprite(context->game->Resources->getTextureResourceDataByName("meat_chunk")->texture), 20, 30);
				meat_chunk->Init();
				meat_chunk->animations->push_back(rhand_anim);
				meat_chunk->SetAnimation("rhand");
				context->game->GetStateByName(stateName)->StateObjects->push_back(meat_chunk);

				context->game->GetStateByName(stateName)->worldIsPaused = false;

				IsDead = true;
			}
		}
	}

	virtual void leftCollision(Object*&object, b2Fixture *fixtureA, b2Fixture *fixtureB, Context*&context, std::string stateName)
	{
		if (static_cast<npc_zombie*>(fixtureA->GetBody()->GetUserData()) == this)
		{
			//Zombie has one fixure that is responsible for it's senses and it is set to sensor
			if (fixtureA->IsSensor())
			{
				if (target != NULL && target == object)
				{
					
					target = NULL;

				}
			}
			else
			{
				if (object == attackTarget)
				{
					attackTarget = NULL;
				}
			}
		}

		else if (static_cast<npc_zombie*>(fixtureB->GetBody()->GetUserData()) == this)
		{
			//Zombie has one fixure that is responsible for it's senses and it is set to sensor
			if (fixtureB->IsSensor())
			{
				if (target != NULL && target == object)
				{
					
					target = NULL;
				}

			}
			else
			{
				if (object == attackTarget)
				{
					attackTarget = NULL;
				}
			}
		}

	}

	virtual void onCollision(Object*&object, b2Fixture *fixtureA, b2Fixture *fixtureB, Context*&context, std::string stateName)
	{

		if (!IsDead)
		{

			if (static_cast<npc_zombie*>(fixtureA->GetBody()->GetUserData()) == this)
			{
				
				/*if (dynamic_cast<Player*>(object))
				{
					this->addRelation(ai_relationtype(RelationType::Noone, Player::Type()));
					this->addRelation(ai_relationtype(RelationType::Ally, npc_zombie::Type()));
				}*/
				if (dynamic_cast<ai_node*>(object))
				{
					if (static_cast<FixtureData*>(fixtureA->GetUserData())->getActionType() == FixtureActionType::Collision)
					{
						if (static_cast<FixtureData*>(fixtureB->GetUserData())->getActionType() == FixtureActionType::AI_Node_Action)
						{
							this->usedNode = dynamic_cast<ai_node*>(object);
							if (usedNode != NULL)
							{
								if (usedNode->next != NULL)
								{
									this->target = usedNode->next;
								}
							}
						}
						 
					}
					else if (static_cast<FixtureData*>(fixtureA->GetUserData())->getActionType() == FixtureActionType::Sense)
					{
						if (static_cast<FixtureData*>(fixtureB->GetUserData())->getActionType() == FixtureActionType::Trigger)
						{
							this->target = object;
						}
					}
					else
					{
						FixtureData*d = static_cast<FixtureData*>(fixtureA->GetUserData());
						std::cout << d->getActionType() << std::endl;;

					}

					
				}
				if (this->getRelationWithPawnType(object->getType()) == RelationType::Enemy)
				{
					if (!fixtureA->IsSensor())
					{
						if (!isAttacking)
							this->Attack(object, context, stateName);
					}


					sf::Vector2f diff;
					diff.x = object->GetObjectPosition().x - this->body->GetPosition().x;
					diff.y = object->GetObjectPosition().y - this->body->GetPosition().y;

					this->SetObjectRotation((atan2(diff.y, diff.x)*(180 / M_PI)));

					target = object;
				}
				if (this->getRelationWithPawnType(object->getType()) == RelationType::Ally)
				{
					sf::Vector2f diff;
					diff.x = object->GetObjectPosition().x - this->body->GetPosition().x;
					diff.y = object->GetObjectPosition().y - this->body->GetPosition().y;

					this->SetObjectRotation((atan2(diff.y, diff.x)*(180 / M_PI)));

					target = object;
				}
				if (this->getRelationWithPawnType(object->getType()) == RelationType::Follow)
				{
					//it is techically same as Ally with the difference that Ally should have more complexity
					//zombies Ally hovever is close to following
					//but this one is made for ai_nodes
					//because object is supposed to walk close to ai_nodes area of action

					sf::Vector2f diff;
					diff.x = object->GetObjectPosition().x - this->body->GetPosition().x;
					diff.y = object->GetObjectPosition().y - this->body->GetPosition().y;

					this->SetObjectRotation((atan2(diff.y, diff.x)*(180 / M_PI)));

					target = object;
				}
				/*if (dynamic_cast<Player*>(object))
				{
					if (this->getRelationWithPawnType(Player::Type()) == RelationType::Enemy)
					{
						if (!fixtureA->IsSensor())
						{
							if (!isAttacking)
								this->Attack(object, context, stateName);
						}


						sf::Vector2f diff;
						diff.x = object->GetObjectPosition().x - this->body->GetPosition().x;
						diff.y = object->GetObjectPosition().y - this->body->GetPosition().y;

						this->SetObjectRotation((atan2(diff.y, diff.x)*(180 / M_PI)));

						target = object;
					}
					else if (this->getRelationWithPawnType(Player::Type()) == RelationType::Ally)
					{
						sf::Vector2f diff;
						diff.x = object->GetObjectPosition().x - this->body->GetPosition().x;
						diff.y = object->GetObjectPosition().y - this->body->GetPosition().y;

						this->SetObjectRotation((atan2(diff.y, diff.x)*(180 / M_PI)));

						target = object;
					}

				}

				if (dynamic_cast<npc_zombie_base*>(object))
				{
					if (!fixtureB->IsSensor())
					{
						if (!isAttacking)
							this->Attack(object, context, stateName);
					}

					sf::Vector2f diff;
					diff.x = object->GetObjectPosition().x - this->body->GetPosition().x;
					diff.y = object->GetObjectPosition().y - this->body->GetPosition().y;

					this->SetObjectRotation((atan2(diff.y, diff.x)*(180 / M_PI)));

					target = object;
				}*/
			}

			else if (static_cast<npc_zombie*>(fixtureB->GetBody()->GetUserData()) == this)
			{

				/*if (dynamic_cast<Player*>(object))
				{
					this->addRelation(ai_relationtype(RelationType::Enemy, Player::Type()));
					this->addRelation(ai_relationtype(RelationType::Ally, npc_zombie::Type()));
				}*/
				if (dynamic_cast<ai_node*>(object))
				{
					if (static_cast<FixtureData*>(fixtureA->GetUserData())->getActionType() == FixtureActionType::Collision)
					{
						if (static_cast<FixtureData*>(fixtureB->GetUserData())->getActionType() == FixtureActionType::AI_Node_Action)
						{
							this->usedNode = dynamic_cast<ai_node*>(object);
							if (usedNode != NULL)
							{
								if (usedNode->next != NULL)
								{
									this->target = usedNode->next;
								}
							}
						}

					}
					else if (static_cast<FixtureData*>(fixtureA->GetUserData())->getActionType() == FixtureActionType::Sense)
					{
						if (static_cast<FixtureData*>(fixtureB->GetUserData())->getActionType() == FixtureActionType::Trigger)
						{
							this->target = object;
						}
					}
					else
					{
						FixtureData*d = static_cast<FixtureData*>(fixtureA->GetUserData());
						std::cout << d->getActionType() << std::endl;;

					}

				}
				if (this->getRelationWithPawnType(object->getType()) == RelationType::Enemy)
				{
					if (!fixtureB->IsSensor())
					{
						if (!isAttacking)
							this->Attack(object, context, stateName);
					}


					sf::Vector2f diff;
					diff.x = object->GetObjectPosition().x - this->body->GetPosition().x;
					diff.y = object->GetObjectPosition().y - this->body->GetPosition().y;

					this->SetObjectRotation((atan2(diff.y, diff.x)*(180 / M_PI)));

					target = object;
				}
				if (this->getRelationWithPawnType(object->getType()) == RelationType::Ally)
				{
					sf::Vector2f diff;
					diff.x = object->GetObjectPosition().x - this->body->GetPosition().x;
					diff.y = object->GetObjectPosition().y - this->body->GetPosition().y;

					this->SetObjectRotation((atan2(diff.y, diff.x)*(180 / M_PI)));

					target = object;
				}
				if (this->getRelationWithPawnType(object->getType()) == RelationType::Follow)
				{
					//it is techically same as Ally with the difference that Ally should have more complexity
					//zombies Ally hovever is close to following
					//but this one is made for ai_nodes
					//because object is supposed to walk close to ai_nodes area of action

					sf::Vector2f diff;
					diff.x = object->GetObjectPosition().x - this->body->GetPosition().x;
					diff.y = object->GetObjectPosition().y - this->body->GetPosition().y;

					this->SetObjectRotation((atan2(diff.y, diff.x)*(180 / M_PI)));

					target = object;
				}


				if (dynamic_cast<npc_zombie_base*>(object))
				{
					if (!fixtureB->IsSensor())
					{
						if (!isAttacking)
							this->Attack(object, context, stateName);
					}

					sf::Vector2f diff;
					diff.x = object->GetObjectPosition().x - this->body->GetPosition().x;
					diff.y = object->GetObjectPosition().y - this->body->GetPosition().y;

					this->SetObjectRotation((atan2(diff.y, diff.x)*(180 / M_PI)));

					target = object;
				}
			}


		}
	}

	npc_zombie(sf::Vector2f position, float speed, float width, float height, int area_id = 0) :npc_zombie_base(position, speed, width, height)
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
		Scale.x = 0.4f;
		Scale.y = 0.45f;
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
						spritesAnimations->animations->at(in)->Sprites->at(i).setScale(Scale);
						spritesAnimations->animations->at(in)->Sprites->at(i).setOrigin(sf::Vector2f(spritesAnimations->animations->at(in)->Sprites->at(i).getTextureRect().width / 2, spritesAnimations->animations->at(in)->Sprites->at(i).getTextureRect().height / 2));
					}
				}
			}
		}

	}

	//attacks current target if it isn't NULL
	void Attack(Context*&context, std::string stateName)
	{
		if (target != NULL)
		{
			isAttacking = true;

			target->onDamage(20.f, this, context, stateName);
			timeInAttack = 0.f;

			SetAnimation("skeleton_attack");

			int id = m_get_random_number(1, 2);
			context->game->PlaySound("zombie_attack"+std::to_string(id));
		}
	}

	//attcks given object.... well... that's all
	void Attack(Object*&obj, Context*&context, std::string stateName)
	{
		attackTarget = obj;
		isAttacking = true;

		/*obj->onDamage(20.f, this, context, stateName);*/
		this->context = context;
		this->stateName = stateName;

		timeInAttack = 0.f;

		SetAnimation("skeleton_attack");

		bool isPlaying = false;

		if (voice_sound_channel_id > 0)
		{
			context->game->Channels->at(voice_sound_channel_id)->isPlaying(&isPlaying);
		}

		if (!isPlaying)
		{
			int id = m_get_random_number(1, 2);
			context->game->PlaySound("zombie_attack" + std::to_string(id),voice_sound_channel_id);
		}



	}

	void Update(sf::Time dt)
	{
		
		
		if (IsDead)
		{
			target = NULL;
		}
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
		if (target != NULL)
		{
			sf::Vector2f diff;
			diff.x = target->GetObjectPosition().x - this->body->GetPosition().x;
			diff.y = target->GetObjectPosition().y - this->body->GetPosition().y;

			this->SetObjectRotation((atan2(diff.y, diff.x)*(180 / M_PI)));

			this->Done = false;
			this->Pattern->clear();
			this->body->SetLinearVelocity(b2Vec2(0, 0));
			this->dirIndex = 0;
			this->AddMovement(MovementDirection(static_cast<float>((atan2(diff.y, diff.x)*(180 / M_PI))), sqrt(diff.x*diff.x + diff.y*diff.y)));
		}

		this->UpdateMovement(dt);


		if (isAttacking)
		{
			timeInAttack += dt.asSeconds();
			if (timeInAttack >= attackTime)
			{
				if (attackTarget != NULL)
				{
					attackTarget->onDamage(20.f, this, context, stateName);
				}
				
				isAttacking = false;
			}
		}
		else
		{
			if (body->GetLinearVelocity().x != 0 || body->GetLinearVelocity().y != 0)
			{
				SetAnimation("skeleton_move");
			}
			else
			{
				SetAnimation("skeleton_idle");
			}
		}
		

	
	}
};
