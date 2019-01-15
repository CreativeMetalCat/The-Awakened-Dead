#pragma once
#include "SceneActor.h"

//static solid objects like walls
//Every other static object will be created without proper collision
class SolidObject :public SceneActor
{
public:
	SolidObject(sf::Vector2f position, sf::Sprite sprite) :SceneActor(position, sprite)
	{
		this->sprite = sprite;
		sprite.setPosition(position);

	}
	SolidObject(sf::Vector2f position, sf::Sprite sprite, float width, float height) : SceneActor(position, sprite, width, height)
	{
		this->sprite = sprite;
		sprite.setPosition(position);

	}
	SolidObject(sf::Sprite sprite) :SceneActor(sprite)
	{
		this->sprite = sprite;
	}

};