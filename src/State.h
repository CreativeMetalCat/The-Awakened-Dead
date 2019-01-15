#pragma once

#include "SFML/Graphics.hpp"
#include <fmod.hpp>
#include <fmod_errors.h>

class Game;

//Way of passing resources between states
struct Context
{
public:
	sf::RenderWindow *window;
	Game*game;
	FMOD::System**lowSoundSystem;
	Context()
	{

	}
};


//Base class for states. Hadles drawing, events and object updates. Similar to idea of gamemods in unreal engine 4
//When creating own states - for base functions you MUST use override
class State
{
protected:
	//to keep track of state type
	int ID = 0;
public:
	bool worldIsPaused = false;

	b2World world;
	std::string Name = "";
	bool IsActive = false;
	Context* context;
	State(std::string Name) :world(b2Vec2(0.f, 0.f)) { this->Name = Name; }
	//Every state has it's own set of objects
	//e.g.
	//Play state has world objects
	//Menu\pause-Menu buttons, logos, etc.
	std::vector<Object*>*StateObjects = new std::vector<Object*>();
	virtual void HandleEvent(sf::Event& event) = 0;
	virtual void Draw() = 0;
	virtual void Update(sf::Time dt) = 0;
	virtual void Init() = 0;

	//creates physical body for object using given data
	//Data HAS to be created before calling
	virtual void CreatePhysicsObject(Object*object, b2BodyDef bodyDef, std::vector<b2FixtureDef>*&fixtures, bool addToStateObjectContainer, float mass = 1.f)
	{

		object->Init();
		object->body = world.CreateBody(&bodyDef);

		if (!fixtures->empty())
		{
			for (size_t i = 0; i < fixtures->size(); i++)
			{
				object->body->CreateFixture(&fixtures->at(i));
			}
		}
		object->body->SetUserData(object);

		if (addToStateObjectContainer)
		{
			StateObjects->push_back(object);
		}

	}

	virtual void PlaySound(std::string name)
	{

	}
	//creates physical body for object using given data
	//Data HAS to be created before calling
	virtual void CreatePhysicsObject(Object*object, b2BodyDef bodyDef, b2FixtureDef fixture, bool addToStateObjectContainer, float mass = 1.f)
	{

		object->Init();
		object->body = world.CreateBody(&bodyDef);

		object->body->CreateFixture(&fixture);

		object->body->SetUserData(object);

		if (addToStateObjectContainer)
		{
			StateObjects->push_back(object);
		}

	}

	//creates physical body for object using given data
	//Data HAS to be created before calling
	virtual void CreatePhysicsObject(Object*&object, b2BodyDef bodyDef, std::vector<b2FixtureDef>*&fixtures, bool addToStateObjectContainer, float mass = 1.f)
	{

		object->Init();
		object->body = world.CreateBody(&bodyDef);

		if (!fixtures->empty())
		{
			for (size_t i = 0; i < fixtures->size(); i++)
			{
				object->body->CreateFixture(&fixtures->at(i));
			}
		}
		object->body->SetUserData(object);

		if (addToStateObjectContainer)
		{
			StateObjects->push_back(object);
		}

	}

	//creates physical body for object using given data
	//Data HAS to be created before calling
	virtual void CreatePhysicsObject(Object*&object, b2BodyDef bodyDef, b2FixtureDef fixture, bool addToStateObjectContainer, float mass = 1.f)
	{

		object->Init();
		object->body = world.CreateBody(&bodyDef);

		object->body->CreateFixture(&fixture);

		object->body->SetUserData(object);

		if (addToStateObjectContainer)
		{
			StateObjects->push_back(object);
		}

	}
};