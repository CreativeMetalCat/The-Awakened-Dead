#pragma once
#include "includes.h"

//Base Class for EVERY object in scene. Drawable & not
class SceneObject
{
protected:
	//NOT a proper collision object made for setting sfml properties of object
	//e.g. 
	//sprites,scales,positions
	sf::FloatRect collision;
public:
	//Way of checking if collision should be considered
	bool HasCollision = true;
	std::vector<SceneObject*>*CollidingObjects = new std::vector< SceneObject*>();
	int RotationAngle = 0;
	sf::Vector2f Scale;

	//physical body
	b2Body*body;

	std::function<void(SceneObject*object)>OnCollision = [this](SceneObject*object)
	{
		CollidingObjects->push_back(object);
	};
	std::function<void(SceneObject*object)>LeftCollision = [this](SceneObject*object)
	{
		if (!CollidingObjects->empty())
		{
			CollidingObjects->erase(std::find(CollidingObjects->begin(), CollidingObjects->end(), object));
		}

	};

	virtual void Update(sf::Time dt) {}
	const sf::FloatRect GetSceneObjectRectangle()
	{
		return collision;
	}
	int Layer = 0;
	bool Visible = true;
	SceneObject*Parent = NULL;
	std::vector<SceneObject*>*Children = new std::vector<SceneObject*>();

	void AttachChild(SceneObject*&object)
	{
		object->Parent = this;
		Children->push_back(object);
	}

	virtual void Move(sf::Vector2f vec)
	{

	}
	SceneObject* DetachChild(int id)
	{
		if (!Children->empty() && (id > 0 && id < Children->size()))
		{
			SceneObject*res;
			Children->at(id)->Parent = NULL;
			auto i = std::find(Children->begin(), Children->end(), this->Children->at(id));
			res = Children->at(id);
			res->Parent = nullptr;
			Children->erase(i);
			return res;
		}
		return nullptr;
	}

	SceneObject* DetachChild(SceneObject*&object)
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
			SceneObject*res;
			res = Children->at(i);
			Children->erase(it);
			return res;
		}
		return nullptr;
	}

	SceneObject(sf::Vector2f position)
	{
		this->collision.left = position.x;
		this->collision.top = position.y;
	}
	SceneObject(sf::Vector2f position, float width, float height)
	{
		this->collision.left = position.x;
		this->collision.top = position.y;
		this->collision.width = width;
		this->collision.height = height;
	}
	SceneObject() {}

	virtual sf::Vector2f GetSceneObjectPosition()
	{
		sf::Vector2f res;
		res.x = collision.left;
		res.y = collision.top;
		return res;
	}
	virtual void SetSceneObjectPosition(sf::Vector2f pos)
	{
		collision.left = pos.x;
		collision.top = pos.y;


	}


	virtual void Draw(sf::RenderWindow*&window) {};

	virtual void Draw(sf::RenderWindow&window) {};

	//only stores angle right now
	virtual void SetSceneObjectRotation(int angle)
	{
		RotationAngle = angle;

		if (!this->Children->empty())
		{
			for (size_t i = 0; i < Children->size(); i++)
			{
				Children->at(i)->SetSceneObjectRotation(angle);
			}
		}
	}


	//Doesn't do anything right now
	virtual void RotateSceneObject(int angle)
	{
		if (!this->Children->empty())
		{
			for (size_t i = 0; i < Children->size(); i++)
			{
				Children->at(i)->RotateSceneObject(angle);
			}
		}
	}

	void SetSceneObjectRectangleWidth(float width)
	{
		collision.width = width;
	}

	void SetSceneObjectRectangleHeight(float height)
	{
		collision.height = height;
	}

	void SetSceneObjectRectangle(sf::FloatRect rect) { this->collision = rect; }

	//if you need to locate sprites, text etc.
	virtual void Init() {}

};

//Base class for ALL drawable objects in scene that use sprites.
class SceneActor :public SceneObject
{
protected:

public:

	sf::Sprite sprite;
	const sf::Sprite GetSprite() { return sprite; }
	void SetSprite(sf::Sprite) { this->sprite = sprite; }


	SceneActor(sf::Vector2f position, sf::Sprite sprite) :SceneObject(position)
	{
		this->sprite = sprite;
		sprite.setPosition(position);

	}

	SceneActor(sf::Vector2f position, sf::Sprite sprite, float width, float height) :SceneObject(position, width, height)
	{
		this->sprite = sprite;
		sprite.setPosition(position);

	}
	SceneActor(sf::Sprite sprite) :SceneObject()
	{
		this->sprite = sprite;
	}
	SceneActor() {}

	void Draw(sf::RenderWindow&window)
	{
		window.draw(sprite);
	}

	void Draw(sf::RenderWindow*&window)
	{
		window->draw(sprite);
	}

	void Init()override
	{
		sf::Vector2f scale;
		scale.x = collision.width / sprite.getTextureRect().width;
		scale.y = collision.height / sprite.getTextureRect().height;
		sprite.setScale(scale);
		sprite.setPosition(sf::Vector2f(this->collision.left, this->collision.top));
		/*sprite.setOrigin(sf::Vector2f(sprite.getTextureRect().width*scale.x / 2, sprite.getTextureRect().height*scale.y / 2));*/

	}

};


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

		Anim = new Animation::SpriteSheetAnimation(animFrameDuration, NULL, sf::Vector2f(animFrameWidth, animFrameHeight), sprite);
		Anim->IsRepated = isRepeated;
	}
	//If lifeTime = -1.f it will be infinite
	Decal(sf::Vector2f position, float LifeTime, sf::Sprite sprite) :SceneActor(position, sprite), mLifeTime(LifeTime)
	{
		Anim = new Animation::SpriteSheetAnimation(0.f, NULL, sf::Vector2f(sprite.getTextureRect().width, sprite.getTextureRect().height), sprite);
		Anim->IsRepated = true;
	}
	//If lifeTime = -1.f it will be infinite
	Decal(sf::Vector2f position, float LifeTime, sf::Sprite sprite, float width, float height) :SceneActor(position, sprite, width, height), mLifeTime(LifeTime)
	{
		Anim = new Animation::SpriteSheetAnimation(0.f, NULL, sf::Vector2f(width, height), sprite);
		Anim->IsRepated = true;
	}
	//If lifeTime = -1.f it will be infinite
	//animFrame sizes MUST not be scaled and taked from original image's size
	Decal(sf::Vector2f position, float animFrameDuration, bool isRepeated, float LifeTime, float animFrameWidth, float animFrameHeight, sf::Sprite sprite, float width, float height) :SceneActor(position, sprite, width, height), mLifeTime(LifeTime)
	{
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

		}
	}

	void Update(sf::Time dt) override
	{
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