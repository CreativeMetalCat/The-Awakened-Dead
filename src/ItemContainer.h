#pragma once
#include "Item.h"

//class that acts like inventory made of items
class ItemContainer
{
protected:



	//number of items per strip
	//mostly needed for displaying items in states
	int _stripLenght = 1;

	//number of items per row
	//mostly needed for displaying items in states
	int _rowLenght = 1;

	//-1 for infinite
	int _maxItemsCount = 1;


public:

	//used by gui components such as ItemInventoryUIC
	sf::Vector2f onScreenPosition;

	//id of item that's beeing used by mouse etc.
	//assinged by state
	int currentItemId = -1;

	//should container be drawn
	bool isVisible = false;

	//can be interacted with (mouse clicks etc.)
	bool isActive = false;


	//all items in container
	std::vector<Item*>*_items = new std::vector<Item*>();

	const bool isEmpty()const
	{
		return _items->empty();
	}

	//doesn't add if stack is full
	void addItem(Item*&item)
	{
		if (_maxItemsCount != -1)
		{
			if (_items->size() < _maxItemsCount)
			{
				_items->push_back(item);
			}
		}
	}

	//draws all items in stack by calling Item::Draw(sf::RenderWindow*&window)
	void drawItems(sf::RenderWindow*&window)
	{
		if (!_items->empty())
		{

			int stripId = 0;
			int rowId = 0;
			for (size_t i = 0; i < _items->size(); i++)
			{

				if (stripId <= _stripLenght)
				{
					_items->at(i)->sprite.setPosition(onScreenPosition.x + 20 * stripId, onScreenPosition.y*rowId);
					_items->at(i)->Draw(window);
					stripId++;
				}
				else
				{
					stripId = 0;
					rowId++;
					if (rowId > _rowLenght)
					{

					}
				}
			}
		}
	}

	Item*getItemByName(std::string name)
	{
		if (!_items->empty())
		{
			for (size_t i = 0; i < _items->size(); i++)
			{
				if (_items->at(i)->name == name) { return _items->at(i); break; }
			}
			return nullptr;
		}
		else
		{
			return nullptr;
		}
	}

	int getStripLenght()const { return _stripLenght; }

	int getRowLenght()const { return _rowLenght; }

	void setStripLenght(int stripLenght) { _stripLenght = stripLenght; }

	void setRowLenght(int rowLenght) { _rowLenght = rowLenght; }

	ItemContainer(int maxItemsCount, int stripLenght, int rowLenght)
	{
		this->_maxItemsCount = maxItemsCount;
		this->_stripLenght = stripLenght;
		this->_rowLenght = rowLenght;
	}
	ItemContainer(int maxItemsCount)
	{
		this->_maxItemsCount = maxItemsCount;
	}
	ItemContainer()
	{
		this->_maxItemsCount = -1;
	}
};
