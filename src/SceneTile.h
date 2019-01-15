#pragma once

#include "SceneActor.h"

//Tile that has no physical body and needed for proper map loading and drawing
class SceneTile :public SceneActor
{
public:
	SceneTile(sf::Vector2f position, sf::Sprite sprite) :SceneActor(position, sprite)
	{
		this->sprite = sprite;
		sprite.setPosition(position);

	}

	SceneTile(sf::Vector2f position, sf::Sprite sprite, float width, float height) :SceneActor(position, sprite, width, height)
	{
		this->sprite = sprite;
		sprite.setPosition(position);

	}
	SceneTile(sf::Sprite sprite) :SceneActor(sprite)
	{
		this->sprite = sprite;
	}
	SceneTile() {}
};