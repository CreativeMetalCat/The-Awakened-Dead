#pragma once
#include "Object.h"

struct MovementDirection
{
public:
	float Angle = 0.0f;
	float Distance = 0.0f;
	MovementDirection(float Angle, float Distance) :Angle(Angle), Distance(Distance)
	{

	}
};

//class for hadling movement of npc's etc.
class MovingPawn :public Object
{
protected:
	float mTravelledDist = 0.0f;

	bool Done = false;
public:
	std::vector<MovementDirection>*Pattern = new std::vector<MovementDirection>();
	size_t dirIndex = 0;
	float Speed = 1.0f;

	//change it if you want to stop pawn or make it move
	//Warning: 
	//if pawn is already done movement(last MovementDirection is completed) 
	//and none is added, Done will be reseted to true,no matter what you set it to
	void SetMovementDone(bool b) { Done = b; }
	const bool GetIsMovementDone() { return Done; }

	MovingPawn(float Speed, sf::Vector2f position, float rectWidth, float rectHeight) :Object(position, rectWidth, rectHeight)
	{
		this->Speed = Speed;
	}
	void UpdateMovement(sf::Time dt)
	{
		if (Done != true)
		{
			if (!Pattern->empty())
			{
				float distanceToTravell = Pattern->at(dirIndex).Distance;
				if (distanceToTravell < mTravelledDist)
				{
					mTravelledDist = 0.0f;
					if (dirIndex + 1 < Pattern->size())
					{
						dirIndex += 1;
					}
					else
					{
						dirIndex = Pattern->size() - 1;
						Done = true;
					}

				}
				float rads = ((Pattern->at(dirIndex).Angle)*M_PI / 180);
				float vx = Speed * std::cos(rads);
				float vy = Speed * std::sin(rads);
				this->body->SetLinearVelocity(b2Vec2(vx, vy));
				this->SetObjectRotation(Pattern->at(dirIndex).Angle);
				mTravelledDist += Speed * 10.f * dt.asSeconds();
			}
		}

	}
	void AddMovement(MovementDirection md)
	{
		Pattern->push_back(md);
	}
};