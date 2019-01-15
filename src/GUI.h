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
		Component*GetComponentByName(std::string name);

		Container() {}
		void SelectNext();

		void SelectPrevious();

		void Select(size_t index);

	};

	class Label : public Component
	{
	protected:
		sf::Color color;
	public:
		sf::Text text;
		sf::Sprite Sprite;
		sf::Vector2f Scale;

		Label(std::string text, sf::Color textColor, sf::Font &font, int textSize, sf::Texture&texture);
		Label(std::string text, sf::Color textColor, sf::Font &font, int textSize, sf::Texture&texture, sf::IntRect SpriteRect);

		Label(std::string name, std::string text, sf::Color textColor, sf::Font &font, int textSize, sf::Texture&texture);
		Label(std::string name, std::string text, sf::Color textColor, sf::Font &font, int textSize, sf::Texture&texture, sf::IntRect SpriteRect);
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

		void SetPosition(sf::Vector2f pos)override;
		void Draw(sf::RenderWindow*& window) override;
		void Init()override;
		sf::Vector2<float> GetPosition()override;
	};

	//base class for buttons
	class Button :public Component
	{
	public:
		sf::Text text;
		sf::Vector2f Scale;
		//base sprite for button
		sf::Sprite Sprite;

		Button(std::string text, sf::Color textColor, sf::Font &font, int textSize, sf::Texture&texture);
		Button(std::string text, sf::Color textColor, sf::Font &font, int textSize, sf::Sprite sprite);
		Button(std::string text, sf::Color textColor, sf::Font &font, int textSize, sf::Texture&texture, sf::IntRect SpriteRect);

		//Disabled
		//If you need to make your one just override it 
		virtual void Select() override;

		//Disabled 
		//If you need to make your one just override it 
		virtual void UnSelect() override;

		virtual sf::Vector2f GetPosition()override;
		void Init()override;
		void SetPosition(sf::Vector2f pos) override;

		//Calls class component Action;
		//.
		//If you need to make your one just override it 
		virtual void Activate() override;

		//Disabled 
		//If you need to make your one just override it 
		virtual void DeActivate() override;

		void Draw(sf::RenderWindow*& window) override;
	};
}

