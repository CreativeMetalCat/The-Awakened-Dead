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
protected:

public:

	projectile*Projectile;

	size_t ammoPerClip = 0;
	size_t maxClips = 2;

	int ammoInTheClip = ammoPerClip;
	int  clips = maxClips;

	size_t weaponType = WEAPON_TYPE_NONE;
	float projectileSpeed = 0.f;
	float projectileDamage = 0.f;
	std::vector<Animation::Animation> * animations = new std::vector<Animation::Animation>();
	Animation::SpriteSheetAnimation* Anim;

	void fire(State&state)
	{

	}
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
