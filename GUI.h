#pragma once
#include <functional>
#include "SFML/Graphics.hpp"
namespace GUI
{
	class Component
	{
	protected:
		bool _isSelected = false;

		sf::Vector2f _originalPos;
	public:

		std::string Name = "";
		bool isSelectable = true;

		//gets position where object was put in creation
		//.
		//can be rewritten in childer classes if needed
		sf::Vector2f GetOriginalPosition()const { return _originalPos; }

		//can be used for state(mode)-dependent action by using lamba-expressions
		//e.g change state or affect state objects
		std::function<void()> Action = []() {};

		//so every GUI object will have rectangle
		sf::FloatRect ComponentRectangle;

		//perform action when object is selected
		//e.g change color of the text, size,texture
		virtual void Select() = 0;

		virtual void Init() {}
		//perform action when object is unselected
		//e.g change color of the text, size,texture
		virtual void UnSelect() = 0;

		const bool IsSelected() { return _isSelected; }

		//basic function
		//name says it all
		virtual void SetPosition(sf::Vector2f pos) = 0;


		virtual sf::Vector2f GetPosition() = 0;

		Component(std::string Name) { this->Name = Name; }

		Component() {}

		// supposed happen after Select(),or instead of it. (depends on event handling in state(mode))

		virtual void Activate() = 0;

		// supposed happen after UnSelect(),or instead of it. (depends on event handling in state(mode))
		virtual void DeActivate() = 0;

		virtual void Draw(sf::RenderWindow*& window) = 0;
	};

	class Container
	{
	public:
		std::vector<Component*>*Components = new std::vector<Component*>();
		size_t selectedIndex = 0;

		//returns component with this is name if one is existing
		Component*GetComponentByName(std::string name)
		{
			if (!Components->empty())
			{
				for (size_t i = 0; i < Components->size(); i++)
				{
					if (Components->at(i)->Name == name) { return Components->at(i); }
				}
			}
		}

		Container() {}
		void SelectNext()
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

		void SelectPrevious()
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

		void Select(size_t index)
		{
			if (Components->size() > index)
			{
				if (Components->at(index)->isSelectable) { Components->at(index)->Select(); }
			}
		}

	};

	class Label : public Component
	{
	protected:
		sf::Color color;
	public:
		sf::Text text;
		sf::Sprite Sprite;
		sf::Vector2f Scale;

		Label(std::string text, sf::Color textColor, sf::Font &font, int textSize, sf::Texture&texture) :text(text, font, textSize), Sprite(texture), color(textColor)
		{
			Scale.x = texture.getSize().x / this->text.getGlobalBounds().width;
			Scale.y = texture.getSize().y / this->text.getGlobalBounds().height;
		}
		Label(std::string text, sf::Color textColor, sf::Font &font, int textSize, sf::Texture&texture, sf::IntRect SpriteRect) :text(text, font, textSize), Sprite(texture, SpriteRect)
		{
			ComponentRectangle.height = SpriteRect.height;
			ComponentRectangle.width = SpriteRect.width;
			ComponentRectangle.top = SpriteRect.top;
			ComponentRectangle.left = SpriteRect.left;
		}

		Label(std::string name, std::string text, sf::Color textColor, sf::Font &font, int textSize, sf::Texture&texture) :Component(name), text(text, font, textSize), Sprite(texture), color(textColor)
		{
			Scale.x = texture.getSize().x / this->text.getGlobalBounds().width;
			Scale.y = texture.getSize().y / this->text.getGlobalBounds().height;
		}
		Label(std::string name, std::string text, sf::Color textColor, sf::Font &font, int textSize, sf::Texture&texture, sf::IntRect SpriteRect) :Component(name), text(text, font, textSize), Sprite(texture, SpriteRect)
		{
			ComponentRectangle.height = SpriteRect.height;
			ComponentRectangle.width = SpriteRect.width;
			ComponentRectangle.top = SpriteRect.top;
			ComponentRectangle.left = SpriteRect.left;
		}

