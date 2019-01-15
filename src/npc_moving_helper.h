#pragma once

#include "MovingPawn.h"

class npc_moving_helper :public MovingPawn
{
protected:
	std::string text;
	sf::Color color;
	size_t animIndex = 0;

public:
	std::vector<Animation::Animation> * animations = new std::vector<Animation::Animation>();
	Animation::SpriteSheetAnimation* Anim;
	bool DisplayText = false;
	sf::Text help_text;
	npc_moving_helper(float MovementSpeed, std::string text, sf::Color color, sf::Font&font, sf::Vector2f position, float rectWidth, float rectHeight, float FrameHeight, float FrameWidth, sf::Sprite sprite) :MovingPawn(MovementSpeed, position, rectWidth, rectHeight)
	{
		this->collision.width = rectWidth;
		this->collision.height = rectHeight;

		this->color = color;
		help_text.setColor(color);

		this->text = text;
		help_text.setPosition(position);
		help_text.setString(text);

		Scale.x = rectWidth / FrameWidth;
		Scale.y = rectHeight / FrameHeight;

		Anim = new Animation::SpriteSheetAnimation(0.1f, 1, sf::Vector2f(FrameWidth, FrameHeight), sprite);
		Anim->IsRepated = true;
	}
	sf::Vector2f GetObjectPosition() override
	{
		return Anim->sprite.getPosition();
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

	/*void Update(sf::Time dt)
	{
		this->UpdateMovement(dt);
		if (!animations->empty())
		{
			Anim->Time += dt.asSeconds();
			if (Anim->Time >= Anim->FrameDuration)
			{
				Anim->SetFrame(animations->at(animIndex).FrameIndexes->at(animations->at(animIndex).CurrentFrameIndex).ColumnIndex, animations->at(animIndex).FrameIndexes->at(animations->at(animIndex).CurrentFrameIndex).StripIndex);
				Anim->Time = 0;
				animations->at(animIndex).CurrentFrameIndex++;
				if (animations->at(animIndex).CurrentFrameIndex > animations->at(animIndex).FrameIndexes->size()-1)
				{
					if (animIndex + 1 < animations->size())
					{
						animIndex++;
					}
					else
					{
						animations->at(animIndex).CurrentFrameIndex = 0;
					}
				}
			}
		}
	}*/

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
		this->UpdateMovement(dt);
		this->Anim->sprite.setPosition(sf::Vector2f(body->GetPosition().x, body->GetPosition().y));
	}

	void Move(sf::Vector2f vec)override
	{
		collision.left += vec.x;
		collision.top += vec.y;
		help_text.move(vec);
		Anim->sprite.move(vec);
	}

	virtual void Draw(sf::RenderWindow*& window)
	{
		window->draw(Anim->sprite);
		if (DisplayText)
		{
			window->draw(help_text);
		}

	}
	//has to be called after creation
	void Init()override
	{
		this->Anim->sprite.setScale(this->Scale);
		this->Anim->sprite.setPosition(sf::Vector2f(collision.left, collision.top));
		this->help_text.setPosition(sf::Vector2f(collision.left, collision.top));
		this->help_text.setString(text);


	}
};