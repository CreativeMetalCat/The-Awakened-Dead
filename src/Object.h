#pragma once
#include "SFML/Graphics.hpp"
#include <Box2D.h>
#include <functional>

#define M_PI           3.14159265358979323846  /* pi */

//Base Class for EVERY object in scene. Drawable & not
class Object
{
private:

protected:
	//NOT a proper collision object made for setting sfml properties of object
	//e.g. 
	//sprites,scales,positions
	sf::FloatRect collision;

	//required to be used in update because box2d doesn't work sometimes
	bool _impulseApplied = true;

	//temp value
	b2Vec2 _impulse;

	//required to be used in update because box2d doesn't work sometimes
	bool _velocitySet = true;

	//temp value
	b2Vec2 _velocity;
public:
	//it's id that is usually assinged by layer on the map 
	//needed for the optimization
	int area_id = 0;

	//is set manually or by some function in children class
	//unnessesary if alternative already exists
	bool physBodyInitialized = false;

	//is set manually or by some function in children class
	//unnessesary if alternative already exists
	bool bodyIsSensor = false;

	//Way of checking if collision should be considered
	bool HasCollision = true;
	std::vector<Object*>*CollidingObjects = new std::vector< Object*>();
	int RotationAngle = 0;
	sf::Vector2f Scale;

	//physical body
	b2Body*body;

	virtual void applyImpulse(b2Vec2 impulse);

	virtual void applyImpulse(sf::Vector2f impulse)
	{
		_impulse = b2Vec2(impulse.x, impulse.y);
		_impulseApplied = false;
	}

	virtual void setVelocity(b2Vec2 vel)
	{
		_velocity = vel;
		_velocitySet = false;
	}

	virtual void setVelocity(sf::Vector2f vel)
	{
		_velocity = b2Vec2(vel.x, vel.y);
		_velocitySet = false;
	}

	std::function<void(Object*object,b2Fixture *fixtureA,b2Fixture *fixtureB)>OnCollision = [this](Object*object, b2Fixture *fixtureA, b2Fixture *fixtureB)
	{
		if (this->bodyIsSensor)
		{
			/*CollidingObjects->push_back(object);*/
		}
	};
	std::function<void(Object*object, b2Fixture *fixtureA, b2Fixture *fixtureB)>LeftCollision = [this](Object*object, b2Fixture *fixtureA, b2Fixture *fixtureB)
	{
		if (!CollidingObjects->empty())
		{
			CollidingObjects->erase(std::find(CollidingObjects->begin(), CollidingObjects->end(), object));
		}

	};

	virtual void Update(sf::Time dt)
	{
		if (physBodyInitialized)
		{
			this->collision.left = body->GetPosition().x;
			this->collision.top = body->GetPosition().y;
		}
	}
	const sf::FloatRect GetObjectRectangle()
	{
		return collision;
	}
	int Layer = 0;
	bool Visible = true;
	Object*Parent = NULL;
	std::vector<Object*>*Children = new std::vector<Object*>();

	void AttachChild(Object*&object)
	{
		object->Parent = this;
		Children->push_back(object);
	}

	virtual void Move(sf::Vector2f vec)
	{

	}

	Object* DetachChild(int id)
	{
		if (!Children->empty() && (id > 0 && id < Children->size()))
		{
			Object*res;
			Children->at(id)->Parent = NULL;
			auto i = std::find(Children->begin(), Children->end(), this->Children->at(id));
			res = Children->at(id);
			res->Parent = nullptr;
			Children->erase(i);
			return res;
		}
		return nullptr;
	}

	Object* DetachChild(Object*&object)
	{
		if (!Children->empty())
		{
			size_t i = 0;
			for (i = 0; i < this->Children->size(); i++)
			{
				if (object == Children->at(i))
				{
					break;
				}
			}
			auto it = std::find(Children->begin(), Children->end(), object);
			Object*res;
			res = Children->at(i);
			Children->erase(it);
			return res;
		}
		return nullptr;
	}

	Object(sf::Vector2f position, int area_id = 0):area_id(area_id)
	{
		this->collision.left = position.x;
		this->collision.top = position.y;
	}
	Object(sf::Vector2f position, float width, float height, int area_id = 0):area_id(area_id)
	{
		this->collision.left = position.x;
		this->collision.top = position.y;
		this->collision.width = width;
		this->collision.height = height;
	}
	Object() {}

	virtual sf::Vector2f GetObjectPosition()
	{
		sf::Vector2f res;
		res.x = collision.left;
		res.y = collision.top;
		return res;
	}
	virtual void SetObjectPosition(sf::Vector2f pos)
	{
		collision.left = pos.x;
		collision.top = pos.y;


	}


	virtual void Draw(sf::RenderWindow*&window) {};

	virtual void Draw(sf::RenderWindow&window) {};

	//only stores angle right now
	virtual void SetObjectRotation(int angle)
	{
		RotationAngle = angle;

		if (!this->Children->empty())
		{
			for (size_t i = 0; i < Children->size(); i++)
			{
				Children->at(i)->SetObjectRotation(angle);
			}
		}
	}


	//Doesn't do anything right now
	virtual void RotateObject(int angle)
	{
		if (!this->Children->empty())
		{
			for (size_t i = 0; i < Children->size(); i++)
			{
				Children->at(i)->RotateObject(angle);
			}
		}
	}

	void SetObjectRectangleWidth(float width)
	{
		collision.width = width;
	}

	void SetObjectRectangleHeight(float height)
	{
		collision.height = height;
	}

	void SetObjectRectangle(sf::FloatRect rect) { this->collision = rect; }

	//if you need to locate sprites, text etc.
	virtual void Init() {}

};