		//Disabled because labels are't usually selectable
		//If you need to make your one just override it 
		virtual void Select() override {}

		//Disabled because labels are't usually selectable
		//If you need to make your one just override it 
		virtual void UnSelect() override {}

		//Disabled because labels are't usually selectable
		//If you need to make your one just override it 
		virtual void Activate() override {}

		//Disabled because labels are't usually selectable
		//If you need to make your one just override it 
		virtual void DeActivate() override {}

		void SetPosition(sf::Vector2f pos) override
		{

			text.setPosition(pos);
			this->ComponentRectangle.left = pos.x;
			this->ComponentRectangle.top = pos.y;
			Sprite.setPosition(pos);

		}
		void Draw(sf::RenderWindow*& window) override
		{
			window->draw(Sprite);
			window->draw(text);

		}
		void Init()override
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
		sf::Vector2<float> GetPosition()override
		{
			sf::Vector2f vec;
			vec.x = ComponentRectangle.left;
			vec.y = ComponentRectangle.top;
			return  vec;
		}
	};

	//base class for buttons
	class Button :public Component
	{
	public:
		sf::Text text;
		sf::Vector2f Scale;
		//base sprite for button
		sf::Sprite Sprite;

		Button(std::string text, sf::Color textColor, sf::Font &font, int textSize, sf::Texture&texture) :text(text, font, textSize), Sprite(texture)
		{
			/*Sprite.setTextureRect(ToIntRect(this->text.getGlobalBounds()));*/
			Scale.x = texture.getSize().x / this->text.getGlobalBounds().width;
			Scale.y = texture.getSize().y / this->text.getGlobalBounds().height;
		}
		Button(std::string text, sf::Color textColor, sf::Font &font, int textSize, sf::Sprite sprite) :text(text, font, textSize)
		{
			this->Sprite = sprite;
		}
		Button(std::string text, sf::Color textColor, sf::Font &font, int textSize, sf::Texture&texture, sf::IntRect SpriteRect) :text(text, font, textSize), Sprite(texture, SpriteRect)
		{
			ComponentRectangle.height = SpriteRect.height;
			ComponentRectangle.width = SpriteRect.width;
			ComponentRectangle.top = SpriteRect.top;
			ComponentRectangle.left = SpriteRect.left;
		}

		//Disabled
		//If you need to make your one just override it 
		virtual void Select() override
		{
			text.setColor(sf::Color::Red);
		}

		//Disabled 
		//If you need to make your one just override it 
		virtual void UnSelect() override { text.setColor(sf::Color::White); }

		virtual sf::Vector2f GetPosition()override
		{
			sf::Vector2f vec;
			vec.x = ComponentRectangle.left;
			vec.y = ComponentRectangle.top;
			return  vec;
		}
		void Init()override
		{
			ComponentRectangle.width = this->text.getLocalBounds().width;
			ComponentRectangle.height = this->text.getLocalBounds().width;
			Scale.x = this->text.getLocalBounds().width / Sprite.getTexture()->getSize().x;
			Scale.y = this->text.getLocalBounds().height / Sprite.getTexture()->getSize().y;
			this->Sprite.setScale(Scale);
			this->Sprite.setPosition(sf::Vector2f((this->ComponentRectangle.left), (this->ComponentRectangle.top + this->text.getLocalBounds().height / 2)));
			this->text.setPosition(sf::Vector2f(this->ComponentRectangle.left, this->ComponentRectangle.top));
		}
		void SetPosition(sf::Vector2f pos) override
		{

			text.setPosition(pos);
			this->ComponentRectangle.left = pos.x;
			this->ComponentRectangle.top = pos.y;
			Sprite.setPosition(pos);

		}

		//Calls class component Action;
		//.
		//If you need to make your one just override it 
		virtual void Activate() override
		{
			Action();
		}

		//Disabled 
		//If you need to make your one just override it 
		virtual void DeActivate() override {}

		void Draw(sf::RenderWindow*& window) override
		{
			window->draw(Sprite);
			window->draw(text);

		}
	};
}

