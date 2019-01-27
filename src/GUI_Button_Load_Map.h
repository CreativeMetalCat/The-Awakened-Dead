#pragma once

#include "GUI.h"

namespace GUI
{
	//button that contains data for loading map from state
	class Button_Load_Map :public Button
	{
	protected:

	public:
		std::string map_path = "./../maps/";
		Button_Load_Map(std::string text, sf::Color textColor, sf::Font &font, int textSize, sf::Texture&texture);
		Button_Load_Map(std::string text, sf::Color textColor, sf::Font &font, int textSize, sf::Sprite sprite);
		Button_Load_Map(std::string text, sf::Color textColor, sf::Font &font, int textSize, sf::Texture&texture, sf::IntRect SpriteRect);
	};
}
