#pragma once

#include "SceneActor.h"
#include "Animation.h"
#include "ItemContainer.h"
#include "Weapon.h"
#include "Projectile.h"
#include "MaterialTypes.h"
#include "ammo_data.h"
#include "object_point.h"


#define PAWN_PLAYER 12

class Player :public SceneActor
{
protected:

	size_t animIndex = 0;

	void UpdateSprites()
	{

		sprite.setRotation(RotationAngle);
		sprite.setScale(Scale);
		sprite.setOrigin(sf::Vector2f(body->GetPosition().x, body->GetPosition().y));


	}
public:

	std::vector<int>*shooting_sound_channel_ids = new std::vector<int>();
	//gets type of the object for the relations
	static int Type() { return PAWN_PLAYER; }

	//use it only if you do not not what object class you may encounter
	//MUST RETURN THE SAME TYPE AS TYPE() METHOD
	virtual int getType()const { return PAWN_PLAYER; }

	//way of keeping track of amount of ammo player has
	//this way player can have ammo for weapons that he currently doesn't have
	std::vector<ammo_data>*ammoData = new std::vector<ammo_data>();

	//temp value to use channel from state that player is currently in
	int reload_sound_channel_id = -1;

	//temp value to keep track of reloading
	float _time_in_reload = 0.f;

	//player can not shoot while reloading 
	//also used to set proper animation
	bool is_reloading = false;

	//sound type of surface that player is stepping on
	int footstep_sound_type = MAT_SOUND_TYPE_CONCRETE;

	//time before next footstep sound will play(they override each other)
	float time_per_footstep = 0.5f;

	//temp value used by state
	float time_footstep_elapsed = 0.f;

	//used in states to controll footstep sounds
	int footsteps_sound_channel_id = -1;

	float health = 100.f;


	//change animation and movement according to this value
	//mostly for the animation
	bool is_shooting = false;

	Animation::SpritesAnimation*currentAnimation = nullptr;
	Animation::SpritesAnimationsContainer*spritesAnimations = new Animation::SpritesAnimationsContainer();


	size_t weapon_id = 0;

	std::vector<Weapon*>*weapons = new std::vector<Weapon*>();

	//not used
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


