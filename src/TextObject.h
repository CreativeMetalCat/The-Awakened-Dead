#pragma once

#include "SFML/Graphics.hpp"
#include "Object.h"

class TextObject :public Object
{

protected:
	sf::Color Color;
	int Size = 0;
	std::string Text;

	bool m_size_defined_by_text = true;
public:

	sf::Color GetOriginalColor()
	{
		return Color;
	}
	sf::Text textObj;
	TextObject(std::string text, sf::Font&font, sf::Color color, int size, bool useTextSize = true)
	{
		this->Text = text;
		this->Color = color;
		this->Size = size;

		textObj = sf::Text(text, font, size);
	}
	virtual void OnCollision() {}
	virtual void SetObjectPosition(sf::Vector2f pos) override
	{
		collision.left = pos.x;
		collision.top = pos.y;

		textObj.setPosition(pos);
	}
};
