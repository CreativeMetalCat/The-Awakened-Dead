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

	Object* GetObjectByName(std::string name)
	{
		if (!StateObjects->empty())
		{
			for (size_t i = 0; i < StateObjects->size(); i++)
			{
				if (StateObjects->at(i)->name == name) { return StateObjects->at(i); }
			}
		}
		else
		{
			return NULL;
		}
	}

	std::string current_map = "";
	bool _map_is_loaded = false;

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

	//used to safely destroy physical objects
	//but can also be used for evry object that can be deleted only on update
	std::vector<Object*>*objectsToDestroy = new std::vector<Object*>();

	virtual void HandleEvent(sf::Event& event) = 0;
	virtual void Draw() = 0;
	virtual void Update(sf::Time dt) = 0;
	virtual void Init() = 0;

	//destroys all objects from objectsToDestroy array
	virtual void finishDestoy()
	{
		if (!objectsToDestroy->empty())
		{
			for (size_t i = 0; i < objectsToDestroy->size(); i++)
			{
				if (this->DeleteObjectFromState(objectsToDestroy->at(i)))
				{
					
				}
			}
			objectsToDestroy->clear();
		}
	}

	//basic function
	//needs better version
	bool operator==(State &state)
	{
		if (Name == state.Name) { return true; }
	}
	//states might handle this differently
	virtual void LoadMap(std::string name)
	{

	}
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

	//Deletes object from StateObjects that equals parametr 
	//IT WILL NOT DELETE OBJECT THAT YOU GIVE BUT RATHER IT'S COPY IN STATEOBJECTS
	//returns true on success
	virtual bool DeleteObjectFromState(Object*object)
	{
		auto it = std::find(StateObjects->begin(), StateObjects->end(), object);
		if (it != StateObjects->end())
		{
			auto index = std::distance(StateObjects->begin(), it);
			
			//delete it's body if it has one
			if (StateObjects->at(index)->physBodyInitialized)
			{
				//Make NULL step to avoid crashes
				StateObjects->at(index)->body->GetWorld()->Step(0, 0, 0);

				StateObjects->at(index)->body->SetActive(false);

				StateObjects->at(index)->body->GetWorld()->DestroyBody(StateObjects->at(index)->body);

				//just in case
				StateObjects->at(index)->physBodyInitialized = false;
			}
			StateObjects->erase(it);

			//object is deleted
			//return true
			return true;
		}
		{
			return false;
		}
	}


	//Plays sound using one of the channels
	virtual void PlaySound(std::string name)
	{
		
	}

	
	//Plays sound using one of the channels and sound from game->resourses->SoundData
	//@param
	//channel_id - id of channel that was used for this sound 
	virtual void PlaySound(std::string name, int &channel_id)
	{
		
	}
	//creates physical body for object using given data
	//Data HAS to be created before calling
	virtual void CreatePhysicsObject(Object*object, b2BodyDef bodyDef, b2FixtureDef fixture, bool addToStateObjectContainer, float mass = 1.f)
	{

		b2MassData propMass;
		propMass.mass = mass;
		propMass.center = b2Vec2(object->GetObjectRectangle().left + object->GetObjectRectangle().width / 2, object->GetObjectRectangle().top + object->GetObjectRectangle().height / 2);

		world.Step(0, 0, 0);

		object->Init();
		object->body = world.CreateBody(&bodyDef);

		object->body->SetMassData(&propMass);
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