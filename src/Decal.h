#pragma once

//Sprite that is dynamically spawn and destroyed(not restricted to that) after some time(unless LifeTime is set to -1)
//or if state has limit to them
//Doesn't have physical body or any type of collision by default
class Decal :public SceneActor
{
protected:
	//Not Recomened. May cause crash
	Decal() {}
	size_t animIndex;
	float mLifeTime = 0.f;
	float mLivedTime = 0.f;
	bool mShouldBeDead = false;
public:
	std::vector<Animation::Animation> * animations = new std::vector<Animation::Animation>();
	Animation::SpriteSheetAnimation* Anim;
	float getLifeTime()const { return mLifeTime; }

	bool IsDone()const { return mShouldBeDead; }

	void setLifeTime(float lt) { mLifeTime = lt; }

	//Time that is passed since creation
	//Can be used to check if object should be deleted
	float getLivedTime()const { return mLivedTime; }

	//resets time that is passed since creation
	void resetLivedTime() { mLivedTime = 0.f; }

	//If lifeTime==-1.f it will be infinite
	//animFrame sizes MUST not be scaled and taked from original image's size
	Decal(sf::Vector2f position, float animFrameDuration, bool isRepeated, float LifeTime, float animFrameWidth, float animFrameHeight, sf::Sprite sprite) :SceneActor(position, sprite), mLifeTime(LifeTime)
	{
		body = nullptr;
		Anim = new Animation::SpriteSheetAnimation(animFrameDuration, NULL, sf::Vector2f(animFrameWidth, animFrameHeight), sprite);
		Anim->IsRepated = isRepeated;
	}
	//If lifeTime = -1.f it will be infinite
	Decal(sf::Vector2f position, float LifeTime, sf::Sprite sprite) :SceneActor(position, sprite), mLifeTime(LifeTime)
	{
		body = nullptr;
		Anim = new Animation::SpriteSheetAnimation(0.f, NULL, sf::Vector2f(sprite.getTextureRect().width, sprite.getTextureRect().height), sprite);
		Anim->IsRepated = true;
	}
	//If lifeTime = -1.f it will be infinite
	Decal(sf::Vector2f position, float LifeTime, sf::Sprite sprite, float width, float height) :SceneActor(position, sprite, width, height), mLifeTime(LifeTime)
	{
		body = nullptr;
		Anim = new Animation::SpriteSheetAnimation(0.f, NULL, sf::Vector2f(width, height), sprite);
		Anim->IsRepated = true;
	}
	//If lifeTime = -1.f it will be infinite
	//animFrame sizes MUST not be scaled and taked from original image's size
	Decal(sf::Vector2f position, float animFrameDuration, bool isRepeated, float LifeTime, float animFrameWidth, float animFrameHeight, sf::Sprite sprite, float width, float height) :SceneActor(position, sprite, width, height), mLifeTime(LifeTime)
	{
		body = nullptr;
		Scale.x = width / animFrameWidth;
		Scale.y = height / animFrameHeight;

		Anim = new Animation::SpriteSheetAnimation(animFrameDuration, NULL, sf::Vector2f(animFrameWidth, animFrameHeight), sprite);
		Anim->IsRepated = isRepeated;
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

	void Draw(sf::RenderWindow* &window)override
	{
		window->draw(Anim->sprite);
	}

	void Init()override
	{
		sprite.setScale(Scale);
		sprite.setPosition(sf::Vector2f(this->collision.left, this->collision.top));
		Anim->sprite.setScale(Scale);
		Anim->sprite.setPosition(sf::Vector2f(this->collision.left, this->collision.top));
		if (!animations->empty())
		{
			Anim->SetFrame(animations->at(animIndex).FrameIndexes->at(animations->at(animIndex).CurrentFrameIndex).ColumnIndex, animations->at(animIndex).FrameIndexes->at(animations->at(animIndex).CurrentFrameIndex).StripIndex);
			Anim->Time = 0;
			animations->at(animIndex).CurrentFrameIndex++;
			if ((animations->at(animIndex).CurrentFrameIndex > animations->at(animIndex).FrameIndexes->size() - 1) && Anim->IsRepated)
			{
				animations->at(animIndex).CurrentFrameIndex = 0;
			}
			else
			{
				animations->at(animIndex).CurrentFrameIndex = animations->at(animIndex).FrameIndexes->size() - 1;
			}

		}
	}

	void Update(sf::Time dt) override
	{
		if (body != nullptr)
		{
			if (!_impulseApplied)
			{

				b2Vec2 imp = b2Vec2(_impulse.x*dt.asMilliseconds(), _impulse.y*dt.asMilliseconds());
				/*b2Vec2 p = body->GetWorldPoint(body->GetPosition());*/
				body->ApplyLinearImpulse(imp, body->GetPosition(), true);
				_impulseApplied = true;
			}
			if (!_velocitySet)
			{
				body->SetLinearVelocity(b2Vec2(_velocity));
				_velocitySet = true;
			}

			this->Anim->sprite.setPosition(sf::Vector2f(body->GetPosition().x, body->GetPosition().y));
		}

		if (mLifeTime != -1.f)
		{
			mLivedTime += dt.asSeconds();
			if (mLivedTime >= mLifeTime)
			{
				mShouldBeDead = true;
			}
		}
		if (!animations->empty())
		{
			Anim->Time += dt.asSeconds();
			if (Anim->Time >= Anim->FrameDuration)
			{
				Anim->SetFrame(animations->at(animIndex).FrameIndexes->at(animations->at(animIndex).CurrentFrameIndex).ColumnIndex, animations->at(animIndex).FrameIndexes->at(animations->at(animIndex).CurrentFrameIndex).StripIndex);
				Anim->Time = 0;
				animations->at(animIndex).CurrentFrameIndex++;
				if ((animations->at(animIndex).CurrentFrameIndex > animations->at(animIndex).FrameIndexes->size() - 1) && Anim->IsRepated)
				{
					animations->at(animIndex).CurrentFrameIndex = 0;
				}
			}
		}
	}

	/*void Init()override
	{

	}*/
};
