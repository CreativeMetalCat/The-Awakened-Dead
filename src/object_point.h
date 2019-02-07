#pragma once

#include "Object.h"

sf::Vector2f RotatePoint(sf::Vector2f point, float angle, sf::Vector2f origin)
{
	return sf::Vector2f
	(
		cos(angle)* (point.x - origin.x) + sin(angle) * (point.y - origin.y) + origin.x,
		-sin(angle) * (origin.x - point.x) + cos(angle)*(point.y - origin.y) + origin.y
	);
}


class PointObject :public Object
{
private:

	//Point doesn't have rectangle
	void SetObjectRectangleWidth(float width) override
	{
		collision.width = width;
	}

	//Point doesn't have rectangle
	void SetObjectRectangleHeight(float height)override
	{
		collision.height = height;
	}

	//Point doesn't have rectangle
	void SetObjectRectangle(sf::FloatRect rect)override { this->collision = rect; }

	//Point doesn't have rectangle
	const virtual sf::FloatRect GetObjectRectangle()
	{
		return collision;
	}

protected:

	
public:


	//Point - Location Of the Object 
	PointObject(sf::Vector2f point, int area_id = 0) :Object(point, area_id)
	{

	}

	//Point - Location Of the Object 
	PointObject(sf::Vector2f point, std::string name="", int area_id = 0) :Object(point, area_id)
	{
		this->name = name;
	}
	void Update(sf::Time dt)override{}
};