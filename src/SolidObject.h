#pragma once
#include "SceneActor.h"

//static solid objects like walls
//Every other static object will be created without proper collision
class SolidObject :public SceneActor
{
public:
	SolidObject(sf::Vector2f position, sf::Sprite sprite, int area_id = 0) :SceneActor(position, sprite, area_id)
	{
		this->sprite = sprite;
		sprite.setPosition(position);

	}
	SolidObject(sf::Vector2f position, sf::Sprite sprite, float width, float height, int area_id = 0) : SceneActor(position, sprite, width, height, area_id)
	{
		this->sprite = sprite;
		sprite.setPosition(position);

	}
	SolidObject(sf::Sprite sprite, int area_id = 0) :SceneActor(sprite, area_id)
	{
		this->sprite = sprite;
	}

};