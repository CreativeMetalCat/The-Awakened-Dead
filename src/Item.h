#pragma once
#include "SFML/Graphics.hpp"
#include <iostream>

//base class for items that can be put in the inventory etc.
//this class is made for data handling
class Item
{
private:

protected:

public:
	//item's sprite
	//usage is not obligatory can be relaced with any kind of drawable object
	sf::Sprite sprite;

	std::string name;

	Item(std::string name) :name(name) {}

	virtual void Draw(sf::RenderWindow*&window)
	{
		window->draw(sprite);
	}
};
