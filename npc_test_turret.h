#pragma once

#include "SceneActor.h"
class npc_test_turret :public SceneActor
{
public:
	size_t animIndex = 0;
	Animation::SpriteSheetAnimation* Anim;
	std::vector<Animation::Animation> * animations = new std::vector<Animation::Animation>();
	sf::FloatRect reactCollision;


	void Update(sf::Time dt)
	{

		if (!animations->empty())
		{
			Anim->Time += dt.asSeconds();
			if (Anim->Time >= Anim->FrameDuration)
			{
				Anim->SetFrame(animations->at(animIndex).FrameIndexes->at(animations->at(animIndex).CurrentFrameIndex).ColumnIndex, animations->at(animIndex).FrameIndexes->at(animations->at(animIndex).CurrentFrameIndex).StripIndex);
				Anim->Time = 0;
				animations->at(animIndex).CurrentFrameIndex++;
				if (animations->at(animIndex).CurrentFrameIndex > animations->at(animIndex).FrameIndexes->size() - 1)
				{
					animations->at(animIndex).CurrentFrameIndex = 0;
				}
			}
		}
	}

	virtual void onCollision(Object*&object, Context*&context, std::string stateName)
	{
		if (object->GetObjectRectangle().intersects(this->reactCollision))
		{
			sf::Vector2f diff;
			diff.x = object->GetObjectPosition().x - this->GetObjectPosition().x;
			diff.y = object->GetObjectPosition().y - this->GetObjectPosition().x;

			this->SetObjectRotation((atan2(diff.y, diff.x)*(180 / M_PI)));
		}
	}

	virtual void apllyDamage(float damage, Object*object, Context*&context, std::string stateName)
	{
		Animation::Animation blood_a = Animation::Animation("blood_a");
		blood_a.FrameIndexes->push_back(Animation::CellIndex(0, 0));
		blood_a.FrameIndexes->push_back(Animation::CellIndex(0, 1));
		blood_a.FrameIndexes->push_back(Animation::CellIndex(0, 2));
		blood_a.FrameIndexes->push_back(Animation::CellIndex(0, 3));
		/*blood_a.FrameIndexes->push_back(Animation::CellIndex(0, 4));
		blood_a.FrameIndexes->push_back(Animation::CellIndex(0, 5));*/


		Decal*blood = new Decal(sf::Vector2f(this->body->GetPosition().x, this->body->GetPosition().y), 0.05f, false, 20.0f, 512, 512, sf::Sprite(context->game->Resources->getTextureResourceDataByName("blood_a_anim")->texture), 100, 100);
		blood->Init();
		blood->animations->push_back(blood_a);
		blood->SetAnimation("blood_a");
		context->game->GetStateByName(stateName)->StateObjects->push_back(blood);
	}
	virtual void leftCollision(Object*&object, Context*&context, std::string stateName)
	{

	}
	npc_test_turret(sf::Vector2f position, sf::FloatRect activateRect, float Height, float Width, float FrameHeight, float FrameWidth, sf::Sprite sprite) :SceneActor(position, sprite, Width, Height)
	{
		Scale.x = Width / FrameWidth;
		Scale.y = Height / FrameHeight;
		this->reactCollision = activateRect;
		Anim = new Animation::SpriteSheetAnimation(0.1f, 1, sf::Vector2f(FrameWidth, FrameHeight), sprite);
		Anim->IsRepated = true;
	}

	bool SetAnimation(std::string name)
	{
		if (!animations->empty())
		{
			for (size_t i = 0; i < animations->size(); i++)
			{
				if (animations->at(i).name == name)
				{
					animIndex = i;
					return true;
				}
			}
		}
		return false;
	}
	virtual void Draw(sf::RenderWindow*& window)
	{
		window->draw(Anim->sprite);
	}

	void Move(sf::Vector2f vec)override
	{
		collision.left += vec.x;
		collision.top += vec.y;
		sprite.move(vec);
	}
	void SetObjectRotation(int angle)override
	{
		this->Anim->sprite.setRotation(angle);
		this->RotationAngle = angle;
	}
	//has to be called after creation
	void Init()override
	{
		this->Anim->sprite.setScale(this->Scale);
		this->Anim->sprite.setPosition(sf::Vector2f(collision.left, collision.top));
		this->Anim->sprite.setOrigin(sf::Vector2<float>(/*this->Anim->getTextureRect().width*Scale.x/2, this->Anim->getTextureRect().height*Scale.y/2*/0, 0));
		this->OnCollision = [this](Object*object, b2Fixture *fixtureA, b2Fixture *fixtureB)
		{
			if (object->GetObjectRectangle().intersects(this->reactCollision))
			{
				sf::Vector2f diff;
				diff.x = object->GetObjectPosition().x - this->GetObjectPosition().x;
				diff.y = object->GetObjectPosition().y - this->GetObjectPosition().x;


				this->SetObjectRotation((atan2(diff.y, diff.x)*(180 / M_PI)));
			}
		};
	}
};