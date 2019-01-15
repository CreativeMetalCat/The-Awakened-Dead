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
	TextObject(sf::Vector2f pos,std::string text, sf::Font&font, sf::Color color, int size, bool useTextSize = true):Object(pos)
	{
		this->Text = text;
		this->Color = color;
		this->Size = size;

		textObj = sf::Text(text, font, size);
	}

	TextObject(sf::Vector2f pos, std::string text, sf::Color color, sf::Text textObj, bool useTextSize = true):Object(pos),textObj(textObj)
	{
		this->Text = text;
		this->Color = color;
		this->Size = textObj.getCharacterSize();	
		this->textObj.setString(text);
		this->textObj.setPosition(pos);
	}
	virtual void OnCollision() {}
	virtual void SetObjectPosition(sf::Vector2f pos) override
	{
		collision.left = pos.x;
		collision.top = pos.y;

		textObj.setPosition(pos);
	}

	void Init()override
	{
		this->textObj.setFillColor(Color);
		textObj.setString(Text);
	}

	void Update(sf::Time dt)override
	{
		textObj.setString(Text);
		this->collision = this->textObj.getGlobalBounds();
	/*	this->textObj.setPosition(sf::Vector2f(this->collision.left, this->collision.left));*/
	}

	void Draw(sf::RenderWindow*&window)
	{
		window->draw(textObj);
	}
	~TextObject()
	{
		Text.~basic_string();
	}
};
