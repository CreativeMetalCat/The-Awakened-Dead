#include "GUI_Button_Load_Map.h"

namespace GUI
{
	Button_Load_Map::Button_Load_Map(std::string text, sf::Color textColor, sf::Font &font, int textSize, sf::Texture&texture):Button(text,textColor,font,textSize,texture)
	{

	}
	Button_Load_Map::Button_Load_Map(std::string text, sf::Color textColor, sf::Font &font, int textSize, sf::Sprite sprite):Button(text, textColor, font, textSize,sprite)
	{

	}
	Button_Load_Map::Button_Load_Map(std::string text, sf::Color textColor, sf::Font &font, int textSize, sf::Texture&texture, sf::IntRect SpriteRect):Button(text, textColor, font, textSize, texture,SpriteRect)
	{

	}
}