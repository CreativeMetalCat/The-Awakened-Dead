#pragma once


#ifndef AMMO_DATA
#define AMMO_DATA
#endif // !AMMO_OBJECT_HEADER

//class to hold amount of ammo, type of ammo etc.
//should not be created as pointer to save memory
class ammo_data
{
public:
	//weapons that can use this ammo type
	int weapon_type = 0;

	//amount of clips that this object has
	int clip_amount = 1;

	//max amount of clips that pawn can have at one time
	//to get it's amount in ammo for specific type of weapon just multiply it by weapon's ammo_per_clip value
	int max_clips_amount = 100;

	//@param weapon_type
	//weapons that can use this ammo type
	//@param clip_amount
	//amount of clips
	//@param max_clip_amount
	//max amount of clips that pawn can have at one time
	//to get it's amount in ammo for specific type of weapon just multiply it by weapon's ammo_per_clip value
	ammo_data(int weapon_type = 0, int clip_amount = 1, int max_clips_amount = 100) :weapon_type(weapon_type), clip_amount(clip_amount)
	{

	}

	//only for NULL checking
	bool operator ==(int i)
	{
		return this == NULL;;
	}

	//only for NULL checking
	bool operator !=(int i) 
	{ 
		return this != NULL;
	}
};