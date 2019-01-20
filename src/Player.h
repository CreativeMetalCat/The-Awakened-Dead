#pragma once

#include "SceneActor.h"
#include "Animation.h"
#include "ItemContainer.h"
#include "Weapon.h"
#include "Projectile.h"
#include "MaterialTypes.h"


class Player :public SceneActor
{
protected:

	size_t animIndex = 0;

	/*TextureResource* solder_rifle = new TextureResource("solder_rifle", "./textures/survivor_move_rifle_anim.png", false, false);
	TextureResource* solder_pistol = new TextureResource("solder", "./textures/player_idle.gif", false, false);*/

	void UpdateSprites()
	{
		if (currentAnimation != NULL)
		{
			/*if (currentAnimation->CurrentSprite.getTexture() != NULL)
			{
				Scale.x = collision.width / currentAnimation->CurrentSprite.getTexture()->getSize().x;
				Scale.y = collision.height / currentAnimation->CurrentSprite.getTexture()->getSize().y;
			}
			else
			{
				Scale.x = collision.width / sprite.getTexture()->getSize().x;
				Scale.y = collision.height / sprite.getTexture()->getSize().y;
			}
			for (size_t i = 0; i < currentAnimation->Sprites->size();i++)
			{
				currentAnimation->Sprites->at(i).setRotation(RotationAngle);
				currentAnimation->Sprites->at(i).setScale(Scale);
				currentAnimation->Sprites->at(i).setOrigin(sf::Vector2f(currentAnimation->Sprites->at(i).getTextureRect().width / 2, currentAnimation->Sprites->at(i).getTextureRect().height / 2));
			}*/


			/*currentAnimation->CurrentSprite.setRotation(RotationAngle);*/
			/*if (currentAnimation->GetCurrentFrameIndex() < currentAnimation->Sprites->size())
			{
				currentAnimation->Sprites->at(currentAnimation->GetCurrentFrameIndex()).setRotation(RotationAngle);
			}*/

			/*currentAnimation->CurrentSprite.setScale(Scale);
			currentAnimation->CurrentSprite.setOrigin(sf::Vector2f(collision.width / 2, collision.height / 2));*/
		}
		else
		{
			/*Scale.x = collision.width / sprite.getTexture()->getSize().x;
			Scale.y = collision.height / sprite.getTexture()->getSize().y;*/
		}

		sprite.setRotation(RotationAngle);
		sprite.setScale(Scale);
		sprite.setOrigin(sf::Vector2f(body->GetPosition().x, body->GetPosition().y));


	}
public:
	
	int footstep_sound_type = MAT_SOUND_TYPE_CONCRETE;

	float time_per_footstep = 0.5f;

	float time_footstep_elapsed = 0.f;

	//used in states to controll footstep sounds
	int footsteps_sound_channel_id = -1;

	float health = 100.f;
	/*std::vector<Animation::Animation> * animations = new std::vector<Animation::Animation>();
	Animation::SpriteSheetAnimation* Anim;*/

	Animation::SpritesAnimation*currentAnimation = nullptr;
	Animation::SpritesAnimationsContainer*spritesAnimations = new Animation::SpritesAnimationsContainer();


	size_t weapon_id = 0;

	std::vector<Weapon*>*weapons = new std::vector<Weapon*>();

	ItemContainer*items = new ItemContainer(3, 1, 3);

	float MaxSpeed = 1.f;
	float accel = 0.5f;
	sf::Vector2f Velocity;
	std::vector<projectile*>*Projectiles = new std::vector<projectile*>();
	Weapon*currentWeapon = nullptr;
	Player(sf::Vector2f position, float width, float height, sf::Sprite sprite) :SceneActor(position, sprite)
	{
		collision.width = width;
		collision.height = height;
	}



