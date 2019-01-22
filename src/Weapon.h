#pragma once
#include "Item.h"

#pragma region weapons

#define WEAPON_TYPE_NONE 0x0

#define WEAPON_TYPE_TAD_PISTOL 0xA

#define WEAPON_TYPE_TAD_RIFLE 0xb

#define WEAPON_TYPE_TAD_SHOTGUN 0Xc

#define WEAPON_TYPE_TAD_KNIFE 0Xd

#define WEAPON_TYPE_TAD_FLASHLIGHT 0Xe

#define WEAPON_TYPE_TAD_GRENADE 0xf
#pragma endregion

//base class for weapons
class Weapon :public Item
{
private:
	//deprecated
	void fire(State*&state)
	{

	}


protected:

public:

	projectile*Projectile;

	size_t ammoPerClip = 0;
	size_t maxClips = 1;

	//amount of bullets that are shot
	int bullets_per_shot = 1;

	//texture that will be loaded from game's resources
	std::string projectile_texture_name = "";

	//sound that will be loaded from game's resources
	std::string shoot_sound_name = "";

	//sound that will be loaded from game's resources and played when out of ammo
	std::string empty_clip_sound = "";

	//value from 0 to 100
	int inaccuracy = 0.f;

	//in seconds
	float reload_time = 2.f;

	int ammoInTheClip = ammoPerClip;
	int  clips = maxClips;

	size_t weaponType = WEAPON_TYPE_NONE;
	float projectileSpeed = 0.f;
	float projectileDamage = 0.f;
	std::vector<Animation::Animation> * animations = new std::vector<Animation::Animation>();
	Animation::SpriteSheetAnimation* Anim;

	
	std::function<void(Object*object)>projectileOnCollision = [this](Object*object)
	{
		Projectile->CollidingObjects->push_back(object);
	};
	std::function<void(Object*object)>projectileLeftCollision = [this](Object*object)
	{
		if (!Projectile->CollidingObjects->empty())
		{
			Projectile->CollidingObjects->erase(std::find(Projectile->CollidingObjects->begin(), Projectile->CollidingObjects->end(), object));
		}

	};
	Weapon(std::string name, float projectileSpeed, float projectileDamage) :Item(name)
	{
		this->projectileSpeed = projectileSpeed;
		this->projectileDamage = projectileDamage;
	}

};