	void onDamage(float damage, Object*object, Context*&context, std::string stateName) override
	{
		this->health -= damage;
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
			if (!ammoData->empty())
			{
				//temp way of searching
				for (size_t i = 0; i < ammoData->size(); i++)
				{
					if (ammoData->at(i).ammo_type == currentWeapon->weapon_ammo_type)
					{
						currentWeapon->clips = ammoData->at(i).clip_amount;
					}
				}
			}

			if (currentWeapon->weaponType == WEAPON_TYPE_TAD_RIFLE)
			{
				/*if (body->GetLinearVelocity().x > 0 || body->GetLinearVelocity().y > 0)
				{*/
				if (this->is_reloading)
				{
					SetAnimation("solder_rifle_reload");
				}
				else if (is_shooting)
				{
					SetAnimation("solder_rifle_shoot");
				}
				else
				{
					if (body->GetLinearVelocity().x > 0 || body->GetLinearVelocity().y > 0)
					{
						SetAnimation("solder_rifle_move");
					}
					else
					{
						SetAnimation("solder_rifle_idle");
					}
				}

				//}
			}
			if (currentWeapon->weaponType == WEAPON_TYPE_TAD_KNIFE)
			{
				/*if (body->GetLinearVelocity().x > 0 || body->GetLinearVelocity().y > 0)
				{*/
				if (this->is_reloading)
				{
					SetAnimation("solder_knife_idle");
				}
				else if (is_shooting)
				{
					if (this->spritesAnimations->getAnimationByName("solder_knife_attack")->GetCurrentFrameIndex() >= this->spritesAnimations->getAnimationByName("solder_knife_attack")->Sprites->size()-1)
					{
						this->spritesAnimations->getAnimationByName("solder_knife_attack")->SetFrame(0);
					}
					SetAnimation("solder_knife_attack");
				}
				else
				{
					if (body->GetLinearVelocity().x > 0 || body->GetLinearVelocity().y > 0)
					{
						SetAnimation("solder_knife_move");
					}
					else
					{
						SetAnimation("solder_knife_idle");
					}
				}

				//}
			}
			if (currentWeapon->weaponType == WEAPON_TYPE_TAD_SHOTGUN)
			{

				if (this->is_reloading)
				{
					SetAnimation("solder_shotgun_reload");
				}
				else if (is_shooting)
				{
					SetAnimation("solder_pistol_shoot");
				}
				else
				{
					if (body->GetLinearVelocity().x > 0 || body->GetLinearVelocity().y > 0)
					{
						SetAnimation("solder_shotgun_move");
					}
					else
					{
						SetAnimation("solder_shotgun_idle");
					}
				}
			}
			if (currentWeapon->weaponType == WEAPON_TYPE_TAD_PISTOL)
			{
				Scale.x = GetObjectRectangle().width / sprite.getTexture()->getSize().x;
				Scale.y = GetObjectRectangle().height / sprite.getTexture()->getSize().y;

				if (this->is_reloading)
				{
					SetAnimation("solder_pistol_reload");
				}
				else if (is_shooting)
				{
					SetAnimation("solder_pistol_shoot");
				}
				else
				{
					if (body->GetLinearVelocity().x > 0 || body->GetLinearVelocity().y > 0)
					{
						SetAnimation("solder_pistol_move");
					}
					else
					{
						SetAnimation("solder_pistol_idle");
					}
				}

				//if (body->GetLinearVelocity().x != 0 || body->GetLinearVelocity().y != 0)
				//{
				//	SetAnimation("solder_pistol_move");
				//}
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

		if (currentAnimation != NULL)
		{
			if (currentAnimation->IsRepeated == false)
			{
				if (is_shooting)
				{
					if (currentAnimation->GetCurrentFrameIndex() >= currentAnimation->Sprites->size() - 1)
					{
						is_shooting = false;
					}
				}
			}
		}

		this->collision.left = body->GetPosition().x + this->collision.width / 2;
		this->collision.top = body->GetPosition().y + this->collision.height / 2;

		if (!Children->empty())
		{
			for (size_t i = 0; i < Children->size(); i++)
			{
				
				if (dynamic_cast<PointObject*>(Children->at(i)))
				{
					
					/*Children->at(i)->SetObjectPosition(RotatePoint(Children->at(i)->GetObjectPosition(), this->RotationAngle, this->GetObjectPosition()));*/
				}
			}
		}

		if (!Projectiles->empty())
		{
			for (size_t i = 0; i < Projectiles->size(); i++)
			{
				Projectiles->at(i)->Update(dt);
				if (Projectiles->at(i)->getIsDone())
				{
					Projectiles->erase(std::find(Projectiles->begin(), Projectiles->end(), Projectiles->at(i)));
				}
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

		this->AttachChild(new PointObject(sf::Vector2f(70, 20),"rifle_shoot_point"));
		this->AttachChild(new PointObject(sf::Vector2f(140, 70), "pistol_shoot_point"));

		sf::Vector2f scale = { 1,1 };

		if (!spritesAnimations->animations->empty())
		{
			for (size_t in = 0; in < spritesAnimations->animations->size(); in++)
			{
				if (!spritesAnimations->animations->at(in)->Sprites->empty())
				{
					for (size_t i = 0; i < spritesAnimations->animations->at(in)->Sprites->size(); i++)
					{
						if (!spritesAnimations->animations->at(in)->ForceScale)
						{
							scale.x = collision.width / spritesAnimations->animations->at(in)->Sprites->at(i).getTexture()->getSize().x;
							scale.y = collision.height / spritesAnimations->animations->at(in)->Sprites->at(i).getTexture()->getSize().y;
						}

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

	//adds ammo 
	//if already have ammo for this type of weapon it will add to that
	//it only will add for the firts of this type to escape copying same object
	//if not it will create new entry for this type of ammo
	void AddAmmo(ammo_data data)
	{
		if (!ammoData->empty())
		{
			for (size_t i = 0; i < ammoData->size(); i++)
			{
				if (ammoData->at(i).ammo_type == data.ammo_type) 
				{
					//it only will add for the firts of this type to escape copying same object
					ammoData->at(i).clip_amount += data.clip_amount;
					return;
				}
			}
			//create new entry
			ammoData->push_back(data);
			return;
		}
		//create new entry
		ammoData->push_back(data);
	}

	//returns NULL on failure
	ammo_data FindAmmoDataByWeaponType(int ammo_type)
	{
		if (!ammoData->empty())
		{
			for (size_t i = 0; i < ammoData->size(); i++)
			{
				if (ammoData->at(i).ammo_type == ammo_type)
				{
					
					return ammoData->at(i);
				}
			}
			return NULL;
		}
		else
		{
			return NULL;
		}
	}

	//Gets name of current footstep according to inner values
	//This function is not obligatory and made for simplicity of code
	std::string GetFootstepSoundName()const
	{
		std::string filename = "";
		if (footstep_sound_type == MAT_SOUND_TYPE_CONCRETE)
		{
			std::string name = MAT_SOUND_TYPE_CONCRETE_NAME;
			filename = name + std::to_string(m_get_random_number(1, 4));
		}
		if (footstep_sound_type == MAT_SOUND_TYPE_CHAINLINK)
		{
			std::string name = MAT_SOUND_TYPE_CHAINLINK_NAME;
			filename = name + std::to_string(m_get_random_number(1, 4));
		}
		if (footstep_sound_type == MAT_SOUND_TYPE_DIRT)
		{
			std::string name = MAT_SOUND_TYPE_DIRT_NAME;
			filename = name + std::to_string(m_get_random_number(1, 4));
		}
		if (footstep_sound_type == MAT_SOUND_TYPE_DUCT)
		{
			std::string name = MAT_SOUND_TYPE_DUCT_NAME;
			filename = name + std::to_string(m_get_random_number(1, 4));
		}
		if (footstep_sound_type == MAT_SOUND_TYPE_GRASS)
		{
			std::string name = MAT_SOUND_TYPE_GRASS_NAME;
			filename = name + std::to_string(m_get_random_number(1, 4));
		}
		if (footstep_sound_type == MAT_SOUND_TYPE_GRAVEL)
		{
			std::string name = MAT_SOUND_TYPE_GRAVEL_NAME;
			filename = name + std::to_string(m_get_random_number(1, 4));
		}
		if (footstep_sound_type == MAT_SOUND_TYPE_LADDER)
		{
			std::string name = MAT_SOUND_TYPE_LADDER_NAME;
			filename = name + std::to_string(m_get_random_number(1, 4));
		}
		if (footstep_sound_type == MAT_SOUND_TYPE_METAL)
		{
			std::string name = MAT_SOUND_TYPE_METAL_NAME;
			filename = name + std::to_string(m_get_random_number(1, 4));
		}
		if (footstep_sound_type == MAT_SOUND_TYPE_METALGRATE)
		{
			std::string name = MAT_SOUND_TYPE_METALGRATE_NAME;
			filename = name + std::to_string(m_get_random_number(1, 4));
		}
		if (footstep_sound_type == MAT_SOUND_TYPE_MUD)
		{
			std::string name = MAT_SOUND_TYPE_MUD_NAME;
			filename = name + std::to_string(m_get_random_number(1, 4));
		}
		if (footstep_sound_type == MAT_SOUND_TYPE_SAND)
		{
			std::string name = MAT_SOUND_TYPE_SAND_NAME;
			filename = name + std::to_string(m_get_random_number(1, 4));
		}
		if (footstep_sound_type == MAT_SOUND_TYPE_SLOSH)
		{
			std::string name = MAT_SOUND_TYPE_SLOSH_NAME;
			filename = name + std::to_string(m_get_random_number(1, 4));
		}
		if (footstep_sound_type == MAT_SOUND_TYPE_TILE)
		{
			std::string name = MAT_SOUND_TYPE_TILE_NAME;
			filename = name + std::to_string(m_get_random_number(1, 4));
		}
		if (footstep_sound_type == MAT_SOUND_TYPE_WADE)
		{
			std::string name = MAT_SOUND_TYPE_WADE_NAME;
			filename = name + std::to_string(m_get_random_number(1, 4));
		}
		if (footstep_sound_type == MAT_SOUND_TYPE_WOOD)
		{
			std::string name = MAT_SOUND_TYPE_WOOD_NAME;
			filename = name + std::to_string(m_get_random_number(1, 4));
		}
		if (footstep_sound_type == MAT_SOUND_TYPE_WOODPANEL)
		{
			std::string name = MAT_SOUND_TYPE_WOODPANEL_NAME;
			filename = name + std::to_string(m_get_random_number(1, 4));
		}
		return filename;
	}
};