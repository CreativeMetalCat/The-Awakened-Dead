#pragma once
#include "Object.h"

//Base class for ALL drawable objects in scene that use sprites.
class SceneActor :public Object
{
protected:

public:

	sf::Sprite sprite;
	const sf::Sprite GetSprite() { return sprite; }
	void SetSprite(sf::Sprite) { this->sprite = sprite; }


	SceneActor(sf::Vector2f position, sf::Sprite sprite) :Object(position)
	{
		this->sprite = sprite;
		sprite.setPosition(position);

	}

	SceneActor(sf::Vector2f position, sf::Sprite sprite, float width, float height) :Object(position, width, height)
	{
		this->sprite = sprite;
		sprite.setPosition(position);

	}
	SceneActor(sf::Sprite sprite) :Object()
	{
		this->sprite = sprite;
	}
	SceneActor() {}

	void Draw(sf::RenderWindow*&window)
	{
		window->draw(sprite);
	}
	void Draw(sf::RenderWindow&window)
	{
		window.draw(sprite);
	}

	void Init()override
	{
		sf::Vector2f scale;
		scale.x = collision.width / sprite.getTextureRect().width;
		scale.y = collision.height / sprite.getTextureRect().height;
		sprite.setScale(scale);
		sprite.setPosition(sf::Vector2f(this->collision.left, this->collision.top));
		/*sprite.setOrigin(sf::Vector2f(sprite.getTextureRect().width*scale.x / 2, sprite.getTextureRect().height*scale.y / 2));*/

	}

};

