#include "GUI.h"

namespace GUI
{
	Component* Container::GetComponentByName(std::string name)
	{
		if (!Components->empty())
		{
			for (size_t i = 0; i < Components->size(); i++)
			{
				if (Components->at(i)->Name == name) { return Components->at(i); }
			}
		}
	}

	void Container::SelectNext()
	{
		do
			if (selectedIndex + 1 < Components->size())
			{
				if (Components->at(selectedIndex + 1)->isSelectable)
				{
					Components->at(selectedIndex + 1)->Select();
					selectedIndex++;
					return;
				}
			}
			else
			{
				selectedIndex = Components->size() - 1;
				if (Components->at(selectedIndex)->isSelectable)
				{
					Components->at(selectedIndex)->Select();
					return;
				}

			}
		while (selectedIndex < Components->size());
		return;
	}

	void Container::SelectPrevious()
	{
		int index = 0;
		do
		{
			index = selectedIndex;
			if (index - 1 < 0)
			{
				index = 0;
				selectedIndex = 0;
				if (Components->at(0)->isSelectable)
				{
					Components->at(0)->Select();
					return;
				}
				else
				{
					return;
				}

			}
			else
			{
				if (Components->at(index - 1)->isSelectable)
				{
					Components->at(index)->Select();
					index--;
					if (index >= 0) { selectedIndex = index; }
					else { selectedIndex = 0; }
					return;
				}

			}

		} while (index >= 0);
		return;
	}

	void Container::Select(size_t index)
	{
		if (Components->size() > index)
		{
			if (Components->at(index)->isSelectable) { Components->at(index)->Select(); }
		}
	}

	Label::Label(std::string text, sf::Color textColor, sf::Font &font, int textSize, sf::Texture&texture) :text(text, font, textSize), Sprite(texture), color(textColor)
	{
		Scale.x = texture.getSize().x / this->text.getGlobalBounds().width;
		Scale.y = texture.getSize().y / this->text.getGlobalBounds().height;
	}

	Label::Label(std::string text, sf::Color textColor, sf::Font &font, int textSize, sf::Texture&texture, sf::IntRect SpriteRect) :text(text, font, textSize), Sprite(texture, SpriteRect)
	{
		ComponentRectangle.height = SpriteRect.height;
		ComponentRectangle.width = SpriteRect.width;
		ComponentRectangle.top = SpriteRect.top;
		ComponentRectangle.left = SpriteRect.left;
	}

	Label::Label(std::string name, std::string text, sf::Color textColor, sf::Font &font, int textSize, sf::Texture&texture) :Component(name), text(text, font, textSize), Sprite(texture), color(textColor)
	{
		Scale.x = texture.getSize().x / this->text.getGlobalBounds().width;
		Scale.y = texture.getSize().y / this->text.getGlobalBounds().height;
	}

	Label::Label(std::string name, std::string text, sf::Color textColor, sf::Font &font, int textSize, sf::Texture&texture, sf::IntRect SpriteRect) :Component(name), text(text, font, textSize), Sprite(texture, SpriteRect)
	{
		ComponentRectangle.height = SpriteRect.height;
		ComponentRectangle.width = SpriteRect.width;
		ComponentRectangle.top = SpriteRect.top;
		ComponentRectangle.left = SpriteRect.left;
	}

	void Label::SetPosition(sf::Vector2f pos) 
	{

		text.setPosition(pos);
		this->ComponentRectangle.left = pos.x;
		this->ComponentRectangle.top = pos.y;
		Sprite.setPosition(pos);

	}

	void Label::Draw(sf::RenderWindow*& window)
	{
		window->draw(Sprite);
		window->draw(text);

	}

	void Label::Init()
	{
		_originalPos = this->text.getPosition();

		ComponentRectangle.width = this->text.getLocalBounds().width;
		ComponentRectangle.height = this->text.getLocalBounds().height;
		Scale.x = this->text.getLocalBounds().width / Sprite.getTexture()->getSize().x;
		Scale.y = this->text.getLocalBounds().height / Sprite.getTexture()->getSize().y;
		this->Sprite.setScale(Scale);
		this->Sprite.setPosition(sf::Vector2f((this->ComponentRectangle.left), (this->ComponentRectangle.top + this->text.getLocalBounds().height / 2)));
		this->text.setPosition(sf::Vector2f(this->ComponentRectangle.left, this->ComponentRectangle.top));
		this->text.setFillColor(color);
		this->text.setOutlineColor(color);
	}
	sf::Vector2<float> Label::GetPosition()
	{
		sf::Vector2f vec;
		vec.x = ComponentRectangle.left;
		vec.y = ComponentRectangle.top;
		return  vec;
	}



	Button::Button(std::string text, sf::Color textColor, sf::Font &font, int textSize, sf::Texture&texture) :text(text, font, textSize), Sprite(texture)
	{
		/*Sprite.setTextureRect(ToIntRect(this->text.getGlobalBounds()));*/
		Scale.x = texture.getSize().x / this->text.getGlobalBounds().width;
		Scale.y = texture.getSize().y / this->text.getGlobalBounds().height;
	}
	Button::Button(std::string text, sf::Color textColor, sf::Font &font, int textSize, sf::Sprite sprite) :text(text, font, textSize)
	{
		this->Sprite = sprite;
	}
	Button::Button(std::string text, sf::Color textColor, sf::Font &font, int textSize, sf::Texture&texture, sf::IntRect SpriteRect) :text(text, font, textSize), Sprite(texture, SpriteRect)
	{
		ComponentRectangle.height = SpriteRect.height;
		ComponentRectangle.width = SpriteRect.width;
		ComponentRectangle.top = SpriteRect.top;
		ComponentRectangle.left = SpriteRect.left;
	}

	//Disabled
	//If you need to make your one just override it 
	 void Button::Select() 
	{
		text.setColor(sf::Color::Red);
	}

	//Disabled 
	//If you need to make your one just override it 
	 void Button::UnSelect()  { text.setColor(sf::Color::White); }

	 sf::Vector2f Button::GetPosition()
	{
		sf::Vector2f vec;
		vec.x = ComponentRectangle.left;
		vec.y = ComponentRectangle.top;
		return  vec;
	}
	void Button::Init()
	{
		ComponentRectangle.width = this->text.getLocalBounds().width;
		ComponentRectangle.height = this->text.getLocalBounds().height;
		if (Sprite.getTexture() != NULL)
		{
			Scale.x = this->text.getLocalBounds().width / Sprite.getTexture()->getSize().x;
			Scale.y = this->text.getLocalBounds().height / Sprite.getTexture()->getSize().y;
		}	
		this->Sprite.setScale(Scale);
		this->Sprite.setPosition(sf::Vector2f((this->ComponentRectangle.left), (this->ComponentRectangle.top + this->text.getLocalBounds().height / 2)));
		this->text.setPosition(sf::Vector2f(this->ComponentRectangle.left, this->ComponentRectangle.top));
	}
	void Button::SetPosition(sf::Vector2f pos) 
	{

		text.setPosition(pos);
		this->ComponentRectangle.left = pos.x;
		this->ComponentRectangle.top = pos.y;
		Sprite.setPosition(pos);

	}

	//Calls class component Action;
	//.
	//If you need to make your one just override it 
	void Button::Activate() 
	{
		Action();
	}

	//Disabled 
	//If you need to make your one just override it 
	void Button::DeActivate()  {}

	void Button::Draw(sf::RenderWindow*& window) 
	{
		window->draw(Sprite);
		window->draw(text);

	}
}
