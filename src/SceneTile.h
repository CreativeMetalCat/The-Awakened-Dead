#pragma once

#include "SceneActor.h"
#include "MaterialTypes.h"

//Tile that has no physical body and needed for proper map loading and drawing
class SceneTile :public SceneActor
{

public:

	//sound that will be played when character steps on it
	int mat_sound_type_id = MAT_SOUND_TYPE_CONCRETE;

	//it's id that is usually assinged by layer on the map 
	//needed for the optimization
	int area_id = 0;
	SceneTile(sf::Vector2f position, sf::Sprite sprite, int area_id = 0, int mat_sound_type_id = MAT_SOUND_TYPE_CONCRETE) :SceneActor(position, sprite, area_id), mat_sound_type_id(mat_sound_type_id)
	{
		this->sprite = sprite;
		sprite.setPosition(position);

	}

	SceneTile(sf::Vector2f position, sf::Sprite sprite, float width, float height,int area_id=0, int mat_sound_type_id = MAT_SOUND_TYPE_CONCRETE) :SceneActor(position, sprite, width, height, area_id), mat_sound_type_id(mat_sound_type_id)
	{
		this->sprite = sprite;
		sprite.setPosition(position);

	}
	SceneTile(sf::Sprite sprite, int area_id = 0,int mat_sound_type_id = MAT_SOUND_TYPE_CONCRETE) :SceneActor(sprite, area_id), mat_sound_type_id(mat_sound_type_id)
	{
		this->sprite = sprite;
	}
	SceneTile() {}
};