#pragma once

#include "Object.h"

#define ENT_AI_NODE 0x72

#define DATA_ENT_ACTION 0x8



//basic entity to make objects with ai act in some way
//MUST have physical body to be properly used
class ai_node :public Object
{
	
protected:

public:
	//used to define size of the actionBody 
	float actionRectWidth = 0;

	//used to define size of the actionBody 
	float actionRectHeight= 0;

	////body that is used for calling for actions 
	//b2Body* actionBody;

	//gets type of the object for the relations
	static int Type() { return ENT_AI_NODE; }

	//use it only if you do not not what object class you may encounter
	//MUST RETURN THE SAME TYPE AS TYPE() METHOD
	virtual int getType()const { return Type(); }

	ai_node*next = NULL;
	std::string next_name = "";

	ai_node(sf::Vector2f position, std::string name = "" ,int area_id = 0) :Object(position,area_id)
	{
		this->name = name;
	}
	ai_node(sf::Vector2f position,std::string name="", float actionWidth = 0, float actionHeight = 0, float width = 0, float height = 0, int area_id = 0) :Object(position,width,height, area_id)
	{
		this->actionRectWidth = actionWidth;
		this->name = name;
		this->actionRectHeight = actionHeight;
	}

	virtual void onCollision(Object*&object, b2Fixture *fixtureA, b2Fixture *fixtureB, Context*&context, std::string stateName)
	{
		//prevent from slowdown caused by infinite collidings
		if (fixtureA->GetBody()->GetUserData() == this && fixtureB->GetBody()->GetUserData() == this)
		{
			return;
		}
		if (fixtureA->GetUserData()!= NULL)
		{
			if (static_cast<FixtureData*>(fixtureA->GetUserData())->getActionType()==FixtureActionType::AI_Node_Action)
			{
			}
			
		}
		if (fixtureB->GetUserData() != NULL)
		{
			if (static_cast<FixtureData*>(fixtureB->GetUserData())->getActionType() == FixtureActionType::AI_Node_Action)
			{
			}
			
		}	
	}

	virtual void leftCollision(Object*&object, b2Fixture *fixtureA, b2Fixture *fixtureB, Context*&context, std::string stateName)
	{
		//prevent from slowdown caused by infinite collidings
		if (fixtureA->GetBody()->GetUserData() == this&&fixtureB->GetBody()->GetUserData() == this)
		{
			return;
		}
	}
};