	Player(sf::Sprite sprite) :SceneActor(sprite)
	{

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

	void AddVelocity(sf::Vector2f vel)
	{
		this->Velocity += vel;
	}
	void SetVelocity(sf::Vector2f vel)
	{
		this->Velocity = vel;
	}

	void SetVelocityX(float x)
	{
		this->Velocity.x = x;
	}
	void SetVelocityY(float y)
	{
		this->Velocity.y = y;
	}

	void Update(sf::Time dt)override
	{

		body->SetTransform(body->GetPosition(), RotationAngle);
		if (!weapons->empty())
		{
			currentWeapon = weapons->at(weapon_id);
			if (currentWeapon->weaponType == WEAPON_TYPE_TAD_RIFLE)
			{
				/*if (body->GetLinearVelocity().x > 0 || body->GetLinearVelocity().y > 0)
				{*/
				SetAnimation("solder_rifle_move");
				//}
			}
			if (currentWeapon->weaponType == WEAPON_TYPE_TAD_PISTOL)
			{
				Scale.x = GetObjectRectangle().width / sprite.getTexture()->getSize().x;
				Scale.y = GetObjectRectangle().height / sprite.getTexture()->getSize().y;

				if (body->GetLinearVelocity().x != 0 || body->GetLinearVelocity().y != 0)
				{
					SetAnimation("solder_pistol_move");
				}
			}
			UpdateSprites();
		}
		else
		{
			currentWeapon = nullptr;
		}

		if (currentAnimation != NULL)
		{
			currentAnimation->UpdateAnimation(dt);
			currentAnimation->CurrentSprite.setPosition(sf::Vector2f(body->GetPosition().x, body->GetPosition().y));
			currentAnimation->CurrentSprite.setRotation(RotationAngle);
		}

		/*if (!animations->empty())
		{
			Anim->Time += dt.asSeconds();
			if (Anim->Time >= Anim->FrameDuration)
			{
				Anim->SetFrame(animations->at(animIndex).FrameIndexes->at(animations->at(animIndex).CurrentFrameIndex).ColumnIndex, animations->at(animIndex).FrameIndexes->at(animations->at(animIndex).CurrentFrameIndex).StripIndex);
				Anim->Time = 0;
				animations->at(animIndex).CurrentFrameIndex++;
				if (animations->at(animIndex).CurrentFrameIndex > animations->at(animIndex).FrameIndexes->size() - 1)
				{
					animations->at(animIndex).CurrentFrameIndex = 0;
				}
			}
		}*/



		/*this->Move(sf::Vector2f(Velocity.x*(dt.asMilliseconds()), Velocity.y*(dt.asMilliseconds())));*/
		/*this->Anim->sprite.setPosition(sf::Vector2f(body->GetPosition().x, body->GetPosition().y));*/

		this->collision.left = body->GetPosition().x + this->collision.width / 2;
		this->collision.top = body->GetPosition().y + this->collision.height / 2;

		if (!Projectiles->empty())
		{
			for (size_t i = 0; i < Projectiles->size(); i++)
			{
				Projectiles->at(i)->Update(dt);
			}
		}

	}
	virtual void Draw(sf::RenderWindow*&window)override
	{
		if (currentAnimation != NULL)
		{
			window->draw(this->currentAnimation->CurrentSprite);
		}

		if (!Children->empty())
		{
			for (size_t i = 0; i < this->Children->size(); i++)
			{
				Children->at(i)->Draw(window);
			}
		}
		if (!Projectiles->empty())
		{
			for (size_t i = 0; i < Projectiles->size(); i++)
			{
				Projectiles->at(i)->Draw(window);
			}
		}
	}

	void Move(sf::Vector2f vec)override
	{
		collision.left += vec.x;
		collision.top += vec.y;
		sprite.move(vec);
	}

	void SetObjectPosition(sf::Vector2f pos)override
	{
		collision.left = pos.x;
		collision.top = pos.y;
		sprite.setPosition(pos);
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

		//this->Anim->sprite.setScale(this->Scale);
		//this->Anim->sprite.setPosition(sf::Vector2f(collision.left, collision.top));
		//this->Anim->sprite.setRotation(RotationAngle);
		//this->Anim->sprite.setOrigin(sf::Vector2f(collision.width / 2, collision.height / 2));

		this->sprite.setPosition(sf::Vector2f(collision.left, collision.top));

		/*solder_pistol->CreateResourceFromFile();
		solder_rifle->CreateResourceFromFile();*/

		sprite.setRotation(RotationAngle);
		sprite.setScale(Scale);
		sprite.setOrigin(sf::Vector2f(collision.width / 2, collision.height / 2));
	}



};