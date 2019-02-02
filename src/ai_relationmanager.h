#pragma once
//

#include "Object.h"

//if there is going to be other version of this it will need to have define in the beging
#ifndef AI_RELATION_MANAGER
#define AI_RELATION_MANAGER

//basic version for future 
enum RelationType
{
	//ai manager was unable to find relationship in object's data
	ObjectIsMissing = -1,
	//will be nor attacked nor helped
	Noone = 0,
	Ally,
	Enemy
};

//basic data class that contains basic data about different classes
class ai_relationtype
{
public:

	int pawn_type = 0;

	RelationType relation_type;

	ai_relationtype(RelationType relation_type, int pawn_type = 0) :relation_type(relation_type), pawn_type(pawn_type)
	{

	}
};


class ai_relationmanager
{

protected:
	//goes unused because it should be managed by childer classes
	//for managing squad, etc.
	/*
	int maxAllies;

	int mixAllies = 0;

	int maxEnemies;

	int mixEnemies = 0;


	int amoutAllies = 0;

	int amountEnemies = 0;
	*/

public:

	std::vector<ai_relationtype>*relations = new std::vector<ai_relationtype>();

	/*int countAllies()const { return amoutAllies; }

	int countEnemies()const { return amountEnemies; }*/

	//it will override relationship if it will find object with the same type
	//or it will add it as new
	void addRelation(ai_relationtype relation)
	{
		if (!relations->empty())
		{
			for (size_t i = 0; i < relations->size(); i++)
			{
				if (relations->at(i).pawn_type == relation.pawn_type) { relations->at(i).relation_type = relation.relation_type; return; }
			}
			relations->push_back(relation);
		}
		else
		{
			relations->push_back(relation);
		}
	}

	//pawn type is int variable that must be defined for every Pawn Type
	RelationType getRelationWithPawnType(int pawntype)
	{
		if (!relations->empty())
		{
			for (size_t i = 0; i < relations->size(); i++)
			{
				if (relations->at(i).pawn_type == pawntype) { return relations->at(i).relation_type; }
			}
			return RelationType::ObjectIsMissing;
		}
		else
		{
			return RelationType::ObjectIsMissing;
		}
	}

	//02.02.2019 - no actual values are needed for this object's creation
	ai_relationmanager(){}

	~ai_relationmanager()
	{
		relations->~vector();
	}
};

#else

#endif // !AI_RELATION_MANAGER



