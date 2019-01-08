#define COMPILE

#ifdef COMPILE



#pragma region weapons

#define WEAPON_TYPE_NONE 0x0

#define WEAPON_TYPE_TAD_PISTOL 0xA

#define WEAPON_TYPE_TAD_RIFLE 0xb

#define WEAPON_TYPE_TAD_SHOTGUN 0Xc

#define WEAPON_TYPE_TAD_KNIFE 0Xd

#define WEAPON_TYPE_TAD_FLASHLIGHT 0Xe

#define WEAPON_TYPE_TAD_GRENADE 0xf

#pragma endregion


#pragma region Resource Types

#define RESOURCE_NULL 0x0

#define RESOURCE_FONT 0x65

#define RESOURCE_TEXTURE 0x66

#define RESOURCE_SHADER 0x67

#define RESOURCE_SOUND 0x68

#pragma endregion

#define MAX_SOUND_CHANNELS_COUNT 1024


#define M_PI           3.14159265358979323846  /* pi */

#include "GUI.h"

#include "SFML/Graphics.hpp"
#include <iostream>
#include <vector>
#include <functional>
#include <math.h>
#include <algorithm>

#include <Box2D.h>
#include <fmod.hpp>
#include <fmod_errors.h>

#include <tinyxml2.h>
#include <tinyxml2.cpp>

#ifndef XMLCheckResult
#define XMLCheckResult(a_eResult) if (a_eResult != XML_SUCCESS) { printf("Error: %i\n", a_eResult);/* return a_eResult;*/ }
#endif

const int SCREENWIDTH = 1080;
const int SCREENHEIGHT = 720;

bool DEBUG_DRAWCOLLISION = true;

////sf::Font font;
//sf::Texture solder;
//sf::Texture textBoxTexture1;
//sf::Texture proj;
////sf::Texture Officer1;
//sf::Texture blood1;
//sf::Texture blood_yellow_1;
//sf::Texture blood_a_anim;
//sf::Texture solder_rifle;



class Material
{
public:
	std::vector<sf::Shader>*shaders = new std::vector<sf::Shader>();
	sf::Texture texture;
	Material(sf::Texture texture)
	{
		this->texture = texture;
	}
	Material(sf::Texture texture, std::vector<sf::Shader>*&shaders)
	{
		this->texture = texture;
		this->shaders = shaders;
		if (!shaders->empty())
		{
			for (size_t i = 0; i < shaders->size(); i++)
			{
				shaders->at(i).setParameter("texture", texture);
			}
		}
	}
};

//Particle system for one pixel sized particles
class PixelParticleSystem : public sf::Drawable, public sf::Transformable
{

public:
	sf::Texture* texture;
	sf::Color color;
	struct Particle
	{
	public:
		sf::Vector2f velocity;
		sf::Time lifetime;

	};

	PixelParticleSystem(unsigned int count, sf::Color color) :m_particles(count), color(color), m_vertices(sf::Points, count), m_lifetime(sf::seconds(0.1f)), m_emitter(0.f, 0.f)
	{

	}

	void setEmitter(sf::Vector2f position)
	{
		m_emitter = position;
	}

	void Stop()
	{
		m_particles.clear();
		m_particles.resize(0);
	}
	//elapsed = delta time(dt in most cases)
	void update(sf::Time elapsed)
	{
		for (std::size_t i = 0; i < m_particles.size(); ++i)
		{
			// update the particle lifetime
			Particle& p = m_particles[i];
			p.lifetime -= elapsed;

			// if the particle is dead, respawn it
			if (p.lifetime <= sf::Time::Zero)
				resetParticle(i);

			// update the position of the corresponding vertex
			m_vertices[i].position += p.velocity * elapsed.asSeconds();

			// update the alpha (transparency) of the particle according to its lifetime
			float ratio = p.lifetime.asSeconds() / m_lifetime.asSeconds();
			m_vertices[i].color = color;
			m_vertices[i].color.a = static_cast<sf::Uint8>(ratio * 255);
		}
	}

protected:
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
	{

		// apply the transform
		states.transform *= getTransform();

		// our particles don't use a texture
		states.texture = NULL/*texture*/;

		// draw the vertex array
		target.draw(m_vertices, states);
	}
	void resetParticle(std::size_t index)
	{
		// give a random velocity and lifetime to the particle
		float angle = (std::rand() % 360) * 3.14f / 180.f;
		float speed = (std::rand() % 50) + 5.f;
		m_particles[index].velocity = sf::Vector2f(std::cos(angle) * speed, std::sin(angle) * speed);
		m_particles[index].lifetime = sf::milliseconds((std::rand() % 2000) + 1000);

		// reset the position of the corresponding vertex
		m_vertices[index].position = m_emitter;
	}


	std::vector<Particle> m_particles;
	sf::VertexArray m_vertices;
	sf::Time m_lifetime;
	sf::Vector2f m_emitter;
};


class ParticleSystem : public sf::Drawable, public sf::Transformable
{

public:
	sf::Texture* texture;
	struct Particle
	{
	public:
		sf::Vector2f velocity;
		sf::Time lifetime;
		sf::Color color;
	};

	ParticleSystem(unsigned int count) :m_particles(count), m_vertices(sf::Points, count), m_lifetime(sf::seconds(3.f)), m_emitter(0.f, 0.f)
	{

	}

	void setEmitter(sf::Vector2f position)
	{
		m_emitter = position;
	}

	//elapsed = delta time(dt in most cases)
	void update(sf::Time elapsed)
	{
		sf::Vector2f size(texture->getSize());
		sf::Vector2f half = size / 2.f;
		m_vertices.clear();

		sf::Color c;
		sf::Vector2f pos;
		for (std::size_t i = 0; i < m_particles.size(); ++i)
		{
			pos = m_particles[i].velocity + m_emitter;
			c = sf::Color::Red/*m_particles[i].color*/;

			// update the particle lifetime
			Particle& p = m_particles[i];
			p.lifetime -= elapsed;

			// if the particle is dead, respawn it
			if (p.lifetime <= sf::Time::Zero)
			{
				resetParticle(i);
			}



			// update the alpha (transparency) of the particle according to its lifetime
			float ratio = p.lifetime.asSeconds() / m_lifetime.asSeconds();
			c.a = static_cast<sf::Uint8>(255 * std::max(ratio, 0.f));

			m_vertices.append(sf::Vertex(sf::Vector2f(pos.x - half.x, pos.y - half.y), c, sf::Vector2f(0.f, 0.f)));
			m_vertices.append(sf::Vertex(sf::Vector2f(pos.x + half.x, pos.y - half.y), c, sf::Vector2f(size.x, 0.f)));
			m_vertices.append(sf::Vertex(sf::Vector2f(pos.x + half.x, pos.y + half.y), c, sf::Vector2f(size.x, size.y)));
			m_vertices.append(sf::Vertex(sf::Vector2f(pos.x - half.x, pos.y + half.y), c, sf::Vector2f(0.f, size.y)));

			//// update the position of the corresponding vertex
			//m_vertices[i].position += p.velocity * elapsed.asSeconds();



		}
	}

protected:
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
	{

		// apply the transform
		states.transform *= getTransform();


		states.texture = NULL/*texture*/;

		// draw the vertex array
		target.draw(m_vertices, states);
	}
	void resetParticle(std::size_t index)
	{
		// give a random velocity and lifetime to the particle
		float angle = (std::rand() % 360) * 3.14f / 180.f;
		float speed = (std::rand() % 50) + 50.f;
		m_particles[index].velocity = sf::Vector2f(std::cos(angle) * speed, std::sin(angle) * speed);
		m_particles[index].lifetime = sf::milliseconds((std::rand() % 2000) + 1000);

		//// reset the position of the corresponding vertex
		//m_vertices[index].position = m_emitter;

	}


	std::vector<Particle> m_particles;
	sf::VertexArray m_vertices;
	sf::Time m_lifetime;
	sf::Vector2f m_emitter;
};
//Alternative to thor's animation
namespace Animation
{
	//Animation made of one sprite and rectangle that choses current frame of animation
	class SpriteSheetAnimation
	{
	protected:
		
		//current textureRect
		sf::IntRect Frame;
		//size of frame height(y) and width(x)
		sf::Vector2f FrameSize;
	public:
		const sf::IntRect getTextureRect() { return Frame; }
		
		//time since last frame update. if your object doesn't keep track of it
		float Time = 0.0f;
		//the animation itself
		sf::Sprite sprite;
		float FrameDuration;
		//if set to false stops on last frame of animation
		bool IsRepated = false;
		SpriteSheetAnimation(float FrameDuration, int FrameCount, sf::Vector2f FrameSize, sf::Texture&texture) :sprite(texture)
		{
			this->FrameDuration = FrameDuration;
			this->FrameSize = FrameSize;
			this->Frame.width = FrameSize.x;
			this->Frame.height = FrameSize.y;
		}
		SpriteSheetAnimation(float FrameDuration, int FrameCount, sf::Vector2f FrameSize, sf::Sprite&sprite) :sprite(sprite)
		{
			this->FrameDuration = FrameDuration;
			this->FrameSize = FrameSize;
			this->Frame.width = FrameSize.x;
			this->Frame.height = FrameSize.y;
		}
		enum StripMoveDir
		{
			Left,
			Right
		};
		enum ColumnMoveDir
		{
			Up,
			Down
		};

		void StripMoveFrame(StripMoveDir dir = StripMoveDir::Right)
		{

			if (dir == StripMoveDir::Right)
			{
				Frame.left += FrameSize.x;
				if (Frame.left > sprite.getTexture()->getSize().x)
				{
					if (IsRepated) { Frame.left = 0; }
					else { Frame.left = sprite.getTexture()->getSize().x; }
				}
			}
			if (dir == StripMoveDir::Left)
			{
				Frame.left -= FrameSize.x;
				if (Frame.left < 0)
				{
					if (IsRepated) { Frame.left = sprite.getTexture()->getSize().x; }
					else { Frame.left = 0; }
				}
			}

			sprite.setTextureRect(Frame);
		}
		void ColumnMoveFrame(ColumnMoveDir dir = ColumnMoveDir::Down)
		{
			if (dir == ColumnMoveDir::Up)
			{
				Frame.height -= FrameSize.y;
				if (Frame.height < 0)
				{
					if (IsRepated) { Frame.height = sprite.getTexture()->getSize().y; }
					else { Frame.height = 0; }
				}
			}
			if (dir == ColumnMoveDir::Down)
			{
				Frame.height -= FrameSize.y;
				if (Frame.height > sprite.getTexture()->getSize().y)
				{
					if (IsRepated) { Frame.height = 0; }
					else { Frame.height = sprite.getTexture()->getSize().y; }
				}
			}

			sprite.setTextureRect(Frame);
		}

		void SetFrame(size_t Columnindex, size_t StripIndex)
		{
			if (FrameSize.y*Columnindex < sprite.getTexture()->getSize().y)
			{
				Frame.top = FrameSize.y*Columnindex;
			}
			if (FrameSize.x*StripIndex < sprite.getTexture()->getSize().x)
			{
				Frame.left = FrameSize.x*StripIndex;
			}
			sprite.setTextureRect(Frame);
		}
		//Automaticly updates animation
		//dt = delta time. Use one from State's Update(sf::Time dt)
		void UpdateAnimation(sf::Time dt)
		{
			this->Time += dt.asSeconds();
			if (this->Time >= this->FrameDuration)
			{
				this->StripMoveFrame(StripMoveDir::Right);
				this->Time = 0;
			}
		}
	};

	struct CellIndex
	{
	public:
		size_t ColumnIndex = 0;
		size_t StripIndex = 0;
		CellIndex(size_t ColumnIndex, size_t StripIndex)
		{
			this->ColumnIndex = ColumnIndex;
			this->StripIndex = StripIndex;
		}
	};

	struct Animation
	{
	public:
		size_t CurrentFrameIndex = 0;
		std::vector<CellIndex>*FrameIndexes = new std::vector<CellIndex>();
		std::string name;
		Animation(std::string name)
		{
			this->name = name;
		}
	};

	enum FrameChangeDirection
	{
		//to call PreviousFrame in SpritesAnimation
		Left,
		//to call NextFrame in SpritesAnimation
		Right
	};

	//Animation that is made of multiple sprites that are created and loaded separatly
	class SpritesAnimation
	{
	protected:
		//index of current frame
		int FrameIndex = 0;
	public:
		//name of the animation
		//Neccessary for SpritesAnimationContainer
		std::string Name;
		FrameChangeDirection frameChangeDir = FrameChangeDirection::Left;
		//time since last frame update. if your object doesn't keep track of it
		float Time = 0.0f;
		//if set to false stops on last frame of animation
		bool IsRepeated = false;
		//All frames
		std::vector<sf::Sprite>*Sprites = new std::vector<sf::Sprite>();
		//Current Frame
		sf::Sprite CurrentSprite;
		//time per frame
		float FrameDuration;
		



		SpritesAnimation(bool IsRepeated,float frameDuration, std::string Name, FrameChangeDirection frameChangeDir = FrameChangeDirection::Left) :IsRepeated(IsRepeated),FrameDuration(frameDuration)
		{
			this->frameChangeDir = frameChangeDir;
			this->Name = Name;
		}

		SpritesAnimation(bool IsRepeated, float frameDuration, std::string Name, std::vector<sf::Sprite>*&Sprites, FrameChangeDirection frameChangeDir = FrameChangeDirection::Left) :IsRepeated(IsRepeated), Sprites(Sprites), FrameDuration(frameDuration)
		{
			this->frameChangeDir = frameChangeDir;
			this->Name = Name;
		}

		void NextFrame()
		{
			if (Sprites->empty()) { return; }
			FrameIndex++;
			if (FrameIndex > Sprites->size() - 1)
			{
				if (IsRepeated)
				{
					FrameIndex = 0;
				}
				else
				{
					FrameIndex = Sprites->size() - 1;			
				}
				CurrentSprite = Sprites->at(FrameIndex);
			}
			else
			{
				CurrentSprite = Sprites->at(FrameIndex);
			}
		}

		void PreviousFrame()
		{
			if (Sprites->empty()) { return; }
			FrameIndex--;
			if (FrameIndex < 0)
			{
				if (IsRepeated)
				{
					FrameIndex = Sprites->size() - 1;
				}
				else
				{
					FrameIndex = 0;
					
				}
				CurrentSprite = Sprites->at(FrameIndex);
			}
			else
			{
				CurrentSprite = Sprites->at(FrameIndex);
			}
		}

		void SetFrame(size_t i)
		{
			if (Sprites->empty()) { return; }
			if (i < Sprites->size())
			{
				CurrentSprite = Sprites->at(i);
				FrameIndex = i;
			}
		}

		//updates animation.
		//if it's time to update moves to set direction
		void UpdateAnimation(sf::Time dt)
		{
			this->Time += dt.asSeconds();
			if (this->Time >= this->FrameDuration)
			{
				if (frameChangeDir == FrameChangeDirection::Right)
				{
					NextFrame();
				}
				if (frameChangeDir == FrameChangeDirection::Left)
				{
					PreviousFrame();
				}
				this->Time = 0;
			}
		}
		void AddFrame(sf::Sprite frame)
		{
			Sprites->push_back(frame);
		}
		//replaces existing one if index is in range of container
		//or
		//adds in the end in other cases
		void AddFrame(sf::Sprite frame, size_t index)
		{
			if (index < Sprites->size())
			{
				Sprites->at(index) = frame;
			}
			else
			{
				Sprites->push_back(frame);
			}
		}

		~SpritesAnimation()
		{
			Sprites->~vector();
			Name.~basic_string();
		}
	};

	class SpritesAnimationsContainer
	{
	public:
		std::vector<SpritesAnimation*>*animations = new std::vector<SpritesAnimation*>();

		size_t AnimIndex = 0;
		void addAnimation(SpritesAnimation*&anim)
		{
			animations->push_back(anim);
		}
		SpritesAnimation*getAnimationByName(std::string name)
		{
			if (name != "" )
			{
				throw(std::runtime_error("Name is empty"));
			}
			if (!animations->empty())
			{
				for (size_t i = 0; i < animations->size(); i++)
				{
					if (animations->at(i)->Name == name) { return animations->at(i); }
				}
				throw(std::runtime_error("No animations with given name: "+ name));
			}
			else
			{
				throw(std::runtime_error("No animations avalable"));
			}
		}
	};
}


class ObjectContactListener;

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

	virtual void applyImpulse(b2Vec2 impulse)
	{
		_impulse = impulse;
		_impulseApplied = false;
	}

	virtual void applyImpulse(sf::Vector2f impulse)
	{
		_impulse = b2Vec2(impulse.x,impulse.y);
		_impulseApplied = false;
	}

	virtual void setVelocity(b2Vec2 vel)
	{
		_velocity = vel;
		_velocitySet = false;
	}

	virtual void setVelocity(sf::Vector2f vel)
	{
		_velocity = b2Vec2(vel.x,vel.y);
		_velocitySet = false;
	}

	std::function<void(Object*object)>OnCollision = [this](Object*object) 
	{
		if (this->bodyIsSensor)
		{
			CollidingObjects->push_back(object);
		}
	};
	std::function<void(Object*object)>LeftCollision = [this](Object*object)
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

	Object(sf::Vector2f position)
	{
		this->collision.left = position.x;
		this->collision.top = position.y;
	}
	Object(sf::Vector2f position, float width, float height)
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

//Base class for ALL drawable objects in scene that use sprites.
class SceneActor :public Object
{
protected:
	
public:

	sf::Sprite sprite;
	const sf::Sprite GetSprite() { return sprite; }
	void SetSprite(sf::Sprite) { this->sprite = sprite; }


	SceneActor(sf::Vector2f position, sf::Sprite sprite) :Object(position)
	{
		this->sprite = sprite;
		sprite.setPosition(position);

	}

	SceneActor(sf::Vector2f position, sf::Sprite sprite, float width, float height) :Object(position, width, height)
	{
		this->sprite = sprite;
		sprite.setPosition(position);

	}
	SceneActor(sf::Sprite sprite) :Object()
	{
		this->sprite = sprite;
	}
	SceneActor() {}

	void Draw(sf::RenderWindow*&window)
	{
		window->draw(sprite);
	}
	void Draw(sf::RenderWindow&window)
	{
		window.draw(sprite);
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

class SceneTile :public SceneActor
{
public:
	SceneTile(sf::Vector2f position, sf::Sprite sprite) :SceneActor(position, sprite)
	{
		this->sprite = sprite;
		sprite.setPosition(position);

	}

	SceneTile(sf::Vector2f position, sf::Sprite sprite, float width, float height) :SceneActor(position,sprite, width, height)
	{
		this->sprite = sprite;
		sprite.setPosition(position);

	}
	SceneTile(sf::Sprite sprite) :SceneActor(sprite)
	{
		this->sprite = sprite;
	}
	SceneTile() {}
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
	Decal(sf::Vector2f position,float animFrameDuration,bool isRepeated, float LifeTime,float animFrameWidth,float animFrameHeight, sf::Sprite sprite):SceneActor(position,sprite),mLifeTime(LifeTime)
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
	Decal(sf::Vector2f position, float LifeTime, sf::Sprite sprite, float width, float height) :SceneActor(position, sprite,width,height), mLifeTime(LifeTime)
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
				body->ApplyLinearImpulse(imp,body->GetPosition(),true);
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
				if ((animations->at(animIndex).CurrentFrameIndex > animations->at(animIndex).FrameIndexes->size() - 1)&&Anim->IsRepated)
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



class ObjectContactListener:public b2ContactListener
{
private:

protected:

public:
	void BeginContact(b2Contact*contact)
	{
		void* bodyUserDataA = contact->GetFixtureA()->GetBody()->GetUserData();
		void* bodyUserDataB = contact->GetFixtureB()->GetBody()->GetUserData();
		if (bodyUserDataA&&bodyUserDataB)
		{
			static_cast<Object*>(bodyUserDataA)->OnCollision(static_cast<Object*>(bodyUserDataB));
			static_cast<Object*>(bodyUserDataB)->OnCollision(static_cast<Object*>(bodyUserDataA));
		}
	}
	void EndContact(b2Contact*contact)
	{
		void* bodyUserDataA = contact->GetFixtureA()->GetBody()->GetUserData();
		void* bodyUserDataB = contact->GetFixtureB()->GetBody()->GetUserData();
		if (bodyUserDataA&&bodyUserDataB)
		{
			static_cast<Object*>(bodyUserDataA)->LeftCollision(static_cast<Object*>(bodyUserDataB));
			static_cast<Object*>(bodyUserDataB)->LeftCollision(static_cast<Object*>(bodyUserDataA));
		}
	}
};

//static solid objects like walls
//Every other static object will be created without proper collision
class SolidObject :public SceneActor
{
public:
	SolidObject(sf::Vector2f position, sf::Sprite sprite) :SceneActor(position,sprite)
	{
		this->sprite = sprite;
		sprite.setPosition(position);

	}
	SolidObject(sf::Vector2f position, sf::Sprite sprite, float width, float height) : SceneActor(position,sprite, width, height)
	{
		this->sprite = sprite;
		sprite.setPosition(position);

	}
	SolidObject(sf::Sprite sprite) :SceneActor(sprite)
	{
		this->sprite = sprite;
	}
	
};

//Particle system for one pixel sized particles
class PixelParticleSystemObject : public sf::Drawable, public sf::Transformable
{

public:
	sf::Texture* texture;
	sf::Color color;
	Object*emitterObject;
	//Time before system stops
	//-1 for infinite time
	float MaxTime = 0.0f;
	struct Particle
	{
	public:
		sf::Vector2f velocity;
		sf::Time lifetime;

	};

	//MaxTime:
	//Time before system stops
	//-1 for infinite time
	PixelParticleSystemObject(unsigned int count, float MaxTime, Object*&emitterObject, sf::Color color) :m_particles(count), color(color), MaxTime(MaxTime), emitterObject(emitterObject), m_vertices(sf::Points, count), m_lifetime(sf::seconds(0.1f)), m_emitter(0.f, 0.f)
	{

	}

	void setEmitter(sf::Vector2f position)
	{
		m_emitter = position;
	}

	void Stop()
	{
		m_particles.clear();
		m_particles.resize(0);
	}
	//elapsed = delta time(dt in most cases)
	void update(sf::Time elapsed)
	{
		if (MaxTime > 0)
		{
			MaxTime -= elapsed.asSeconds();
		}

		m_emitter = emitterObject->GetObjectPosition();
		if (MaxTime > 0)
		{
			for (std::size_t i = 0; i < m_particles.size(); ++i)
			{
				// update the particle lifetime
				Particle& p = m_particles[i];
				p.lifetime -= elapsed;

				// if the particle is dead, respawn it
				if (p.lifetime <= sf::Time::Zero)
					resetParticle(i);

				// update the position of the corresponding vertex
				m_vertices[i].position += p.velocity * elapsed.asSeconds();

				// update the alpha (transparency) of the particle according to its lifetime
				float ratio = p.lifetime.asSeconds() / m_lifetime.asSeconds();
				m_vertices[i].color = color;
				m_vertices[i].color.a = static_cast<sf::Uint8>(ratio * 255);
			}
		}
		else
		{
			this->Stop();
		}
	}

protected:
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
	{

		// apply the transform
		states.transform *= getTransform();

		// our particles don't use a texture
		states.texture = NULL/*texture*/;

		// draw the vertex array
		target.draw(m_vertices, states);
	}
	void resetParticle(std::size_t index)
	{
		// give a random velocity and lifetime to the particle
		float angle = (std::rand() % 360) * 3.14f / 180.f;
		float speed = (std::rand() % 50) + 5.f;
		m_particles[index].velocity = sf::Vector2f(std::cos(angle) * speed, std::sin(angle) * speed);
		m_particles[index].lifetime = sf::milliseconds((std::rand() % 2000) + 1000);

		// reset the position of the corresponding vertex
		m_vertices[index].position = m_emitter;
	}


	std::vector<Particle> m_particles;
	sf::VertexArray m_vertices;
	sf::Time m_lifetime;
	sf::Vector2f m_emitter;
};

sf::IntRect ToIntRect(sf::FloatRect rect)
{
	sf::IntRect res;
	res.height = rect.height;
	res.width = rect.width;
	res.top = rect.top;
	res.left = rect.left;
	return res;
}







class State;
class npc_zombie;
class Player;


class Game;


class Resource
{
protected:
	std::string fileExtension;
public:
	int Type = RESOURCE_NULL;
	std::string name;
	std::string filename;
	
	Resource(std::string name) { this->name = name; }

	Resource(std::string name, std::string filename, int Type = RESOURCE_NULL) { this->name = name; this->filename = filename; this->fileExtension = fileExtension; this->Type = Type; }
	//fileExt - file extension
	virtual void CreateResourceFromFile() {};
};


class FontResource :public Resource
{
public:
	sf::Font font;
	FontResource(std::string name, std::string filename,int Type = RESOURCE_FONT) :Resource(name, filename,Type)
	{

	}
	FontResource(std::string name, std::string filename, sf::Font& font, int Type = RESOURCE_FONT) :Resource(name, filename, Type),font(font)
	{
		
	}

	void CreateResourceFromFile()override
	{
		font.loadFromFile(filename);
	}

	~FontResource()
	{
		this->font.~Font();
	}
};
class TextureResource :public Resource
{
private:
	bool textureIsRepeated;
	bool textureIsSmooth;
public:
	sf::Texture texture;
	TextureResource(std::string name, std::string filename,bool textureIsRepeated,bool textureIsSmooth,int Type = RESOURCE_TEXTURE) :Resource(name, filename, Type)
	{
		this->textureIsRepeated = textureIsRepeated;
		this->textureIsSmooth = textureIsSmooth;
	}
	TextureResource(std::string name, std::string filename, sf::Texture& texture, bool textureIsRepeated, bool textureIsSmooth, int Type = RESOURCE_TEXTURE) :Resource(name, filename, Type),texture(texture)
	{
		this->textureIsRepeated = textureIsRepeated;
		this->textureIsSmooth = textureIsSmooth;
	}
	void CreateResourceFromFile()override
	{
		texture.loadFromFile(filename);
		texture.setRepeated(textureIsRepeated);
		texture.setSmooth(textureIsSmooth);
	}
	~TextureResource()
	{
		texture.~Texture();
		this->name.~basic_string();
	}
};
class ShaderResource :public Resource
{
public:
	sf::Shader shader;
	ShaderResource(std::string name, std::string fileExtension, int Type = RESOURCE_SHADER) :Resource(name, fileExtension, Type)
	{

	}
	ShaderResource(std::string name, std::string fileExtension, sf::Shader&shader, int Type = RESOURCE_SHADER) :Resource(name, fileExtension, Type)
	{

	}
	void CreateResourceFromFile()override
	{

	}
};


class ResourceContainer
{
private:
	//gets firts resource with this name of that type
	Resource* getResourceDataByName(std::string name,int Type)
	{
		//name MUST be longer then 0
		if (name == "") { throw(std::runtime_error("Texture name is null (\"\")")); }

		if (ResourceData->empty())
		{
			throw(std::runtime_error("No resources avalable"));
		}
		else
		{
			for (size_t i = 0; i < ResourceData->size(); i++)
			{
				if (ResourceData->at(i)->name == name && ResourceData->at(i)->Type == Type) { return ResourceData->at(i); }
			}
			throw(std::runtime_error("Unable to find resorce"));
		}
	}
protected:
	//all data for vector specific operations
	std::vector<Resource*>*ResourceData = new std::vector<Resource*>();
public:
	std::vector<TextureResource*>*TextureData = new std::vector<TextureResource*>();
	std::vector<FontResource*>*FontData = new std::vector<FontResource*>();

	void addAnimationData(Resource* textureResource) { ResourceData->push_back(textureResource); }

	//gets firts resource with this name
	TextureResource* getTextureResourceDataByName(std::string name)
	{
		//name MUST be longer then 0
		if (name == "") { throw(std::runtime_error("name is null (\"\")")); }

		if (TextureData->empty())
		{
			throw(std::runtime_error("No resources avalable"));
		}
		else
		{
			for (size_t i = 0; i < TextureData->size(); i++)
			{
				if (TextureData->at(i)->name == name) { return TextureData->at(i); }
			}
			throw(std::runtime_error("Unable to find resorce"));
		}
	}

	
	//gets firts resource with this name of that type
	FontResource* getFontResourceDataByName(std::string name)
	{
		//name MUST be longer then 0
		if (name == "") { throw(std::runtime_error("name is null (\"\")")); }

		if (FontData->empty())
		{
			throw(std::runtime_error("No resources avalable"));
		}
		else
		{
			for (size_t i = 0; i < FontData->size(); i++)
			{
				if (FontData->at(i)->name == name) { return FontData->at(i); }
			}
			throw(std::runtime_error("Unable to find resorce"));
		}
	}

	void AddTextureResource(TextureResource*r)
	{
		TextureData->push_back(r);
	}
	void AddFontResource(FontResource*r)
	{
		FontData->push_back(r);
	}
	void InitResources()
	{
		if (!TextureData->empty())
		{
			for (size_t i = 0; i < TextureData->size(); i++)
			{
				TextureData->at(i)->CreateResourceFromFile();
				std::cout << "Texture resource created. name: " + TextureData->at(i)->name + "       filename: " + TextureData->at(i)->filename << std::endl;
			}
		}
		if (!FontData->empty())
		{
			for (size_t i = 0; i < FontData->size(); i++)
			{
				FontData->at(i)->CreateResourceFromFile();
				std::cout << "Font resource created. name: " + FontData->at(i)->name + "       filename: " + FontData->at(i)->filename << std::endl;
			}
		}

	}


};
#ifdef ALT_0_0

class TextureResourceDataContainer
{
public:
	//all data for vector specific operations
	std::vector<TextureResource>*TextureResourceData = new std::vector<TextureResource>();

	void addAnimationData(TextureResource textureResource) { TextureResourceData->push_back(textureResource); }

	TextureResource getAnimationDataByName(std::string name, std::string ext)
	{
		//name MUST be longer then 0
		if (name == "") { throw(std::runtime_error("Texture name is null (\"\")")); }

		if (TextureResourceData->empty())
		{
			//returns null animation with no name
			return TextureResource("", "");
		}
		else
		{
			for (size_t i = 0; i < TextureResourceData->size(); i++)
			{
				if (TextureResourceData->at(i).name == name) { return TextureResourceData->at(i); }
			}
		}
	}
};

class FontResourceDataContainer
{
public:
	//all data for vector specific operations
	std::vector<FontResource>*FontResourceData = new std::vector<FontResource>();

	void addAnimationData(FontResource fontResource) { FontResourceData->push_back(fontResource); }

	FontResource getAnimationDataByName(std::string name, std::string ext)
	{
		//name MUST be longer then 0
		if (name == "") { throw(std::runtime_error("Font name is null (\"\")")); }

		if (FontResourceData->empty())
		{
			//returns null animation with no name
			return FontResource("", "");
		}
		else
		{
			for (size_t i = 0; i < FontResourceData->size(); i++)
			{
				if (FontResourceData->at(i).name == name) { return FontResourceData->at(i); }
			}
		}
	}
};
#endif // ALT


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

//@description
//physical object that can be pushed around 
//@e.g.
//chair, table,box
//@note: altrought every object based on Object class can be used in similar way it's better to have class specificly for that
//because it's simplier to update one class and cheking errors there then changing base class and wathcing all project crash
class PropPhysics :public SceneActor
{
protected:

public:
	PropPhysics(sf::Vector2f position, sf::Sprite sprite, float width, float height) :SceneActor(position, sprite, width, height)
	{
		this->sprite = sprite;
		sprite.setPosition(position);

	}
	PropPhysics(sf::Sprite sprite) :SceneActor(sprite)
	{
		this->sprite = sprite;
	}
	PropPhysics() {}

	void Init()override
	{
		sf::Vector2f scale;
		scale.x = collision.width / sprite.getTextureRect().width;
		scale.y = collision.height / sprite.getTextureRect().height;
		sprite.setScale(scale);
		sprite.setPosition(sf::Vector2f(this->collision.left, this->collision.top));
		sprite.setOrigin(sprite.getTextureRect().width / 2, sprite.getTextureRect().height / 2);
	}

	//function is better than lamba expression in this case
	virtual void onCollision(Object*&object, Context*&context, std::string stateName)
	{
		if (bodyIsSensor)
		{
			CollidingObjects->push_back(object);
		}
		b2Vec2 objectImpulse = b2Vec2(object->body->GetLinearVelocity().x*object->body->GetMass(), object->body->GetLinearVelocity().y*object->body->GetMass());
		b2Vec2 thisImpulse = b2Vec2(this->body->GetLinearVelocity().x*this->body->GetMass(), this->body->GetLinearVelocity().y*this->body->GetMass());
		b2Vec2 impulse = objectImpulse + thisImpulse;

		this->applyImpulse(impulse);
	}
	void Update(sf::Time dt) override
	{
		if (body != nullptr && physBodyInitialized)
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

			this->collision.left = body->GetPosition().x;
			this->collision.top = body->GetPosition().y ;

			sprite.setPosition(sf::Vector2f(body->GetPosition().x/* + this->sprite.getTextureRect().width / 2*/, body->GetPosition().y /*+ this->sprite.getTextureRect().height / 2*/));
			sprite.setRotation(body->GetAngle());
		}
	}
	//function is better than lamba expression in this case
	virtual void leftCollision(Object*&object, Context*&context, std::string stateName)
	{

		if (!this->CollidingObjects->empty())
		{
			if (std::find(this->CollidingObjects->begin(), this->CollidingObjects->end(), object) != this->CollidingObjects->end())
			{
				this->CollidingObjects->erase(std::find(this->CollidingObjects->begin(), this->CollidingObjects->end(), object));
			}
		}
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
};


class AnimationDataContainer
{
public:
	//all data for vector specific operations
	std::vector<Animation::Animation>*animationsData = new std::vector<Animation::Animation>();

	void addAnimationData(Animation::Animation anim) { animationsData->push_back(anim); }

	Animation::Animation getAnimationDataByName(std::string name)
	{
		//name MUST be longer then 0
		if (name == "") { throw(std::runtime_error("Animation name is null (\"\")")); }

		if (animationsData->empty())
		{		
			//returns null animation with no name
			return Animation::Animation("");
		}
		else
		{
			for (size_t i = 0; i < animationsData->size(); i++)
			{
				if (animationsData->at(i).name == name) {return animationsData->at(i);}
			}
		}
	}
};
class Game
{
protected:
	void ProccessEvents()
	{
		Context context = {};
		context.window = &window;
		context.game = this;
		context.lowSoundSystem = &lowSoundSystem;
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (!States->empty())
			{
				for (size_t i = 0; i < States->size(); i++)
				{
					if (States->at(i)->IsActive)
					{
						States->at(i)->context = &context;
						States->at(i)->HandleEvent(event);
					}
				}
			}
			/*CurrentState->HandleEvent(event);*/
		}

	}

	void Render()
	{

		Context context = {};
		context.window = &window;
		context.game = this;
		context.lowSoundSystem = &lowSoundSystem;

		if (!States->empty())
		{
			window.clear(sf::Color::Black);
			for (size_t i = 0; i < States->size(); i++)
			{
				if (States->at(i)->IsActive)
				{
					States->at(i)->context = &context;
					States->at(i)->Draw();
				}
			}
			window.display();
		}
	}

	//dt = deltaTime
	void Update(sf::Time dt)
	{
		window.clear(sf::Color::Black);
		Context context = {};
		context.window = &window;
		context.lowSoundSystem = &lowSoundSystem;
		context.game = this;
		if (!States->empty())
		{
			for (size_t i = 0; i < States->size(); i++)
			{
				if (States->at(i)->IsActive)
				{
					States->at(i)->context = &context;
					States->at(i)->Update(dt);
				}
			}
		}
	}
	std::vector<State*>*States = new std::vector<State*>();


public:
	


	FMOD::System*lowSoundSystem = NULL;
	sf::RenderWindow window;
	State*CurrentState;
	AnimationDataContainer*animationsData = new AnimationDataContainer();
	ResourceContainer*Resources = new ResourceContainer();

	Game(std::string WindowName, sf::VideoMode videoMode) :window(videoMode, WindowName)
	{
		/*window.setFramerateLimit(200);*/
		FMOD_RESULT res;
		res = FMOD::System_Create(&lowSoundSystem);
		if (res != FMOD_RESULT::FMOD_OK)
		{

		}
		res = lowSoundSystem->init(MAX_SOUND_CHANNELS_COUNT, FMOD_INIT_NORMAL, NULL);
		if (res != FMOD_RESULT::FMOD_OK)
		{

		}
	}

	//has to be called BEFORE Run()
	void Init()
	{

		std::cout << "Initialasing resourses..." << std::endl;
		Resources->InitResources();
		std::cout << "Initialased resourses" << std::endl;
		Context context = {};
		context.window = &window;
		context.game = this;
		context.lowSoundSystem = &lowSoundSystem;

		if (!States->empty())
		{
			for (size_t i = 0; i < States->size(); i++)
			{
				States->at(i)->context = &context;
				States->at(i)->Init();
			}
		}
	}
	void Run()
	{
		sf::Clock clock;


		while (window.isOpen())
		{

			window.clear(sf::Color::Black);
			sf::Time dt = clock.restart();
			ProccessEvents();
			Update(dt);
			window.clear(sf::Color::Black);
			Render();
		}
	}



	void AddState(State*state)
	{
		if (States->empty())
		{
			States->push_back(state);
		}
		else
		{
			for (size_t i = 0; i < States->size(); i++)
			{
				if (States->at(i) == state)
				{
					return;
				}
			}
			States->push_back(state);
		}
	}

	void SetActiveStates(size_t ids[])
	{
		if (States->size() < sizeof(ids)) { return; }
		for (size_t i = 0; i < sizeof(ids); i++)
		{
			States->at(ids[i])->IsActive = true;
		}
	}
	void SetActiveState(size_t id)
	{
		if (States->size() < id) { return; }

		States->at(id)->IsActive = true;
	}

	void DisableStates(size_t ids[])
	{
		if (States->size() < sizeof(ids)) { return; }
		for (size_t i = 0; i < sizeof(ids); i++)
		{
			States->at(ids[i])->IsActive = false;
		}
	}

	void DisableState(size_t id)
	{
		if (States->size() < id) { return; }

		States->at(id)->IsActive = false;
	}

	void DisableState(State*state)
	{
		if (!States->empty())
		{
			for (size_t i = 0; i < States->size(); i++)
			{
				if (States->at(i) == state)
				{
					States->at(i)->IsActive = false;
				}
			}
		}

	}
	void ActivateState(State*state)
	{
		if (!States->empty())
		{
			for (size_t i = 0; i < States->size(); i++)
			{
				if (States->at(i) == state)
				{
					States->at(i)->IsActive = true;
				}
			}
		}

	}

	//@note
	//you can NOT use that to get state's world parametr
	//due to the way yhe c++ work you will get a copy of the world parametr wich will not affect the original
	State*& GetStateByName(std::string name)
	{
		if (!States->empty())
		{
			for (size_t i = 0; i < States->size(); i++)
			{
				if (States->at(i)->Name == name)
				{
					return States->at(i);
				}
			}
		}
		throw(std::range_error("No states with given name: " + name + " were found"));
	}
};

struct MovementDirection
{
public:
	float Angle = 0.0f;
	float Distance = 0.0f;
	MovementDirection(float Angle, float Distance) :Angle(Angle), Distance(Distance)
	{

	}
};
class npc_helper :public SceneActor
{
protected:
	std::string text;
	sf::Color color;
public:
	Animation::SpriteSheetAnimation* Anim;
	bool DisplayText = false;
	sf::Text help_text;

	npc_helper(std::string text, sf::Color color, sf::Font&font, sf::Vector2f position, float rectWidth, float rectHeight, float FrameHeight, float FrameWidth, sf::Sprite sprite) :SceneActor(position, sprite), help_text(text, font)
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

	void Move(sf::Vector2f vec)override
	{
		collision.left += vec.x;
		collision.top += vec.y;
		help_text.move(vec);
		sprite.move(vec);
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
		this->Anim->sprite.setOrigin(sf::Vector2<float>(/*this->Anim->getTextureRect().width*Scale.x/2, this->Anim->getTextureRect().height*Scale.y/2*/0,0));
		this->OnCollision = [this](Object*object)
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

//class npc_zombie :public MovingPawn
//{
//protected:
//	size_t animIndex = 0;
//public:
//	std::vector<Animation::Animation> * animations = new std::vector<Animation::Animation>();
//	Animation::SpriteSheetAnimation* Anim;
//
//	npc_zombie(float MovementSpeed, sf::Vector2f position, float rectWidth, float rectHeight, float FrameHeight, float FrameWidth, sf::Sprite sprite) :MovingPawn(MovementSpeed, position, rectWidth, rectHeight)
//	{
//		this->collision.width = rectWidth;
//		this->collision.height = rectHeight;
//
//		Scale.x = rectWidth / FrameWidth;
//		Scale.y = rectHeight / FrameHeight;
//
//		Anim = new Animation::SpriteSheetAnimation(0.1f, 1, sf::Vector2f(FrameWidth, FrameHeight), sprite);
//		Anim->IsRepated = true;
//	}
//
//	sf::Vector2f GetObjectPosition() override
//	{
//		return Anim->sprite.getPosition();
//	}
//
//	void Init()override
//	{
//		this->Anim->sprite.setScale(this->Scale);
//		this->Anim->sprite.setPosition(sf::Vector2f(collision.left, collision.top));
//
//		this->OnCollision = [this](Object*object)
//		{
//			sf::Vector2f diff;
//			diff.x = object->body->GetPosition().x - this->GetObjectPosition().x;
//			diff.y = object->body->GetPosition().y - this->GetObjectPosition().y;
//
//			float distance = sqrt((pow(static_cast<double>(diff.x), 2), pow(static_cast<double>(diff.y), 2)));
//			this->Pattern->clear();
//			this->AddMovement(MovementDirection(static_cast<float>(atan2(diff.y, diff.x)*(180 / M_PI)), distance));
//			this->SetMovementDone(false);
//		};
//	}
//	virtual void Draw(sf::RenderWindow*& window)
//	{
//		window->draw(Anim->sprite);
//	}
//	void Move(sf::Vector2f vec)override
//	{
//		collision.left += vec.x;
//		collision.top += vec.y;
//		Anim->sprite.move(vec);
//	}
//
//	bool SetAnimation(std::string name)
//	{
//		if (!animations->empty())
//		{
//			for (size_t i = 0; i < animations->size(); i++)
//			{
//				if (animations->at(i).name == name)
//				{
//					animIndex = i;
//					return true;
//				}
//			}
//		}
//		return false;
//	}
//	void Update(sf::Time dt)
//	{
//
//		if (!animations->empty())
//		{
//			Anim->Time += dt.asSeconds();
//			if (Anim->Time >= Anim->FrameDuration)
//			{
//				Anim->SetFrame(animations->at(animIndex).FrameIndexes->at(animations->at(animIndex).CurrentFrameIndex).ColumnIndex, animations->at(animIndex).FrameIndexes->at(animations->at(animIndex).CurrentFrameIndex).StripIndex);
//				Anim->Time = 0;
//				animations->at(animIndex).CurrentFrameIndex++;
//				if (animations->at(animIndex).CurrentFrameIndex > animations->at(animIndex).FrameIndexes->size() - 1)
//				{
//					animations->at(animIndex).CurrentFrameIndex = 0;
//				}
//			}
//		}
//		this->UpdateMovement(dt);
//		this->Anim->sprite.setPosition(sf::Vector2f(body->GetPosition().x, body->GetPosition().y));
//	}
//};


class projectile :public SceneActor
{

protected:
	sf::Vector2<float>difference;
	float travelledDistance = 0.f;
	bool IsDone = false;
	//called in Launch(...)
	bool IsInitialized = false;

	b2Vec2 Velocity;

public:
	sf::Vector2<float>Origin;
	float MaxDistance = 0.0f;
	float Speed = 10.0f;
	sf::Vector2<float>Destination;

	projectile(sf::Vector2f position, float Height, float Width, float maxDistance, float Speed, sf::Sprite sprite) :SceneActor(position, sprite), MaxDistance(maxDistance), Speed(Speed)
	{
		collision.width = Width;
		collision.height = Height;
	}
	void Move(sf::Vector2f vec)override
	{
		collision.left += vec.x;
		collision.top += vec.y;
		sprite.move(vec);
	}

	virtual void projectileOnCollision(Object*&object, Context*&context, std::string stateName)
	{

		if (npc_test_turret* ntt = dynamic_cast<npc_test_turret*>(object))
		{
			ntt->apllyDamage(1.f, this, context, stateName);
		}
		Animation::Animation blood_a = Animation::Animation("blood_a");
		blood_a.FrameIndexes->push_back(Animation::CellIndex(0, 0));
		blood_a.FrameIndexes->push_back(Animation::CellIndex(0, 1));
		blood_a.FrameIndexes->push_back(Animation::CellIndex(0, 2));
		blood_a.FrameIndexes->push_back(Animation::CellIndex(0, 3));
		blood_a.FrameIndexes->push_back(Animation::CellIndex(0, 4));
		blood_a.FrameIndexes->push_back(Animation::CellIndex(0, 5));
		blood_a.FrameIndexes->push_back(Animation::CellIndex(0, 6));
		blood_a.FrameIndexes->push_back(Animation::CellIndex(0, 7));

		if (dynamic_cast<projectile*>(object)) { return; }
		/*if (dynamic_cast<Player*>(object)) { return; }*/
		this->CollidingObjects->push_back(object);


		sf::Vector2f diff;

		//Use location where bullet hit rather then hitted object's location(looks more realistic)
		diff.x = this->body->GetPosition().x - this->Origin.x;
		diff.y = this->body->GetPosition().y - this->Origin.y;


		float a = static_cast<float>((atan2(diff.x, diff.y)*(180 / M_PI)));
		Decal*blood;
		/*if (dynamic_cast<npc_zombie*>(object))
		{


			blood = new Decal(sf::Vector2f(this->body->GetPosition().x, this->body->GetPosition().y), 0.05f, true, 0.3f, 512, 512, sf::Sprite(context->game->Resources->getTextureResourceDataByName("blood_a_anim")->texture), 100, 100);
			blood->Anim->sprite.setRotation(-a - 270.f);
			blood->Init();
			blood->animations->push_back(blood_a);
			blood->SetAnimation("blood_a");
			context->game->GetStateByName(stateName)->StateObjects->push_back(blood);
		}
		else
		{


			blood = new Decal(sf::Vector2f(this->body->GetPosition().x, this->body->GetPosition().y), 0.05f, true, 0.3f, 512, 512, sf::Sprite(context->game->Resources->getTextureResourceDataByName("blood_a_anim")->texture), 100, 100);
			blood->Anim->sprite.setRotation(-a - 270.f);
			blood->Init();
			blood->animations->push_back(blood_a);
			blood->SetAnimation("blood_a");
			context->game->GetStateByName(stateName)->StateObjects->push_back(blood);
		}*/
	}

	virtual void projectileOnLeftCollision(Object*&object, Context*&context, std::string stateName)
	{

		if (!this->CollidingObjects->empty())
		{
			if (std::find(this->CollidingObjects->begin(), this->CollidingObjects->end(), object) != this->CollidingObjects->end())
			{
				this->CollidingObjects->erase(std::find(this->CollidingObjects->begin(), this->CollidingObjects->end(), object));
			}
		}
	}

	sf::Vector2f GetObjectPosition()override
	{
		if (IsInitialized)
		{
			return sf::Vector2f(body->GetPosition().x, body->GetPosition().y);
		}
		else
		{
			return { 0.f,0.f };
		}
	}
	void Launch(sf::Vector2<float> destination, sf::Vector2<float> origin, b2World&world, b2Filter filter)
	{
		IsDone = false;
		this->SetObjectPosition(origin);

		Origin = origin;
		difference = destination - origin;
		if (IsInitialized) { world.DestroyBody(this->body); }

		b2BodyDef def;
		def.position.Set(origin.x + this->GetObjectRectangle().width / 2, origin.y + this->GetObjectRectangle().height / 2);
		def.type = b2BodyType::b2_dynamicBody;
		this->body = world.CreateBody(&def);

		b2PolygonShape shape;
		shape.SetAsBox(this->GetObjectRectangle().width / 2, this->GetObjectRectangle().height / 2);

		b2FixtureDef TriggerFixture;
		TriggerFixture.filter = filter;
		TriggerFixture.density = 0.f;
		TriggerFixture.shape = &shape;
		TriggerFixture.isSensor = 1;

		body->CreateFixture(&TriggerFixture);
		body->SetUserData(this);



		/*sf::Vector2f diff;
		diff.x = event.mouseMove.x - player->GetObjectPosition().x;
		diff.y = event.mouseMove.y - player->GetObjectPosition().y;*/


		//degs
		float angle = (atan2(difference.y, difference.x));
		float rads = (angle*(180 / M_PI));
		float vx = Speed * std::cos(rads);
		float vy = Speed * std::sin(rads);
		std::cout << rads << std::endl;
		body->SetLinearVelocity((b2Vec2(vx * 10, vy * 10)));
		Velocity = (b2Vec2(vx * 10, vy * 10));
		IsInitialized = true;
	}

	//angle is in rads
	void Launch(float angle, sf::Vector2<float>origin, b2World&world, b2Filter filter)
	{
		Origin = origin;
		IsDone = false;
		this->SetObjectPosition(origin);
		this->Init();

		if (IsInitialized) { world.DestroyBody(this->body); }
		b2BodyDef def;
		def.position.Set(origin.x + this->GetObjectRectangle().width / 2, origin.y + this->GetObjectRectangle().height / 2);
		def.type = b2BodyType::b2_dynamicBody;
		this->body = world.CreateBody(&def);
		def.bullet = true;

		b2PolygonShape shape;
		shape.SetAsBox(this->GetObjectRectangle().width / 2, this->GetObjectRectangle().height / 2);

		b2FixtureDef TriggerFixture;
		TriggerFixture.filter = filter;
		TriggerFixture.density = 0.f;
		TriggerFixture.shape = &shape;
		TriggerFixture.isSensor = 1;

		body->CreateFixture(&TriggerFixture);
		body->SetUserData(this);



		/*sf::Vector2f diff;
		diff.x = event.mouseMove.x - player->GetObjectPosition().x;
		diff.y = event.mouseMove.y - player->GetObjectPosition().y;*/





		float vx = Speed * std::cos(angle);
		float vy = Speed * std::sin(angle);
		std::cout << angle << std::endl;
		body->SetLinearVelocity((b2Vec2(vx * 100, vy * 100)));
		Velocity = (b2Vec2(vx * 100, vy * 100));
		IsInitialized = true;
	}

	void Update(sf::Time dt) override
	{
		if (IsInitialized)
		{
			body->SetLinearVelocity(Velocity);
			if (!IsDone || IsDone)
			{
				/*this->Move(Speed*difference*(20.0f / dt.asMicroseconds()));*/
				this->sprite.setPosition(sf::Vector2f(body->GetPosition().x, body->GetPosition().y));
				travelledDistance += Speed * (20.0f / dt.asMilliseconds());
				/*this->Move(Speed*difference*(1.0f / dt.asMilliseconds()));
				travelledDistance += Speed * (1.0f / dt.asMilliseconds());*/
				if (travelledDistance > MaxDistance)
				{
					IsDone = true;
					travelledDistance = 0;
				}
			}
		}
	}
	void  SetObjectPosition(sf::Vector2f pos) override
	{
		collision.left = pos.x;
		collision.top = pos.y;

		sprite.setPosition(pos);

	}
};

//base class for items that can be put in the inventory etc.
//this class is made for data handling
class Item
{
private:

protected:

public:
	//item's sprite
	//usage is not obligatory can be relaced with any kind of drawable object
	sf::Sprite sprite;

	std::string name;

	Item(std::string name):name(name){}

	virtual void Draw(sf::RenderWindow*&window)
	{
		window->draw(sprite);
	}
};

//base class for weapons
class Weapon:public Item
{
protected:

public:

	projectile*Projectile;

	size_t ammoPerClip = 0;
	size_t maxClips = 2;

	int ammoInTheClip = ammoPerClip;
	int  clips = maxClips;

	size_t weaponType = WEAPON_TYPE_NONE;
	float projectileSpeed = 0.f;
	float projectileDamage = 0.f;
	std::vector<Animation::Animation> * animations = new std::vector<Animation::Animation>();
	Animation::SpriteSheetAnimation* Anim;

	void fire(State&state)
	{
	
	}
	std::function<void(Object*object)>projectileOnCollision = [this](Object*object)
	{
		Projectile->CollidingObjects->push_back(object);
	};
	std::function<void(Object*object)>projectileLeftCollision = [this](Object*object)
	{
		if (!Projectile->CollidingObjects->empty())
		{
			Projectile->CollidingObjects->erase(std::find(Projectile->CollidingObjects->begin(), Projectile->CollidingObjects->end(), object));
		}

	};
	Weapon(std::string name,float projectileSpeed, float projectileDamage):Item(name)
	{
		this->projectileSpeed = projectileSpeed;
		this->projectileDamage = projectileDamage;
	}

};

//class that acts like inventory made of items
class ItemContainer
{
protected:

	

	//number of items per strip
	//mostly needed for displaying items in states
	int _stripLenght = 1;

	//number of items per row
	//mostly needed for displaying items in states
	int _rowLenght = 1;

	//-1 for infinite
	int _maxItemsCount = 1;


public:

	//used by gui components such as ItemInventoryUIC
	sf::Vector2f onScreenPosition;

	//id of item that's beeing used by mouse etc.
	//assinged by state
	int currentItemId = -1;
	
	//should container be drawn
	bool isVisible = false;
	
	//can be interacted with (mouse clicks etc.)
	bool isActive = false;


	//all items in container
	std::vector<Item*>*_items = new std::vector<Item*>();

	const bool isEmpty()const
	{
		return _items->empty();
	}

	//doesn't add if stack is full
	void addItem(Item*&item)
	{
		if (_maxItemsCount != -1)
		{
			if (_items->size() < _maxItemsCount)
			{
				_items->push_back(item);
			}
		}	
	}

	//draws all items in stack by calling Item::Draw(sf::RenderWindow*&window)
	void drawItems(sf::RenderWindow*&window)
	{
		if (!_items->empty())
		{
			
			int stripId = 0;
			int rowId = 0;
			for (size_t i = 0; i < _items->size(); i++)
			{
				
				if (stripId <= _stripLenght)
				{
					_items->at(i)->sprite.setPosition(onScreenPosition.x+20*stripId, onScreenPosition.y*rowId);
					_items->at(i)->Draw(window);
					stripId++;
				}
				else
				{
					stripId = 0;
					rowId++;
					if (rowId > _rowLenght)
					{

					}
				}
			}
		}
	}

	Item*getItemByName(std::string name)
	{
		if (!_items->empty())
		{
			for (size_t i = 0; i < _items->size(); i++)
			{
				if (_items->at(i)->name == name) { return _items->at(i); break; }
			}
			return nullptr;
		}
		else
		{
			return nullptr;
		}
	}

	int getStripLenght()const { return _stripLenght; }

	int getRowLenght()const { return _rowLenght; }

	void setStripLenght(int stripLenght) { _stripLenght = stripLenght; }

	void setRowLenght(int rowLenght) { _rowLenght = rowLenght; }

	ItemContainer(int maxItemsCount, int stripLenght, int rowLenght)
	{
		this->_maxItemsCount = maxItemsCount;
		this->_stripLenght = stripLenght;
		this->_rowLenght = rowLenght;
	}
	ItemContainer(int maxItemsCount)
	{
		this->_maxItemsCount = maxItemsCount;
	}
	ItemContainer()
	{
		this->_maxItemsCount = -1;
	}
};

namespace GUI
{
	// gui component (UIC - user interface component) for drawing and hadling interactions with ItemContainers
	//0
	//0
	//WARNING!
	//this class is Specificly made for "the awakened dead" game and mostly consists of data only
	class ItemInventoryUIC : public Container
	{
	public:
		//all item containers that will be used by this class
		std::vector<ItemContainer*>*_itemContainers = new std::vector<ItemContainer*>();

		void Init();

		//updates all GUI components of conteiner based on data from ItemContainers
		void Update(sf::Time st)
		{

		}
	};
}

class Player :public SceneActor
{
protected:

	size_t animIndex = 0;

	/*TextureResource* solder_rifle = new TextureResource("solder_rifle", "./textures/survivor_move_rifle_anim.png", false, false);
	TextureResource* solder_pistol = new TextureResource("solder", "./textures/player_idle.gif", false, false);*/

	void UpdateSprites()
	{
		if (currentAnimation != NULL)
		{
			/*if (currentAnimation->CurrentSprite.getTexture() != NULL)
			{
				Scale.x = collision.width / currentAnimation->CurrentSprite.getTexture()->getSize().x;
				Scale.y = collision.height / currentAnimation->CurrentSprite.getTexture()->getSize().y;
			}
			else
			{
				Scale.x = collision.width / sprite.getTexture()->getSize().x;
				Scale.y = collision.height / sprite.getTexture()->getSize().y;
			}
			for (size_t i = 0; i < currentAnimation->Sprites->size();i++)
			{
				currentAnimation->Sprites->at(i).setRotation(RotationAngle);
				currentAnimation->Sprites->at(i).setScale(Scale);
				currentAnimation->Sprites->at(i).setOrigin(sf::Vector2f(currentAnimation->Sprites->at(i).getTextureRect().width / 2, currentAnimation->Sprites->at(i).getTextureRect().height / 2));
			}*/

			currentAnimation->CurrentSprite.setRotation(RotationAngle);

			/*currentAnimation->CurrentSprite.setScale(Scale);
			currentAnimation->CurrentSprite.setOrigin(sf::Vector2f(collision.width / 2, collision.height / 2));*/
		}
		else
		{
			/*Scale.x = collision.width / sprite.getTexture()->getSize().x;
			Scale.y = collision.height / sprite.getTexture()->getSize().y;*/
		}
	
		sprite.setRotation(RotationAngle);
		sprite.setScale(Scale);
		sprite.setOrigin(sf::Vector2f(collision.width / 2, collision.height / 2));

		
	}
public:

	float health = 100.f;
	/*std::vector<Animation::Animation> * animations = new std::vector<Animation::Animation>();
	Animation::SpriteSheetAnimation* Anim;*/

	Animation::SpritesAnimation*currentAnimation = nullptr;
	Animation::SpritesAnimationsContainer*spritesAnimations = new Animation::SpritesAnimationsContainer();


	size_t weapon_id = 0;

	std::vector<Weapon*>*weapons = new std::vector<Weapon*>();
	
	ItemContainer*items = new ItemContainer(3, 1, 3);

	float MaxSpeed = 1.f;
	float accel = 0.5f;
	sf::Vector2f Velocity;
	std::vector<projectile*>*Projectiles = new std::vector<projectile*>();
	Weapon*currentWeapon = nullptr;
	Player(sf::Vector2f position, float width, float height,sf::Sprite sprite) :SceneActor(position, sprite)
	{
		collision.width = width;
		collision.height = height;	
	}



	Player(sf::Sprite sprite) :SceneActor(sprite)
	{
		
	}



	bool SetAnimation(std::string name)
	{
		if (!spritesAnimations->animations->empty())
		{
			for (size_t i = 0; i < spritesAnimations->animations->size(); i++)
			{
				if (spritesAnimations->animations->at(i)->Name == name)
				{
					spritesAnimations->AnimIndex = i;
					currentAnimation = spritesAnimations->animations->at(i);
					return true;
				}
			}
		}
		return false;
	}

	sf::Vector2f GetObjectPosition() override
	{
		return sf::Vector2f(collision.left, collision.top);
	}

	void AddVelocity(sf::Vector2f vel)
	{
		this->Velocity += vel;
	}
	void SetVelocity(sf::Vector2f vel)
	{
		this->Velocity = vel;
	}

	void SetVelocityX(float x)
	{
		this->Velocity.x = x;
	}
	void SetVelocityY(float y)
	{
		this->Velocity.y = y;
	}

	void Update(sf::Time dt)override
	{
		

		if (!weapons->empty())
		{
			currentWeapon = weapons->at(weapon_id);
			if (currentWeapon->weaponType == WEAPON_TYPE_TAD_RIFLE)
			{
				/*if (body->GetLinearVelocity().x > 0 || body->GetLinearVelocity().y > 0)
				{*/
					SetAnimation("solder_rifle_move");
				//}
			}
			if (currentWeapon->weaponType == WEAPON_TYPE_TAD_PISTOL)
			{
				Scale.x = GetObjectRectangle().width / sprite.getTexture()->getSize().x;
				Scale.y = GetObjectRectangle().height / sprite.getTexture()->getSize().y;

				if (body->GetLinearVelocity().x != 0 || body->GetLinearVelocity().y != 0)
				{
					SetAnimation("solder_pistol_move");
				}
			}
			UpdateSprites();
		}
		else
		{
			currentWeapon = nullptr;
		}

		if (currentAnimation != NULL)
		{
			currentAnimation->UpdateAnimation(dt);
			currentAnimation->CurrentSprite.setPosition(sf::Vector2f(body->GetPosition().x+50, body->GetPosition().y+50));
		}

		/*if (!animations->empty())
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
		}*/
		
		

		/*this->Move(sf::Vector2f(Velocity.x*(dt.asMilliseconds()), Velocity.y*(dt.asMilliseconds())));*/
		/*this->Anim->sprite.setPosition(sf::Vector2f(body->GetPosition().x, body->GetPosition().y));*/

		this->collision.left = body->GetPosition().x;
		this->collision.top = body->GetPosition().y;
		
		if (!Projectiles->empty())
		{
			for (size_t i = 0; i < Projectiles->size(); i++)
			{
				Projectiles->at(i)->Update(dt);
			}
		}

	}
	virtual void Draw(sf::RenderWindow*&window)override
	{
		if (currentAnimation != NULL)
		{
			window->draw(this->currentAnimation->CurrentSprite);
		}
		
		if (!Children->empty())
		{
			for (size_t i = 0; i < this->Children->size(); i++)
			{
				Children->at(i)->Draw(window);
			}
		}
		if (!Projectiles->empty())
		{
			for (size_t i = 0; i < Projectiles->size(); i++)
			{
				Projectiles->at(i)->Draw(window);
			}
		}
	}

	void Move(sf::Vector2f vec)override
	{
		collision.left += vec.x;
		collision.top += vec.y;
		sprite.move(vec);
	}

	void SetObjectPosition(sf::Vector2f pos)override
	{
		collision.left = pos.x;
		collision.top = pos.y;
		sprite.setPosition(pos);
	}
	void Init()override
	{
		if (!spritesAnimations->animations->empty())
		{
			for (size_t in = 0; in < spritesAnimations->animations->size(); in++)
			{
				if (!spritesAnimations->animations->at(in)->Sprites->empty())
				{
					for (size_t i = 0; i < spritesAnimations->animations->at(in)->Sprites->size(); i++)
					{
						sf::Vector2f scale;
						scale.x = collision.width / spritesAnimations->animations->at(in)->Sprites->at(i).getTexture()->getSize().x;
						scale.y = collision.height / spritesAnimations->animations->at(in)->Sprites->at(i).getTexture()->getSize().y;

						spritesAnimations->animations->at(in)->Sprites->at(i).setRotation(RotationAngle);
						spritesAnimations->animations->at(in)->Sprites->at(i).setScale(scale);
						spritesAnimations->animations->at(in)->Sprites->at(i).setOrigin(sf::Vector2f(spritesAnimations->animations->at(in)->Sprites->at(i).getTextureRect().width / 2, spritesAnimations->animations->at(in)->Sprites->at(i).getTextureRect().height / 2));
					}
				}
			}
		}

		//this->Anim->sprite.setScale(this->Scale);
		//this->Anim->sprite.setPosition(sf::Vector2f(collision.left, collision.top));
		//this->Anim->sprite.setRotation(RotationAngle);
		//this->Anim->sprite.setOrigin(sf::Vector2f(collision.width / 2, collision.height / 2));

		this->sprite.setPosition(sf::Vector2f(collision.left, collision.top));

		/*solder_pistol->CreateResourceFromFile();
		solder_rifle->CreateResourceFromFile();*/

		sprite.setRotation(RotationAngle);
		sprite.setScale(Scale);
		sprite.setOrigin(sf::Vector2f(collision.width / 2, collision.height / 2));
	}
	

	
};

class npc_zombie :public MovingPawn
{
protected:
	//index of current animation
	size_t animIndex = 0;

	bool IsDead = false;

	void UpdateSprites()
	{
		if (currentAnimation != NULL)
		{

			currentAnimation->CurrentSprite.setRotation(RotationAngle);
		}
		else
		{

		}
	}
public:
	float Health = 100.f;

	Animation::SpritesAnimation*currentAnimation = nullptr;
	Animation::SpritesAnimationsContainer*spritesAnimations = new Animation::SpritesAnimationsContainer();

	virtual void onDamage(float damage, Object*object, Context*&context, std::string stateName)
	{
		Health -= damage;

		if (Health <= 0)
		{
			IsDead = true;
			Animation::Animation rhand_anim = Animation::Animation("rhand");
			rhand_anim.FrameIndexes->push_back(Animation::CellIndex(0, 0));


			Decal*rhand = new Decal(sf::Vector2f(this->body->GetPosition().x + 60, this->body->GetPosition().y), 0.05f, true, 20.0f, 123, 53, sf::Sprite(context->game->Resources->getTextureResourceDataByName("zombie_right_hand")->texture), 51, 27);
			b2PolygonShape shape;
			shape.SetAsBox(rhand->GetObjectRectangle().width / 2, rhand->GetObjectRectangle().height / 2);

			b2BodyDef def;
			def.position.Set(rhand->GetObjectPosition().x + rhand->GetObjectRectangle().width / 2, rhand->GetObjectPosition().y + rhand->GetObjectRectangle().height / 2);
			def.type = b2BodyType::b2_dynamicBody;

			rhand->body = context->game->GetStateByName(stateName)->world.CreateBody(&def);

			rhand->body->CreateFixture(&shape, 0.5f);
			rhand->body->SetUserData(rhand);

			rhand->Init();
			rhand->animations->push_back(rhand_anim);
			rhand->SetAnimation("rhand");

			rhand->applyImpulse(b2Vec2(100, 0));

			context->game->GetStateByName(stateName)->StateObjects->push_back(rhand);


			Decal*lhand = new Decal(sf::Vector2f(this->body->GetPosition().x, this->body->GetPosition().y + 30), 0.05f, true, 20.0f, 123, 53, sf::Sprite(context->game->Resources->getTextureResourceDataByName("zombie_left_hand")->texture), 51, 27);

			//b2BodyDef def;
			def.position.Set(lhand->GetObjectPosition().x + lhand->GetObjectRectangle().width / 2, lhand->GetObjectPosition().y + lhand->GetObjectRectangle().height / 2);
			def.type = b2BodyType::b2_dynamicBody;
			lhand->body = context->game->GetStateByName(stateName)->world.CreateBody(&def);

			/*b2PolygonShape shape;*/
			shape.SetAsBox(lhand->GetObjectRectangle().width / 2, lhand->GetObjectRectangle().height / 2);


			lhand->body->CreateFixture(&shape, 0.5f);
			lhand->body->SetUserData(lhand);
			lhand->Init();
			lhand->animations->push_back(rhand_anim);
			lhand->SetAnimation("rhand");
			context->game->GetStateByName(stateName)->StateObjects->push_back(lhand);

			/*Decal*meat_chunk = new Decal(sf::Vector2f(this->body->GetPosition().x, this->body->GetPosition().y), 0.05f, true, 20.0f, 35, 65, sf::Sprite(context->game->Resources->getTextureResourceDataByName("meat_chunk")->texture), 100, 100);
			meat_chunk->Init();
			meat_chunk->animations->push_back(rhand_anim);
			meat_chunk->SetAnimation("rhand");
			context->game->GetStateByName(stateName)->StateObjects->push_back(meat_chunk);*/
		}


	}
	virtual void leftCollision(Object*&object, Context*&context, std::string stateName)
	{

	}

	virtual void onCollision(Object*&object, Context*&context, std::string stateName)
	{
		if (dynamic_cast<Player*>(object))
		{
			std::cout << "found player" << std::endl;
		}
	}

	npc_zombie(sf::Vector2f position, float speed, float width, float height) :MovingPawn(speed, position, width, height)
	{
		collision.width = width;
		collision.height = height;
	}

	bool SetAnimation(std::string name)
	{
		if (!spritesAnimations->animations->empty())
		{
			for (size_t i = 0; i < spritesAnimations->animations->size(); i++)
			{
				if (spritesAnimations->animations->at(i)->Name == name)
				{
					spritesAnimations->AnimIndex = i;
					currentAnimation = spritesAnimations->animations->at(i);
					return true;
				}
			}
		}
		return false;
	}

	sf::Vector2f GetObjectPosition() override
	{
		return sf::Vector2f(collision.left, collision.top);
	}

	virtual void Draw(sf::RenderWindow*&window)override
	{
		if (IsDead != true)
		{
			if (currentAnimation != NULL)
			{
				window->draw(this->currentAnimation->CurrentSprite);
			}
		}


		if (!Children->empty())
		{
			for (size_t i = 0; i < this->Children->size(); i++)
			{
				Children->at(i)->Draw(window);
			}
		}
	}

	void Init()override
	{
		if (!spritesAnimations->animations->empty())
		{
			for (size_t in = 0; in < spritesAnimations->animations->size(); in++)
			{
				if (!spritesAnimations->animations->at(in)->Sprites->empty())
				{
					for (size_t i = 0; i < spritesAnimations->animations->at(in)->Sprites->size(); i++)
					{
						sf::Vector2f scale;
						scale.x = collision.width / spritesAnimations->animations->at(in)->Sprites->at(i).getTexture()->getSize().x;
						scale.y = collision.height / spritesAnimations->animations->at(in)->Sprites->at(i).getTexture()->getSize().y;

						spritesAnimations->animations->at(in)->Sprites->at(i).setRotation(RotationAngle);
						spritesAnimations->animations->at(in)->Sprites->at(i).setScale(scale);
						spritesAnimations->animations->at(in)->Sprites->at(i).setOrigin(sf::Vector2f(spritesAnimations->animations->at(in)->Sprites->at(i).getTextureRect().width / 2, spritesAnimations->animations->at(in)->Sprites->at(i).getTextureRect().height / 2));
					}
				}
			}
		}

	}

	void Update(sf::Time dt)
	{
		SetAnimation("skeleton_idle");
		if (physBodyInitialized)
		{
			this->collision.left = body->GetPosition().x;
			this->collision.top = body->GetPosition().y;
		}
		if (currentAnimation != NULL)
		{
			currentAnimation->UpdateAnimation(dt);
			currentAnimation->CurrentSprite.setPosition(sf::Vector2f(body->GetPosition().x, body->GetPosition().y));
		}
		this->UpdateMovement(dt);
	}
};


class proje :public projectile
{
public:
	proje(sf::Vector2f position, float Height, float Width, float maxDistance, float Speed, sf::Sprite sprite) :projectile(position,Height,Width,MaxDistance,Speed,sprite)
	{
		collision.width = Width;
		collision.height = Height;
	}
	virtual void projectileOnCollision(Object*&object, Context*&context, std::string stateName) override
	{

		Animation::Animation blood_a = Animation::Animation("blood_a");
		blood_a.FrameIndexes->push_back(Animation::CellIndex(0, 0));
		blood_a.FrameIndexes->push_back(Animation::CellIndex(0, 1));
		blood_a.FrameIndexes->push_back(Animation::CellIndex(0, 2));
		blood_a.FrameIndexes->push_back(Animation::CellIndex(0, 3));
		blood_a.FrameIndexes->push_back(Animation::CellIndex(0, 4));
		blood_a.FrameIndexes->push_back(Animation::CellIndex(0, 5));
		blood_a.FrameIndexes->push_back(Animation::CellIndex(0, 6));
		blood_a.FrameIndexes->push_back(Animation::CellIndex(0, 7));

		if (dynamic_cast<projectile*>(object)) { return; }
		if (dynamic_cast<Player*>(object))
		{ 
			return;
		}
		this->CollidingObjects->push_back(object);


		sf::Vector2f diff;

		//Use location where bullet hit rather then hitted object's location(looks more realistic)
		diff.x = this->body->GetPosition().x - this->Origin.x;
		diff.y = this->body->GetPosition().y - this->Origin.y;


		float a = static_cast<float>((atan2(diff.x, diff.y)*(180 / M_PI)));
		Decal*blood;
		if (npc_zombie*z=dynamic_cast<npc_zombie*>(object))
		{
			z->onDamage(100.f,this,context,stateName);
		}
		else
		{


			blood = new Decal(sf::Vector2f(this->body->GetPosition().x, this->body->GetPosition().y), 0.05f, true, 0.3f, 512, 512, sf::Sprite(context->game->Resources->getTextureResourceDataByName("blood_a_anim")->texture), 100, 100);
			blood->Anim->sprite.setRotation(-a - 270.f);
			blood->Init();
			blood->animations->push_back(blood_a);
			blood->SetAnimation("blood_a");
			context->game->GetStateByName(stateName)->StateObjects->push_back(blood);
		}
	}

	virtual void projectileOnLeftCollision(Object*&object, Context*&context, std::string stateName)override
	{

		if (!this->CollidingObjects->empty())
		{
			if (std::find(this->CollidingObjects->begin(), this->CollidingObjects->end(), object) != this->CollidingObjects->end())
			{
				this->CollidingObjects->erase(std::find(this->CollidingObjects->begin(), this->CollidingObjects->end(), object));
			}
		}
	}
};





class TextObject :public Object
{
protected:
	sf::Color Color;
	int Size = 0;
	std::string Text;

public:

	sf::Color GetOriginalColor()
	{
		return Color;
	}
	sf::Text textObj;
	TextObject(std::string text, sf::Font&font, sf::Color color, int size)
	{
		this->Text = text;
		this->Color = color;
		this->Size = size;

		textObj = sf::Text(text, font, size);
	}
	virtual void OnCollision() {}
	virtual void SetObjectPosition(sf::Vector2f pos) override
	{
		collision.left = pos.x;
		collision.top = pos.y;

		textObj.setPosition(pos);
	}
};
//Test class for menu system
class MenuState :public State
{
protected:
	size_t seletion_index = 0;
public:
	std::vector<TextObject*>*Options = new std::vector<TextObject*>();
	PixelParticleSystem cursorParticles = PixelParticleSystem(3000, sf::Color::Red);

	GUI::Container*container = new GUI::Container();

	FMOD::Sound *click;
	FMOD::Sound *hover;
	FMOD::Sound*release;
	/*FMOD::Channel *channel;*/
	std::vector<FMOD::Channel*>*Channels = new std::vector<FMOD::Channel*>(MAX_SOUND_CHANNELS_COUNT);
	void Init()override
	{
		context->game->lowSoundSystem->createSound("./sounds/ui/buttonclick.wav", FMOD_3D, 0, &click);
		context->game->lowSoundSystem->createSound("./sounds/ui/buttonrollover.wav", FMOD_3D, 0, &hover);
		context->game->lowSoundSystem->createSound("./sounds/ui/buttonclickrelease.wav", FMOD_3D, 0, &release);

		GUI::Button*Button = new GUI::Button("Play", sf::Color::White, context->game->Resources->getFontResourceDataByName("calibri")->font, 64, sf::Sprite(context->game->Resources->getTextureResourceDataByName("textBoxTexture1")->texture));
		sf::IntRect rect = sf::IntRect(Button->GetPosition().x, Button->GetPosition().y, 0, 0);

		cursorParticles.texture = new sf::Texture(context->game->Resources->getTextureResourceDataByName("proj")->texture);
		/*Button->Sprite.setTextureRect(rect);*/

		Button->SetPosition(sf::Vector2<float>(0, 120));
		Button->Action = [this]()
		{
			context->game->ActivateState(context->game->GetStateByName("PlayState"));
			context->game->DisableState(context->game->GetStateByName("MenuState"));
		};
		this->container->Components->push_back(Button);

		Button->Init();
		GUI::Button*Button2 = new GUI::Button("Quit", sf::Color::White, context->game->Resources->getFontResourceDataByName("calibri")->font, 64, sf::Sprite(context->game->Resources->getTextureResourceDataByName("textBoxTexture1")->texture));
		sf::IntRect rect2 = sf::IntRect(Button2->GetPosition().x, Button2->GetPosition().y, 0, 0);


		/*Button2->Sprite.setTextureRect(rect2);*/

		Button2->SetPosition(sf::Vector2<float>(0, 230));
		Button2->Action = [this]()
		{
			/*context->game->ActivateState(context->game->GetStateByName("PlayState"));
			context->game->DisableState(context->game->GetStateByName("MenuState"));*/
			context->window->close();
		};
		Button2->Init();
		this->container->Components->push_back(Button2);

		GUI::Label*logo = new GUI::Label("The Awakened Dead Pre-Alpha", sf::Color::Red, context->game->Resources->getFontResourceDataByName("calibri")->font, 80, context->game->Resources->getTextureResourceDataByName("textBoxTexture1")->texture);
		logo->Init();
		this->container->Components->push_back(logo);
	}
	MenuState(std::string Name) :State(Name)
	{
		
	}


	virtual void Draw()override
	{

		if (!this->StateObjects->empty())
		{
			for (size_t i = 0; i < this->StateObjects->size(); i++)
			{
				if (!dynamic_cast<SceneActor*>(this->StateObjects->at(i)))
				{

				}

				else
				{

					context->window->draw(dynamic_cast<SceneActor*>(this->StateObjects->at(i))->sprite);
				}
			}

		}
		/*if (!this->Options->empty())
		{
			for (size_t i = 0; i < this->Options->size(); i++)
			{
				context->window->draw(dynamic_cast<TextObject*>(this->Options->at(i))->textObj);
			}
		}*/

		if (!container->Components->empty())
		{
			for (size_t i = 0; i < container->Components->size(); i++)
			{
				container->Components->at(i)->Draw(context->window);
			}
		}
		context->window->draw(cursorParticles);
	}

	virtual void HandleEvent(sf::Event& event)override
	{
		if (!container->Components->empty())
		{
			if (event.type == sf::Event::Closed)
			{
				context->window->close();
			}
			if (event.mouseButton.button == sf::Mouse::Left&&event.type == sf::Event::EventType::MouseButtonPressed)
			{
				if (!container->Components->empty())
				{
					for (size_t i = 0; i < container->Components->size(); i++)
					{
						if (container->Components->at(i)->ComponentRectangle.contains(sf::Vector2f(event.mouseMove.x, event.mouseMove.y)))
						{
							container->Components->at(seletion_index)->Activate();
							seletion_index = i;
							if (!Channels->empty())
							{
								for (size_t i = 0; i < Channels->size(); i++)
								{
									bool res;
									Channels->at(i)->isPlaying(&res);
									if (Channels->at(i) == NULL)
									{
										context->game->lowSoundSystem->playSound(click, 0, false, &Channels->at(i));

										break;
									}
									else if (res == false)
									{
										context->game->lowSoundSystem->playSound(click, 0, false, &Channels->at(i));

										break;
									}
								}

							}
						}
						else
						{

						}
					}
				}

				/*cursorParticles.setEmitter(sf::Vector2f(event.mouseButton.x, event.mouseButton.y));*/
			}
			if (event.type == sf::Event::EventType::MouseMoved)
			{

				event.mouseMove.x;
				event.mouseMove.y;
				cursorParticles.setEmitter(sf::Vector2f(event.mouseMove.x, event.mouseMove.y));
				if (!container->Components->empty())
				{
					for (size_t i = 0; i < container->Components->size(); i++)
					{
						if (container->Components->at(i)->isSelectable)
						{
							if (container->Components->at(i)->ComponentRectangle.contains(sf::Vector2f(event.mouseMove.x, event.mouseMove.y)))
							{
								container->Select(i);
								seletion_index = i;
								if (!Channels->empty())
								{
									for (size_t i = 0; i < Channels->size(); i++)
									{
										bool res;
										Channels->at(i)->isPlaying(&res);
										if (Channels->at(i) == NULL)
										{
											context->game->lowSoundSystem->playSound(hover, 0, false, &Channels->at(i));

											break;
										}
										else if (res == false)
										{
											context->game->lowSoundSystem->playSound(hover, 0, false, &Channels->at(i));

											break;
										}
									}

								}
							}
							else
							{
								container->Components->at(i)->UnSelect();
							}
						}
					}
				}

			}
			/*if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
			{
				for (size_t i = 0; i < Options->size(); i++)
				{
					Options->at(i)->textObj.setColor(Options->at(i)->GetOriginalColor());
				}

				if (seletion_index == Options->size()-1)
				{
					Options->at(seletion_index)->textObj.setColor(sf::Color::Red);
				}
				else
				{
					seletion_index++;
					if (seletion_index > Options->size()-1)
					{
						seletion_index = Options->size()-1;
					}
					Options->at(seletion_index)->textObj.setColor(sf::Color::Red);
				}


			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
			{
				for (size_t i = 0; i < Options->size(); i++)
				{
					Options->at(i)->textObj.setColor(Options->at(i)->GetOriginalColor());
				}
				if (seletion_index == 0)
				{
					Options->at(seletion_index)->textObj.setColor(sf::Color::Red);
				}
				else
				{
					seletion_index -= 1;
					Options->at(seletion_index)->textObj.setColor(sf::Color::Red);
				}



			}*/

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
			{
				for (size_t i = 0; i < container->Components->size(); i++)
				{
					container->Components->at(i)->UnSelect();
				}

				container->SelectNext();
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
			{
				for (size_t i = 0; i < container->Components->size(); i++)
				{
					container->Components->at(i)->UnSelect();
				}
				container->SelectPrevious();
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter))
			{
				container->Components->at(seletion_index)->Activate();
			}

		}

	}

	virtual void Update(sf::Time dt) override
	{
		context->game->lowSoundSystem->update();
		/*context->window->setMouseCursorVisible(false);*/
		cursorParticles.update(dt);
	}
};







//state(mode) when player & everything is moves
class PlayState :public State
{
	
public:

	GUI::Container*PlayerUI = new GUI::Container();

	Player*player;
	projectile* projObj;

	float speed = 1.0f;
	sf::Vector2f dest;
	sf::Vector2f diff = sf::Vector2f(0, 0);

	sf::View view;
	bool WalkUp = false;
	bool WalkDown = false;
	bool WalkLeft = false;
	bool WalkRight = false;

	ObjectContactListener contact_listener;
	FMOD::ChannelGroup* PlayerSoundsGroup;
	PixelParticleSystem cursorParticles = PixelParticleSystem(300, sf::Color::Red);

	FMOD::Sound *shoot;
	FMOD::Sound *shoot2;
	FMOD::Sound*ambience1;

	b2World world;
	/*FMOD::Channel *channel;*/
	std::vector<FMOD::Channel*>*Channels = new std::vector<FMOD::Channel*>(MAX_SOUND_CHANNELS_COUNT);
	std::vector<PixelParticleSystemObject>*pixelParticleSystems = new std::vector<PixelParticleSystemObject>();

	void Init()override
	{
		

#pragma region mapLoading
		//loading resources for game from tad_tileset.tsx
		using namespace tinyxml2;
		XMLDocument doc;
		doc.LoadFile("./../maps/t_t1.tmx");


		XMLElement* root = doc.FirstChildElement("map");

		//current tile id x
		int cTileIdx = 0;
		//current tile id y
		int cTileIdy = 0;

		int layerMaxWidth =0;
		int layerMaxHeight = 0;

		
		for (tinyxml2::XMLElement* child = root->FirstChildElement(); child != NULL; child = child->NextSiblingElement())
		{
			if (child == nullptr) { XMLCheckResult(XML_ERROR_PARSING_ELEMENT); break; }
			std::string name = child->Name();
			if (name == "layer")
			{
				int layerId = child->FindAttribute("id")->IntValue();
				layerMaxWidth = child->FindAttribute("width")->IntValue();
				layerMaxHeight = child->FindAttribute("height")->IntValue();

				XMLElement*data = child->FirstChildElement("data");
				if (data != nullptr)
				{
					XMLElement*chunk = data->FirstChildElement("chunk");
					if (chunk != NULL)
					{
						for (tinyxml2::XMLElement* tile = chunk->FirstChildElement(); tile != NULL; tile = tile->NextSiblingElement())
						{

							if (tile->FindAttribute("gid") != NULL)
							{
								int gid;
								gid = tile->FindAttribute("gid")->IntValue();
								gid--;

								sf::Vector2f pos = sf::Vector2f(64 * cTileIdx, 64 * cTileIdy);
								SceneTile*ta = new SceneTile(pos, sf::Sprite(context->game->Resources->getTextureResourceDataByName(std::to_string(gid))->texture), 64, 64);
								ta->Init();
								StateObjects->push_back(ta);


							}
							cTileIdx++;
							if (cTileIdx >= layerMaxWidth)
							{
								cTileIdx = 0;
								cTileIdy++;
							}
						}
					}
					else
					{
						for (tinyxml2::XMLElement* tile = data->FirstChildElement(); tile != NULL; tile = tile->NextSiblingElement())
						{

							if (tile->FindAttribute("gid") != NULL)
							{
								int gid;
								gid = tile->FindAttribute("gid")->IntValue();
								gid--;

								sf::Vector2f pos = sf::Vector2f(64 * cTileIdx, 64 * cTileIdy);
								SceneTile*ta = new SceneTile(pos, sf::Sprite(context->game->Resources->getTextureResourceDataByName(std::to_string(gid))->texture), 64, 64);
								ta->Init();
								StateObjects->push_back(ta);


							}
							cTileIdx++;
							if (cTileIdx >= layerMaxWidth)
							{
								cTileIdx = 0;
								cTileIdy++;
							}
						}
					}
				}
				
			}
			
			else if (name == "objectgroup")
			{
				for (tinyxml2::XMLElement* obj = child->FirstChildElement(); obj != NULL; obj = obj->NextSiblingElement())
				{

					if (obj->FindAttribute("type") != NULL)
					{
						std::string type = obj->FindAttribute("type")->Value();

						if (type == "info_player_start")
						{
							float posX = obj->FindAttribute("x")->FloatValue();
							float posY = obj->FindAttribute("y")->FloatValue();
							this->player->SetObjectPosition(sf::Vector2f(posX, posY));
						}
						if (type == "solid_object")
						{
							float posX = obj->FindAttribute("x")->FloatValue();
							float posY = obj->FindAttribute("y")->FloatValue();
							float width = obj->FindAttribute("width")->FloatValue();
							float height = obj->FindAttribute("height")->FloatValue();
							this->StateObjects->push_back(new SolidObject(sf::Vector2f(posX, posY), sf::Sprite(), width, height));
						}


					}
					else if (obj->FindAttribute("template") != NULL)
					{
						std::string templateName = obj->FindAttribute("template")->Value();

						XMLDocument tempDoc;

						const char* d = "./../maps/";

						char result[100];   // array to hold the result.

						strcpy(result, d); // copy string one into the result.
						strcat(result, templateName.c_str()); // append string two to the result.

							
						tempDoc.LoadFile(result);

						XMLElement* root = tempDoc.FirstChildElement("template");

						XMLElement*tileSetData = root->FirstChildElement("tileset");

						if (tileSetData != NULL)
						{

						}

						XMLElement*objData = root->FirstChildElement("object");
						if (objData != NULL)
						{
							if (objData->FindAttribute("type") != NULL)
							{
								std::string type = objData->FindAttribute("type")->Value();


								int gid;
								gid = objData->FindAttribute("gid")->IntValue();
								gid--;
								if (type == "info_player_start")
								{
									float posX = obj->FindAttribute("x")->FloatValue();
									float posY = obj->FindAttribute("y")->FloatValue();
									this->player->SetObjectPosition(sf::Vector2f(posX, posY));
								}
								if (type == "solid_object")
								{
									float posX = obj->FindAttribute("x")->FloatValue();
									float posY = obj->FindAttribute("y")->FloatValue();
									float width = objData->FindAttribute("width")->FloatValue();
									float height = objData->FindAttribute("height")->FloatValue();
									this->StateObjects->push_back(new SolidObject(sf::Vector2f(posX, posY), sf::Sprite(), width, height));
								}
								if (type == "PropPhysics")
								{
									float posX = obj->FindAttribute("x")->FloatValue();
									float posY = obj->FindAttribute("y")->FloatValue();
									float width = objData->FindAttribute("width")->FloatValue();
									float height = objData->FindAttribute("height")->FloatValue();

									this->StateObjects->push_back(new PropPhysics(sf::Vector2f(posX, posY), sf::Sprite(context->game->Resources->getTextureResourceDataByName(std::to_string(gid))->texture), width, height));
								}
							}
						}
					}
				}

			}
		}
		//end of loading resources for game from tad_tileset.tsx
#pragma endregion
		Animation::SpritesAnimation*rifle_move =new  Animation::SpritesAnimation(true,0.2f, "solder_rifle_move");
		for (int i = 0; i < 20; i++)
		{
			rifle_move->AddFrame(sf::Sprite(context->game->Resources->getTextureResourceDataByName("solder_rifle_move_" + std::to_string(i))->texture));
		}
		player->spritesAnimations->addAnimation(rifle_move);
		Animation::SpritesAnimation*pistol_move = new  Animation::SpritesAnimation(true, 0.2f, "solder_pistol_move");
		for (int i = 0; i < 20; i++)
		{
			pistol_move->AddFrame(sf::Sprite(context->game->Resources->getTextureResourceDataByName("solder_pistol_move_" + std::to_string(i))->texture));
		}
		player->spritesAnimations->addAnimation(pistol_move);
		player->Init();

		npc_zombie*z = new npc_zombie(sf::Vector2f(500, 500),1.f, 100, 100);
		z->Init();
		z->OnCollision = [this, z](Object*object)
		{
			z->onCollision(object, this->context, "PlayState");
		};
		/*z->AddMovement(MovementDirection(-95.0f, 10.0f));*/

		/*SolidObject*wall = new SolidObject(sf::Vector2f(100, 300), sf::Sprite(context->game->Resources->getTextureResourceDataByName("textBoxTexture1")->texture), 300, 300);
		wall->Init();*/

		npc_test_turret*tt = new npc_test_turret(sf::Vector2<float>(500, 100), sf::FloatRect(250, 50, 300, 100), 100, 100, 45, 32, sf::Sprite(context->game->Resources->getTextureResourceDataByName("officer_walk")->texture));
		tt->Init();
		tt->OnCollision = [tt](Object*object)
		{
			sf::Vector2f diff;
			diff.x = object->body->GetPosition().x - tt->GetObjectPosition().x;
			diff.y = object->body->GetPosition().y - tt->GetObjectPosition().y;


			tt->SetObjectRotation((atan2(diff.y, diff.x)*(180 / M_PI)));

		};
		tt->LeftCollision = [tt](Object*object)
		{
			/*std::cout << "l" << std::endl;*/
		};



		projObj = new projectile(sf::Vector2f(0, 0), 100.f, 100.f, 50.0f, 2.0f, sf::Sprite(context->game->Resources->getTextureResourceDataByName("proj")->texture));

		npc_moving_helper*helper = new npc_moving_helper(1.0f, "no help from me", sf::Color::White, context->game->Resources->getFontResourceDataByName("calibri")->font, sf::Vector2f(100, 100), 100, 100, 45, 32, sf::Sprite(context->game->Resources->getTextureResourceDataByName("officer_walk")->texture));
	

		helper->animations->push_back(context->game->animationsData->getAnimationDataByName("run"));
		tt->animations->push_back(context->game->animationsData->getAnimationDataByName("run"));
		tt->SetAnimation("run");

		Animation::SpritesAnimation*zombie_idle = new  Animation::SpritesAnimation(true, 0.2f, "skeleton_idle");
		for (int i = 0; i < 17; i++)
		{
			zombie_idle->AddFrame(sf::Sprite(context->game->Resources->getTextureResourceDataByName("skeleton-idle_" + std::to_string(i))->texture));
		}
		z->spritesAnimations->addAnimation(zombie_idle);
		z->Init();
		z->SetAnimation("skeleton_idle");

		helper->SetAnimation("run");

		helper->AddMovement(MovementDirection(65.0f, 10.0f));
		helper->AddMovement(MovementDirection(-15.0f, 10.0f));

		helper->OnCollision = [helper](Object*object)
		{
			helper->DisplayText = true;
			MovementDirection dir(helper->Pattern->at(helper->dirIndex).Angle*-1, helper->Pattern->at(helper->dirIndex).Distance);
			helper->Pattern->clear();
			helper->dirIndex = 0;
			helper->AddMovement(dir);
			helper->CollidingObjects->push_back(object);
			helper->SetAnimation("unr");

		};
		helper->LeftCollision = [helper](Object*object)
		{
			helper->DisplayText = false;
			if (!helper->CollidingObjects->empty())
			{
				if (std::find(helper->CollidingObjects->begin(), helper->CollidingObjects->end(), object) != helper->CollidingObjects->end())
				{
					helper->CollidingObjects->erase(std::find(helper->CollidingObjects->begin(), helper->CollidingObjects->end(), object));
				}
			}

		};

		helper->help_text.setString("no help from me");

		/*helper->sprite.setPosition(100, 100);*/
		helper->Init();
		/*this->StateObjects->push_back(wall);*/
		this->StateObjects->push_back(helper);
		this->StateObjects->push_back(tt);
		this->StateObjects->push_back(z);




		//GUI

		GUI::Label*l1 = new GUI::Label("weapon_name","0", sf::Color::Red, context->game->Resources->getFontResourceDataByName("calibri")->font, 80, context->game->Resources->getTextureResourceDataByName("textBoxTexture1")->texture);
		l1->Init();
		this->PlayerUI->Components->push_back(l1);

		GUI::Label*l2 = new GUI::Label("health", "0", sf::Color::Red, context->game->Resources->getFontResourceDataByName("calibri")->font, 80, context->game->Resources->getTextureResourceDataByName("textBoxTexture1")->texture);
		l2->SetPosition(sf::Vector2f(10, SCREENHEIGHT));
		l2->Init();
		this->PlayerUI->Components->push_back(l2);


		cursorParticles.Stop();
		projObj->Init();
		//GUI
		

		b2Filter filter;
		filter.categoryBits = 0x1;
		
		
		

		if (!StateObjects->empty())
		{
			for (size_t i = 0; i < StateObjects->size(); i++)
			{
				if (SolidObject*obj = dynamic_cast<SolidObject*>(StateObjects->at(i)))
				{
					b2BodyDef def;
					def.position.Set(obj->GetObjectPosition().x + obj->GetObjectRectangle().width / 2, obj->GetObjectPosition().y + obj->GetObjectRectangle().height / 2);
					def.type = b2BodyType::b2_staticBody;
					obj->body = world.CreateBody(&def);

					b2PolygonShape shape;
					shape.SetAsBox(obj->GetObjectRectangle().width / 2, obj->GetObjectRectangle().height / 2);

					
					obj->body->CreateFixture(&shape, 0.f);
					obj->body->SetUserData(obj);
					
					StateObjects->at(i)->physBodyInitialized = true;
					StateObjects->at(i)->bodyIsSensor = false;
				}
				else if (dynamic_cast<SceneTile*>(StateObjects->at(i)))
				{
					
				}
				else if (npc_zombie*z = dynamic_cast<npc_zombie*>(StateObjects->at(i)))
				{
					b2BodyDef def;
					def.position.Set(StateObjects->at(i)->GetObjectPosition().x + StateObjects->at(i)->GetObjectRectangle().width / 2, StateObjects->at(i)->GetObjectPosition().y + StateObjects->at(i)->GetObjectRectangle().height / 2);
					def.type = b2BodyType::b2_dynamicBody;
					StateObjects->at(i)->body = world.CreateBody(&def);

					b2PolygonShape shape;
					shape.SetAsBox(StateObjects->at(i)->GetObjectRectangle().width / 2, StateObjects->at(i)->GetObjectRectangle().height / 2);

					b2PolygonShape senseShape;
					senseShape.SetAsBox(StateObjects->at(i)->GetObjectRectangle().width , StateObjects->at(i)->GetObjectRectangle().height );

					b2FixtureDef TriggerFixture;
					TriggerFixture.filter = filter;
					TriggerFixture.density = 0.f;
					TriggerFixture.shape = &shape;
					TriggerFixture.isSensor = 0;

					b2FixtureDef senceFixture;
					senceFixture.filter = filter;
					senceFixture.density = 0.f;
					senceFixture.shape = &senseShape;
					senceFixture.isSensor = 1;

					StateObjects->at(i)->body->CreateFixture(&TriggerFixture);
					StateObjects->at(i)->body->CreateFixture(&senceFixture);
					StateObjects->at(i)->body->SetUserData(StateObjects->at(i));

					StateObjects->at(i)->physBodyInitialized = true;
					//is set by main fixture and/or purpose of the object itself
					StateObjects->at(i)->bodyIsSensor = TriggerFixture.isSensor;
				}
				else if (PropPhysics*pp = dynamic_cast<PropPhysics*>(StateObjects->at(i)))
				{
					b2PolygonShape shapeA;
					shapeA.SetAsBox(StateObjects->at(i)->GetObjectRectangle().width / 2, StateObjects->at(i)->GetObjectRectangle().height / 2);


					b2BodyDef defA;
					defA.position.Set(StateObjects->at(i)->GetObjectPosition().x + StateObjects->at(i)->GetObjectRectangle().width / 2, StateObjects->at(i)->GetObjectPosition().y + StateObjects->at(i)->GetObjectRectangle().height / 2);
					defA.type = b2BodyType::b2_dynamicBody;

					StateObjects->at(i)->body = world.CreateBody(&defA);

					StateObjects->at(i)->body->CreateFixture(&shapeA, 1.f);
					StateObjects->at(i)->body->SetUserData(StateObjects->at(i));
					StateObjects->at(i)->Init();
					StateObjects->at(i)->OnCollision = [this, pp](Object*object)
					{
						pp->onCollision(object, this->context, "PlayState");
					};
					pp->LeftCollision = [this, pp](Object*object)
					{
						pp->leftCollision(object, this->context, "PlayState");
					};
					StateObjects->at(i)->physBodyInitialized = true;
					StateObjects->at(i)->bodyIsSensor = false;


					StateObjects->at(i)->Init();
				}
				else
				{
					
					b2BodyDef def;
					def.position.Set(StateObjects->at(i)->GetObjectPosition().x + StateObjects->at(i)->GetObjectRectangle().width / 2, StateObjects->at(i)->GetObjectPosition().y + StateObjects->at(i)->GetObjectRectangle().height / 2);
					def.type = b2BodyType::b2_dynamicBody;
					StateObjects->at(i)->body = world.CreateBody(&def);

					b2PolygonShape shape;
					shape.SetAsBox(StateObjects->at(i)->GetObjectRectangle().width / 2, StateObjects->at(i)->GetObjectRectangle().height / 2);

					b2FixtureDef TriggerFixture;
					TriggerFixture.filter = filter;
					TriggerFixture.density = 0.f;
					TriggerFixture.shape = &shape;
					TriggerFixture.isSensor = 0;

					StateObjects->at(i)->body->CreateFixture(&TriggerFixture);
					StateObjects->at(i)->body->SetUserData(StateObjects->at(i));

					StateObjects->at(i)->physBodyInitialized = true;
					StateObjects->at(i)->bodyIsSensor = TriggerFixture.isSensor;

					StateObjects->at(i)->Init();
				}
			}
		}

		b2Filter filter2;
		filter2.categoryBits = 0x1;

		//player body begin
		player->Init();

		b2BodyDef defP;
		defP.position.Set(player->GetObjectPosition().x + player->GetObjectRectangle().width / 2, player->GetObjectPosition().y + player->GetObjectRectangle().height / 2);
		defP.type = b2BodyType::b2_dynamicBody;
		player->body = world.CreateBody(&defP);

		/*b2PolygonShape shape;
		shape.SetAsBox(player->GetObjectRectangle().width / 2, player->GetObjectRectangle().height / 2);*/
		b2CircleShape shapeP;
		shapeP.m_radius = 50;

		b2FixtureDef TriggerFixtureP;
		TriggerFixtureP.filter = filter2;
		TriggerFixtureP.density = 1.f;
		TriggerFixtureP.shape = &shapeP;

		player->body->CreateFixture(&TriggerFixtureP);
		player->body->SetUserData(player);



		//player body end
		player->Init();

		

		context->game->lowSoundSystem->createSound("./sounds/pistol_fire2.wav", FMOD_3D, 0, &shoot);
		context->game->lowSoundSystem->createSound("./sounds/fire1.wav", FMOD_3D, 0, &shoot2);
		context->game->lowSoundSystem->createSound("./sounds/ambience_base.wav", FMOD_3D, 0, &ambience1);
		ambience1->setMode(FMOD_LOOP_NORMAL);
		ambience1->setLoopCount(-1);
		
		if (!Channels->empty())
		{
			for (size_t i = 0; i < Channels->size(); i++)
			{
				bool res;
				Channels->at(i)->isPlaying(&res);
				if (Channels->at(i) == NULL)
				{
					context->game->lowSoundSystem->playSound(ambience1, 0, false, &Channels->at(i));

					break;
				}
				else if (res == false)
				{
					context->game->lowSoundSystem->playSound(ambience1, 0, false, &Channels->at(i));

					break;
				}
			}

		}


		Animation::Animation rhand_anim = Animation::Animation("rhand");
		rhand_anim.FrameIndexes->push_back(Animation::CellIndex(0, 0));

		/*Decal*rhand = new Decal(sf::Vector2f(60.f, 200.f), 0.05f, true, 20.0f, 123, 53, sf::Sprite(context->game->Resources->getTextureResourceDataByName("zombie_right_hand")->texture), 510, 270);
		b2PolygonShape shapeH;
		shapeH.SetAsBox(rhand->GetObjectRectangle().width / 2, rhand->GetObjectRectangle().height / 2);

		b2BodyDef defH;
		defH.position.Set(rhand->GetObjectPosition().x + rhand->GetObjectRectangle().width / 2, rhand->GetObjectPosition().y + rhand->GetObjectRectangle().height / 2);
		defH.type = b2BodyType::b2_dynamicBody;

		rhand->body = world.CreateBody(&defH);

		rhand->body->CreateFixture(&shapeH, 0.5f);
		rhand->body->SetUserData(rhand);

		rhand->Init();
		rhand->animations->push_back(rhand_anim);
		rhand->SetAnimation("rhand");

		rhand->applyImpulse(b2Vec2(100, 0));

		StateObjects->push_back(rhand);*/


		PropPhysics*armChair = new PropPhysics(sf::Vector2f(400, 400), sf::Sprite(context->game->Resources->getTextureResourceDataByName("3")->texture), 100, 100);
		b2PolygonShape shapeA;
		shapeA.SetAsBox(armChair->GetObjectRectangle().width / 2, armChair->GetObjectRectangle().height / 2);

		
		b2BodyDef defA;
		defA.position.Set(armChair->GetObjectPosition().x + armChair->GetObjectRectangle().width / 2, armChair->GetObjectPosition().y + armChair->GetObjectRectangle().height / 2);
		defA.type = b2BodyType::b2_dynamicBody;

		armChair->body = world.CreateBody(&defA);

		armChair->body->CreateFixture(&shapeA,1.f);
		armChair->body->SetUserData(armChair);
		armChair->Init();
		armChair->OnCollision = [this, armChair](Object*object)
		{
			armChair->onCollision(object, this->context, "PlayState");
		};
		armChair->LeftCollision = [this, armChair](Object*object)
		{
			armChair->leftCollision(object, this->context, "PlayState");
		};
		armChair->physBodyInitialized = true;
		armChair->bodyIsSensor = false;
		StateObjects->push_back(armChair);
	}
	PlayState(std::string Name) :State(Name),world(b2Vec2(0.f,0.f))
	{
		world.SetContactListener(&contact_listener);
	}
	virtual void Draw() override
	{

		if (!this->StateObjects->empty())
		{
			for (size_t i = 0; i < this->StateObjects->size(); i++)
			{

				
				/*context->window->draw(dynamic_cast<SceneActor*>(this->StateObjects->at(i))->sprite);*/
				this->StateObjects->at(i)->Draw(context->window);
				if (DEBUG_DRAWCOLLISION)
				{
					if (this->StateObjects->at(i)->physBodyInitialized == true)
					{

						if (this->StateObjects->at(i)->body->GetFixtureList() != NULL)
						{
							
							
							
							for (b2Fixture*fix = this->StateObjects->at(i)->body->GetFixtureList(); fix != NULL; fix = fix->GetNext())
							{
								if (fix->GetType() == b2Shape::Type::e_circle)
								{
									b2CircleShape*coll = dynamic_cast<b2CircleShape*>(fix->GetShape());
									sf::CircleShape cs = sf::CircleShape(coll->m_radius);
									cs.setPosition(sf::Vector2f(this->StateObjects->at(i)->body->GetPosition().x, this->StateObjects->at(i)->body->GetPosition().y));
									context->window->draw(cs);
								}
								if (fix->GetType() == b2Shape::Type::e_polygon)
								{
									b2PolygonShape*coll = dynamic_cast<b2PolygonShape*>(fix->GetShape());

									sf::VertexArray va = sf::VertexArray(sf::PrimitiveType::LineStrip);

									for (int ind = 0; ind < coll->m_count; ind++)
									{
										va.append(sf::Vertex(sf::Vector2f(coll->m_vertices[ind].x + this->StateObjects->at(i)->GetObjectPosition().x, coll->m_vertices[ind].y + this->StateObjects->at(i)->GetObjectPosition().y), sf::Color::Red));
									}
									va.append(sf::Vertex(sf::Vector2f(coll->m_vertices[0].x + this->StateObjects->at(i)->GetObjectPosition().x, coll->m_vertices[0].y + this->StateObjects->at(i)->GetObjectPosition().y), sf::Color::Red));
									context->window->draw(va);
								}
							}
							/*if (this->StateObjects->at(i)->body->GetFixtureList()->GetType() == b2Shape::Type::e_circle)
							{
								b2CircleShape*coll = dynamic_cast<b2CircleShape*>(this->StateObjects->at(i)->body->GetFixtureList()->GetShape());
								sf::CircleShape cs = sf::CircleShape(coll->m_radius);
								cs.setPosition(sf::Vector2f(this->StateObjects->at(i)->body->GetPosition().x, this->StateObjects->at(i)->body->GetPosition().y));
								context->window->draw(cs);
							}
							if (this->StateObjects->at(i)->body->GetFixtureList()->GetType() == b2Shape::Type::e_polygon)
							{
								b2PolygonShape*coll = dynamic_cast<b2PolygonShape*>(this->StateObjects->at(i)->body->GetFixtureList()->GetShape());

								sf::VertexArray va = sf::VertexArray(sf::PrimitiveType::LineStrip);

								for (int ind = 0; ind < coll->m_count; ind++)
								{
										va.append(sf::Vertex(sf::Vector2f(coll->m_vertices[ind].x+ this->StateObjects->at(i)->GetObjectPosition().x, coll->m_vertices[ind].y +this->StateObjects->at(i)->GetObjectPosition().y), sf::Color::Red));
								}							
								context->window->draw(va);
							}*/
						}

					}
				}

				/*context->window->draw(projObj->sprite);*/

			}

		}
		player->Draw(context->window);
		if (DEBUG_DRAWCOLLISION)
		{
			if (player->body->GetFixtureList() != NULL)
			{
				if (player->body->GetFixtureList()->GetType() == b2Shape::Type::e_circle)
				{
					b2CircleShape*coll = dynamic_cast<b2CircleShape*>(player->body->GetFixtureList()->GetShape());
					sf::CircleShape cs = sf::CircleShape(coll->m_radius);
					cs.setPosition(sf::Vector2f(player->body->GetPosition().x, player->body->GetPosition().y));
					cs.setFillColor(sf::Color::Red);
					context->window->draw(cs);
				}
			}
		}
		/*if (!player->Projectiles->empty())
		{
			for (size_t i = 0; i < player->Projectiles->size(); i++)
			{
				player->Projectiles->at(i)->Draw(context->window);
			}
		}*/
		context->window->draw(cursorParticles);
		for (size_t i = 0; i < pixelParticleSystems->size(); i++)
		{
			context->window->draw(pixelParticleSystems->at(i));
		}
		/*sf::VertexArray lines(sf::LinesStrip, 2);
		lines[0].position = player->GetObjectPosition();
		lines[1].position = projObj->GetObjectPosition();
		context->window->draw(lines);*/

		if (!PlayerUI->Components->empty())
		{
			for (size_t i = 0; i < PlayerUI->Components->size(); i++)
			{
				PlayerUI->Components->at(i)->Draw(context->window);
			}
		}


		if (!player->items->isEmpty()&&player->items->isVisible)
		{
			player->items->drawItems(context->window);
		}
	}

	bool CheckObjectCollision(Object*&object)
	{
		if (StateObjects->empty()) { return false; }
		bool res = false;
		for (b2ContactEdge* ce = object->body->GetContactList(); ce; ce->next)
		{
			b2Contact* c = ce->contact;
			for (size_t i = 0; i < StateObjects->size(); i++)
			{
				if (object != StateObjects->at(i)&&c->GetFixtureA()!=c->GetFixtureB())
				{
					if (std::find(object->CollidingObjects->begin(), object->CollidingObjects->end(), StateObjects->at(i)) == object->CollidingObjects->end())
					{
						if (c->GetFixtureA() == StateObjects->at(i)->body->GetFixtureList())
						{
							StateObjects->at(i)->OnCollision(object);
							object->OnCollision(StateObjects->at(i));
							res = true;
							break;
						}
						else
						{

						}
					}
					else
					{
						if (c->GetFixtureA() != StateObjects->at(i)->body->GetFixtureList())
						{
							object->LeftCollision(StateObjects->at(i));
							StateObjects->at(i)->LeftCollision(object);
						}
					}
				}
			}
			break;
		}
		return res;
	}
	bool CheckProjectileCollision(projectile *proj)
	{
		/*if (StateObjects->empty()) { return false; }
		bool res = false;
		for (size_t i = 0; i < StateObjects->size(); i++)
		{
			if (std::find(proj->CollidingObjects->begin(), proj->CollidingObjects->end(), StateObjects->at(i)) == proj->CollidingObjects->end())
			{
				for (b2ContactEdge* ce = StateObjects->at(i)->body->GetContactList(); ce; ce->next)
				{
					b2Contact* c = ce->contact;
					if (c->GetFixtureA() == StateObjects->at(i)->body->GetFixtureList())
					{
						StateObjects->at(i)->OnCollision(proj);
						proj->OnCollision(StateObjects->at(i));
						res = true;
					}
					else
					{
						proj->LeftCollision(StateObjects->at(i));
						StateObjects->at(i)->LeftCollision(proj);
					}
				}
			}
			else
			{
				for (b2ContactEdge* ce = StateObjects->at(i)->body->GetContactList(); ce; ce->next)
				{
					b2Contact* c = ce->contact;
					if (c->GetFixtureA() == StateObjects->at(i)->body->GetFixtureList())
					{
						res = true;
					}
					else
					{
						proj->LeftCollision(StateObjects->at(i));
						StateObjects->at(i)->LeftCollision(proj);
					}
				}
			}
		}*/


		//if (StateObjects->empty()) { return false; }
		//for (size_t i = 0; i < StateObjects->size(); i++)
		//{
		//	if (std::find(proj->CollidingObjects->begin(), proj->CollidingObjects->end(), StateObjects->at(i)) == proj->CollidingObjects->end())
		//	{

		//		if (proj->GetObjectRectangle().intersects(StateObjects->at(i)->GetObjectRectangle()))
		//		{
		//			StateObjects->at(i)->OnCollision(proj);
		//			proj->OnCollision(StateObjects->at(i));
		//			/*return true;*/
		//		}
		//	}
		//	else
		//	{
		//		if (!proj->GetObjectRectangle().intersects(StateObjects->at(i)->GetObjectRectangle()))
		//		{
		//			proj->LeftCollision(StateObjects->at(i));
		//			StateObjects->at(i)->LeftCollision(proj);
		//		}
		//	}
		//}

		return false;
	}
	bool CheckPlayerCollision()
	{
		if (StateObjects->empty()) { return false; }
		bool res = false;
		for (b2ContactEdge* ce = player->body->GetContactList(); ce; ce->next)
		{
			b2Contact* c = ce->contact;
			for (size_t i = 0; i < StateObjects->size(); i++)
			{
				if (std::find(player->CollidingObjects->begin(), player->CollidingObjects->end(), StateObjects->at(i)) == player->CollidingObjects->end())
				{
					if (c->GetFixtureA() == StateObjects->at(i)->body->GetFixtureList())
					{
						StateObjects->at(i)->OnCollision(player);
						player->OnCollision(StateObjects->at(i));
						res = true;
						break;
					}
					else
					{
						
					}
				}
				else
				{
					if (c->GetFixtureA() != StateObjects->at(i)->body->GetFixtureList())
					{						
						player->LeftCollision(StateObjects->at(i));
						StateObjects->at(i)->LeftCollision(player);
					}
				}
			}
			break;
		}
		
		/*if (StateObjects->empty()) { return false; }
		for (size_t i = 0; i < StateObjects->size(); i++)
		{
			if (std::find(player->CollidingObjects->begin(), player->CollidingObjects->end(), StateObjects->at(i)) == player->CollidingObjects->end())
			{
				if (player->GetObjectRectangle().intersects(StateObjects->at(i)->GetObjectRectangle()))
				{
					StateObjects->at(i)->OnCollision(player);
					player->OnCollision(StateObjects->at(i));
					return true;
				}

			}
			else
			{
				StateObjects->at(i)->LeftCollision(player);
				player->LeftCollision(StateObjects->at(i));
			}
		}
		return false;*/
	}
	virtual void HandleEvent(sf::Event& event) override
	{

		sf::Vector2f m;
		m.x = 1.0f;
		m.y = 0.0f;
		sf::Vector2f m2;
		m2.x = 0.0f;
		m2.y = 1.0f;

	

		FMOD::ChannelGroup *cg;
		

		if (event.type == sf::Event::EventType::Closed)
		{
			context->window->close();
		}
		if (event.key.code == sf::Keyboard::Tab&&event.type == sf::Event::EventType::KeyPressed)
		{
			if (!player->weapons->empty())
			{
				player->items->setStripLenght(player->weapons->size());
				for (size_t i = 0; i < player->weapons->size(); i++)
				{
					Item*w = new Item(player->weapons->at(i)->name);
					w->sprite = player->weapons->at(i)->sprite;
					w->sprite.setTextureRect(player->weapons->at(i)->sprite.getTextureRect());
					player->items->addItem(w);
				}
				player->items->isVisible = !player->items->isVisible;
				player->items->isActive = !player->items->isActive;

			}
			
		}
		if (event.mouseButton.button == sf::Mouse::Left&&event.type == sf::Event::EventType::MouseButtonPressed)
		{
			if (player->items->isActive&&player->items->isEmpty())
			{
				for (size_t i = 0; i < player->items->_items->size(); i++)
				{
					if (player->items->_items->at(i)->sprite.getGlobalBounds().contains(sf::Vector2f(event.mouseButton.x, event.mouseButton.y)))
					{
						player->items->currentItemId = i;
					}
				}
			}

			if (!Channels->empty())
			{
				for (size_t i = 0; i < Channels->size(); i++)
				{
					bool res;
					Channels->at(i)->isPlaying(&res);
					if (Channels->at(i) == NULL)
					{
						if (player->currentWeapon->weaponType == WEAPON_TYPE_TAD_PISTOL)
						{
							context->game->lowSoundSystem->playSound(shoot, 0, false, &Channels->at(i));
						}
						if (player->currentWeapon->weaponType == WEAPON_TYPE_TAD_RIFLE)
						{
							context->game->lowSoundSystem->playSound(shoot2, 0, false, &Channels->at(i));
						}
						

						break;
					}
					else if (res == false)
					{
						if(player->currentWeapon->weaponType == WEAPON_TYPE_TAD_PISTOL)
						{
							context->game->lowSoundSystem->playSound(shoot, 0, false, &Channels->at(i));
						}
						if (player->currentWeapon->weaponType == WEAPON_TYPE_TAD_RIFLE)
						{
							context->game->lowSoundSystem->playSound(shoot2, 0, false, &Channels->at(i));
						}

						break;
					}
				}

			}

			this->world;
			this->context->game->GetStateByName("PlayState")->world;
			Animation::Animation blood_a = context->game->animationsData->getAnimationDataByName("blood_a");

			b2Filter filter;
			filter.categoryBits = 1;

			sf::Vector2f mousePos = sf::Vector2f(event.mouseButton.x + player->GetObjectPosition().x - (SCREENWIDTH / 2), event.mouseButton.y + player->GetObjectPosition().y - (SCREENHEIGHT / 2));
			sf::Vector2f diff;
			diff.x = mousePos.x - player->GetObjectPosition().x;
			diff.y = mousePos.y - player->GetObjectPosition().y;

			if (player->currentWeapon->ammoInTheClip <= 0)
			{
				if (player->currentWeapon->clips > 0)
				{
					player->currentWeapon->clips -= 1;
					player->currentWeapon->ammoInTheClip = player->currentWeapon->ammoPerClip;
				}
				else
				{
					return;
				}
			}
			if (player->currentWeapon->weaponType == WEAPON_TYPE_TAD_RIFLE)
			{
				proje* bullet = new proje(sf::Vector2f(0, 0), 10.f, 10.f, 50.0f, player->currentWeapon->projectileSpeed*10, sf::Sprite(context->game->Resources->getTextureResourceDataByName("proj")->texture));

				bullet->OnCollision = [this, blood_a, bullet](Object*object)
				{
					bullet->projectileOnCollision(object, this->context, "PlayState");
				};
				bullet->LeftCollision = [this, bullet](Object*object)
				{
					bullet->projectileOnLeftCollision(object, this->context, "PlayState");
				};

				bullet->Launch(static_cast<float>((atan2(diff.y, diff.x)/**(180 / M_PI)*/)), sf::Vector2f(player->body->GetPosition().x,player->body->GetPosition().y), this->world, filter);
				player->Projectiles->push_back(bullet);
				player->currentWeapon->ammoInTheClip -= 1;
			
			}
			else if (player->currentWeapon->weaponType == WEAPON_TYPE_TAD_PISTOL)
			{
				
				projectile* bullet = new projectile(sf::Vector2f(0, 0), 10.f, 10.f, 50.0f, player->currentWeapon->projectileSpeed, sf::Sprite(context->game->Resources->getTextureResourceDataByName("proj")->texture));

				bullet->OnCollision = [this, blood_a, bullet](Object*object)
				{

					bullet->projectileOnCollision(object, this->context, "PlayState");
				};
				bullet->LeftCollision = [this, bullet](Object*object)
				{
					bullet->projectileOnLeftCollision(object, this->context, "PlayState");
				};

				bullet->Launch(static_cast<float>((atan2(diff.y, diff.x)/**(180 / M_PI)*/)), sf::Vector2f(player->body->GetPosition().x, player->body->GetPosition().y), this->world, filter);
				player->Projectiles->push_back(bullet);
				player->currentWeapon->ammoInTheClip -= 1;
			}
			
			
		}
		if (event.mouseButton.button == sf::Mouse::Right&&event.type == sf::Event::EventType::MouseButtonPressed)
		{
			/*projObj->SetObjectPosition(sf::Vector2f(player->GetObjectRectangle().left, player->GetObjectRectangle().top));
			dest = sf::Vector2f(event.mouseButton.x, event.mouseButton.y);
			diff = dest - player->GetObjectPosition();
			int vo = 0;*/
			if (!Channels->empty())
			{
				for (size_t i = 0; i < Channels->size(); i++)
				{
					bool res;
					Channels->at(i)->isPlaying(&res);
					if (res == false)
					{
						context->game->lowSoundSystem->playSound(shoot2, 0, false, &Channels->at(i));

						break;
					}
					else if (Channels->at(i) == NULL)
					{
						context->game->lowSoundSystem->playSound(shoot2, 0, false, &Channels->at(i));

						break;
					}

				}

			}

			Animation::Animation blood_a = Animation::Animation("blood_a");
			blood_a.FrameIndexes->push_back(Animation::CellIndex(0, 0));
			blood_a.FrameIndexes->push_back(Animation::CellIndex(0, 1));
			blood_a.FrameIndexes->push_back(Animation::CellIndex(0, 2));
			blood_a.FrameIndexes->push_back(Animation::CellIndex(0, 3));
			blood_a.FrameIndexes->push_back(Animation::CellIndex(0, 4));
			blood_a.FrameIndexes->push_back(Animation::CellIndex(0, 5));
			blood_a.FrameIndexes->push_back(Animation::CellIndex(0, 6));
			blood_a.FrameIndexes->push_back(Animation::CellIndex(0, 7));

			b2Filter filter;
			filter.categoryBits = 1;

			sf::Vector2f diff;

			diff.x = event.mouseButton.x - player->GetObjectPosition().x;
			diff.y = event.mouseButton.y - player->GetObjectPosition().y;

			projectile* bullet = new projectile(sf::Vector2f(0, 0), 10.f, 10.f, 50.0f, player->currentWeapon->projectileSpeed, sf::Sprite(context->game->Resources->getTextureResourceDataByName("proj")->texture));

			bullet->OnCollision = [this, blood_a, bullet](Object*object)
			{

				if (dynamic_cast<projectile*>(object)) { return; }
				if (dynamic_cast<Player*>(object)) { return; }
				bullet->CollidingObjects->push_back(object);


				sf::Vector2f diff;
				/*diff.x = object->GetObjectPosition().x - bullet->Origin.x;
				diff.y = object->GetObjectPosition().y - bullet->Origin.y;*/

				//Use location where bullet hit rather then hitted object's location(looks more realistic)
				diff.x = bullet->body->GetPosition().x - bullet->Origin.x;
				diff.y = bullet->body->GetPosition().y - bullet->Origin.y;


				float a = static_cast<float>((atan2(diff.x, diff.y)*(180 / M_PI)));
				Decal*blood;
				if (dynamic_cast<npc_zombie*>(object))
				{


					blood = new Decal(sf::Vector2f(bullet->body->GetPosition().x, bullet->body->GetPosition().y), 0.05f, true, 0.3f, 512, 512, sf::Sprite(context->game->Resources->getTextureResourceDataByName("blood_a_anim")->texture), 100, 100);
					blood->Anim->sprite.setRotation(-a - 270.f);
					blood->Init();
					blood->animations->push_back(blood_a);
					blood->SetAnimation("blood_a");
					this->StateObjects->push_back(blood);
				}
				else
				{


					blood = new Decal(sf::Vector2f(bullet->body->GetPosition().x, bullet->body->GetPosition().y), 0.05f, true, 0.3f, 512, 512, sf::Sprite(context->game->Resources->getTextureResourceDataByName("blood_a_anim")->texture), 100, 100);
					blood->Anim->sprite.setRotation(-a - 270.f);
					blood->Init();
					blood->animations->push_back(blood_a);
					blood->SetAnimation("blood_a");
					this->StateObjects->push_back(blood);
				}
			};
			bullet->LeftCollision = [this, bullet](Object*object)
			{
				if (!bullet->CollidingObjects->empty())
				{
					if (std::find(bullet->CollidingObjects->begin(), bullet->CollidingObjects->end(), object) != bullet->CollidingObjects->end())
					{
						bullet->CollidingObjects->erase(std::find(bullet->CollidingObjects->begin(), bullet->CollidingObjects->end(), object));
					}
				}
			};

			bullet->Launch(static_cast<float>((atan2(diff.y, diff.x)/**(180 / M_PI)*/)), sf::Vector2f(player->body->GetPosition().x, player->body->GetPosition().y), this->world, filter);
			player->Projectiles->push_back(bullet);
		}
		if (event.type == sf::Event::EventType::MouseWheelScrolled)
		{
			int wid = player->weapon_id;
			if ((wid + event.mouseWheel.delta) < 0)
			{
				player->weapon_id = 0;
			
			}
			else
			{
				player->weapon_id += event.mouseWheelScroll.delta;
				if (player->weapon_id > player->weapons->size() - 1)
				{
					player->weapon_id = player->weapons->size() - 1;
				}
			}
			
		}
		if (event.type == sf::Event::EventType::MouseMoved)
		{
			sf::Vector2f mousePos = sf::Vector2f(event.mouseMove.x + player->GetObjectPosition().x - (SCREENWIDTH / 2), event.mouseMove.y + player->GetObjectPosition().y - (SCREENHEIGHT / 2));
			sf::Vector2f diff;
			diff.x = mousePos.x - player->GetObjectPosition().x;
			diff.y = mousePos.y - player->GetObjectPosition().y;
		

			/*std::cout << atan2(diff.y, diff.x)*(180 / M_PI) << std::endl;*/
		/*	player->Anim->sprite.setRotation((atan2(diff.y, diff.x)*(180 / M_PI)));*/
			player->RotationAngle = (atan2(diff.y, diff.x)*(180 / M_PI));
		}
		if (event.key.code == sf::Keyboard::Left&&event.type == sf::Event::EventType::KeyPressed)
		{
			WalkLeft = true;
		}
		if (event.key.code == sf::Keyboard::Right&&event.type == sf::Event::EventType::KeyPressed)
		{
			WalkRight = true;
		}
		if (event.key.code == sf::Keyboard::Up&&event.type == sf::Event::EventType::KeyPressed)
		{
			WalkUp = true;
		}
		if (event.key.code == sf::Keyboard::Down&&event.type == sf::Event::EventType::KeyPressed)
		{
			WalkDown = true;
		}


		if (event.key.code == sf::Keyboard::Left&&event.type == sf::Event::EventType::KeyReleased)
		{
			WalkLeft = false;
		}
		if (event.key.code == sf::Keyboard::Right&&event.type == sf::Event::EventType::KeyReleased)
		{
			WalkRight = false;
		}
		if (event.key.code == sf::Keyboard::Up&&event.type == sf::Event::EventType::KeyReleased)
		{
			WalkUp = false;
		}
		if (event.key.code == sf::Keyboard::Down&&event.type == sf::Event::EventType::KeyReleased)
		{
			WalkDown = false;
		}

		/*if (WalkLeft)
		{
			
			player->AddVelocity(sf::Vector2f(-(player->accel), 0.f));
			if (player->Velocity.x < -player->MaxSpeed)
			{
				player->Velocity.x = -player->MaxSpeed;
			}
		}
		else if (!WalkLeft)
		{
			player->AddVelocity(sf::Vector2f((player->accel), 0.f));
			if (player->Velocity.x > player->MaxSpeed)
			{
				player->Velocity.x = player->MaxSpeed;
			}
		}
		if (WalkRight)
		{
			player->AddVelocity(sf::Vector2f((player->accel), 0.f));
			if (player->Velocity.x > player->MaxSpeed)
			{
				player->Velocity.x = player->MaxSpeed;
			}
		}
		else if (!WalkRight)
		{
			player->AddVelocity(sf::Vector2f(-(player->accel), 0.f));
			if (player->Velocity.x < -player->MaxSpeed)
			{
				player->Velocity.x = -player->MaxSpeed;
			}
		}
		if (WalkUp)
		{
			player->AddVelocity(sf::Vector2f(0.f, -(player->accel)));
			if (player->Velocity.y < -player->MaxSpeed)
			{
				player->Velocity.y = -player->MaxSpeed;
			}
		}
		else if (!WalkUp)
		{
			player->AddVelocity(sf::Vector2f(0.f, (player->accel)));
			if (player->Velocity.y > player->MaxSpeed)
			{
				player->Velocity.y = player->MaxSpeed;
			}
		}
		if (WalkDown)
		{
			player->AddVelocity(sf::Vector2f(0.f, (player->accel)));
			if (player->Velocity.y > player->MaxSpeed)
			{
				player->Velocity.y = player->MaxSpeed;
			}
		}
		else if (!WalkDown)
		{
			player->AddVelocity(sf::Vector2f(0.f, -(player->accel)));
			if (player->Velocity.y < -player->MaxSpeed)
			{
				player->Velocity.y = -player->MaxSpeed;
			}
		}*/

		if(WalkLeft)
		{
			if ((player->Velocity.x - player->accel) > (-player->MaxSpeed))
			{
				player->AddVelocity(sf::Vector2f(-(player->accel), 0.f));
			}
			
		}
		if (WalkRight)
		{
			if ((player->Velocity.x + player->accel) < (player->MaxSpeed))
			{
				player->AddVelocity(sf::Vector2f((player->accel),0.f));
			}
			
		}
		else if (!WalkRight && !WalkLeft)
		{
			if (player->Velocity.x > 0.f) {
				player->AddVelocity(sf::Vector2f(-(player->accel), 0.f)); }
			if (player->Velocity.x < 0.f) { player->AddVelocity(sf::Vector2f((player->accel), 0.f)); }
			/*player->SetVelocityX(0.f);*/
		}
		if (WalkUp)
		{
			if ((player->Velocity.y - player->accel) > (-player->MaxSpeed))
			{
				player->AddVelocity(sf::Vector2f(0.f, -(player->accel)));
			}
			
		}
		if (WalkDown)
		{
			if ((player->Velocity.y + player->accel) < (player->MaxSpeed))
			{
				player->AddVelocity(sf::Vector2f(0.f, (player->accel)));
			}
			
		}
		else if (!WalkUp && !WalkDown)
		{
			if (player->Velocity.y > 0.f) {
				player->AddVelocity(sf::Vector2f(0.f, -(player->accel))); }
			if (player->Velocity.y < 0.f) { player->AddVelocity(sf::Vector2f(0.f, (player->accel))); }
			/*player->SetVelocityY(0.f);*/ 
			
		}

		/*if (WalkLeft)
		{
			dynamic_cast<SceneActor*>(this->StateObjects->at(0))->sprite.move(-m);
		}
		if (WalkRight)
		{
			dynamic_cast<SceneActor*>(this->StateObjects->at(0))->sprite.move(m);
		}
		if (WalkUp)
		{
			dynamic_cast<SceneActor*>(this->StateObjects->at(0))->sprite.move(-m2);
		}
		if (WalkDown)
		{
			dynamic_cast<SceneActor*>(this->StateObjects->at(0))->sprite.move(m2);
		}*/


		/*if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
		{
			dynamic_cast<SceneActor*>(this->StateObjects->at(0))->sprite.move(-m);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
		{
			dynamic_cast<SceneActor*>(this->StateObjects->at(0))->sprite.move(m);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
		{
			dynamic_cast<SceneActor*>(this->StateObjects->at(0))->sprite.move(-m2);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
		{
			dynamic_cast<SceneActor*>(this->StateObjects->at(0))->sprite.move(m2);
		}*/

	}

	virtual void Update(sf::Time dt) override
	{
		
		if (!player->items->isEmpty() && player->items->isActive)
		{
			if (player->items->currentItemId != -1)
			{
				sf::Vector2i  mousePos = sf::Mouse::getPosition() - context->window->getPosition();
				player->items->_items->at(player->items->currentItemId)->sprite.setPosition(sf::Vector2f(mousePos.x, mousePos.y));
			}
			
		}
		
		player->body->SetLinearVelocity(b2Vec2(player->Velocity.x*5,player->Velocity.y*5));
		world.Step(1.f / dt.asSeconds(),1, 1);
		player->Update(dt);
		context->window->setFramerateLimit(300);
		context->game->lowSoundSystem->update();
		projObj->Update(dt);
		
		dynamic_cast<GUI::Label*>(PlayerUI->GetComponentByName("weapon_name"))->text.setString(player->currentWeapon->name);
		dynamic_cast<GUI::Label*>(PlayerUI->GetComponentByName("health"))->text.setString(std::to_string(static_cast<int>(player->health)));
		
		if (!PlayerUI->Components->empty())
		{
			for (size_t i = 0; i < PlayerUI->Components->size(); i++)
			{
				PlayerUI->Components->at(i)->SetPosition(sf::Vector2f(50+PlayerUI->Components->at(i)->GetOriginalPosition().x + player->GetObjectPosition().x - (SCREENWIDTH / 2), PlayerUI->Components->at(i)->GetOriginalPosition().y + player->GetObjectPosition().y - (SCREENHEIGHT / 2)));
			}
		}

		sf::View view;
		view.setCenter(player->GetObjectPosition());;
		context->window->setView(view);
		/*CheckPlayerCollision();
		if (!StateObjects->empty())
		{
			for (size_t i = 0; i < StateObjects->size(); i++)
			{
				CheckObjectCollision(StateObjects->at(i));
			}
		}*/
		/*dynamic_cast<npc_moving_helper*>(StateObjects->at(1))->Update(dt);
		dynamic_cast<npc_test_turret*>(StateObjects->at(2))->Update(dt);*/
		if (!StateObjects->empty())
		{
			for (size_t i = 0; i < StateObjects->size(); i++)
			{
				StateObjects->at(i)->Update(dt);
				if (Decal*d = dynamic_cast<Decal*>(StateObjects->at(i)))
				{
					if (d->IsDone())
					{
						if (std::find(StateObjects->begin(), StateObjects->end(), StateObjects->at(i)) != StateObjects->end())
						{
							StateObjects->erase(std::find(StateObjects->begin(), StateObjects->end(), StateObjects->at(i)));
						}
					}
				}
			}
		}
		/*if (dynamic_cast<npc_moving_helper*>(StateObjects->at(1))->dirIndex == dynamic_cast<npc_moving_helper*>(StateObjects->at(1))->Pattern->size() - 1)
		{
			cursorParticles.Stop();
		}*/
	/*	cursorParticles.setEmitter(dynamic_cast<npc_moving_helper*>(StateObjects->at(1))->GetObjectPosition());*/
		for (size_t i = 0; i < pixelParticleSystems->size(); i++)
		{
			pixelParticleSystems->at(i).update(dt);
			
		}
		cursorParticles.update(dt);
	}
};




std::vector<SceneActor*>*Actors = new std::vector<SceneActor*>();


Player*player;

int main(int argc, char** argv)
{
	/*sf::RenderWindow renderWindow(sf::VideoMode(640, 480), "Hello SFML");*/

	Game game("SFML", sf::VideoMode(SCREENWIDTH, SCREENHEIGHT));

	//if (!solder.loadFromFile("./textures/player_idle.gif"))
	//{
	//	return 42;
	//}
	//if (!solder_rifle.loadFromFile("./textures/survivor-move_rifle_0.png"))
	//{
	//	return 42;
	//}
	//sf::Texture Officer1;
	//if (!Officer1.loadFromFile("./textures/bk_officer/officer_walk_strip.png"))
	//{
	//	return 42;
	//}
	//
	//if (!textBoxTexture1.loadFromFile("./textures/textbox1.png"))
	//{
	//	return 42;
	//}
	//*if (!proj.loadFromFile("C:/Users/catgu/source/repos/sfml/x64/Debug/textures/projectile.png"))
	//{
	//	return 42;
	//}*/
	//if (!proj.loadFromFile("./textures/projectile.png"))
	//{
	//	return 42;
	//}
	//textBoxTexture1.setRepeated(true);
	//
	//if (!blood1.loadFromFile("./textures/blood/blood_a_0002.png"))
	//{
	//	return 42; // Robust error handling!
	//}
	//if (!blood_yellow_1.loadFromFile("./textures/blood/blood_yellow_b_0001.png"))
	//{
	//	return 42; // Robust error handling!
	//}
	//
	//if (!blood_a_anim.loadFromFile("./textures/blood/blood_a_anim.png"))
	//{
	//	return 42; // Robust error handling!
	//}
	

	b2Vec2 gravity(0.f, 0.f);
	b2World world(gravity);

	sf::Vector2f g;
	g.x = 0;
	g.y = 100;
	sf::Vector2f g2;
	g2.x = 0;
	g2.y = 200;
	sf::FloatRect rect;
	rect.height = 100;
	rect.width = 100;
	rect.left = g.x;
	rect.top = g.y;
	

	/*solder.setRepeated(true);*/
	auto solder = new TextureResource("solder", "./../textures/player_idle.gif", false, false);
	solder->CreateResourceFromFile();

	/*Animation::Animation animr = Animation::Animation("solder_move_rifle");
	animr.FrameIndexes->push_back(Animation::CellIndex(0, 0));
	animr.FrameIndexes->push_back(Animation::CellIndex(0, 1));
	animr.FrameIndexes->push_back(Animation::CellIndex(0, 2));
	animr.FrameIndexes->push_back(Animation::CellIndex(0, 3));
	animr.FrameIndexes->push_back(Animation::CellIndex(0, 4));
	animr.FrameIndexes->push_back(Animation::CellIndex(0, 5));
	animr.FrameIndexes->push_back(Animation::CellIndex(0, 6));
	animr.FrameIndexes->push_back(Animation::CellIndex(0, 7));
	animr.FrameIndexes->push_back(Animation::CellIndex(0, 8));
	animr.FrameIndexes->push_back(Animation::CellIndex(0, 9));
	animr.FrameIndexes->push_back(Animation::CellIndex(0, 10));
	animr.FrameIndexes->push_back(Animation::CellIndex(0, 11));
	animr.FrameIndexes->push_back(Animation::CellIndex(0, 12));
	animr.FrameIndexes->push_back(Animation::CellIndex(0, 13));
	animr.FrameIndexes->push_back(Animation::CellIndex(0, 14));
	animr.FrameIndexes->push_back(Animation::CellIndex(0, 15));
	animr.FrameIndexes->push_back(Animation::CellIndex(0, 16));
	animr.FrameIndexes->push_back(Animation::CellIndex(0, 17));
	animr.FrameIndexes->push_back(Animation::CellIndex(0, 18));
	animr.FrameIndexes->push_back(Animation::CellIndex(0, 19));*/

	player = new Player(sf::Vector2f(0,0), 150, 100, sf::Sprite(solder->texture));
	/*player->animations->push_back(animr);*/
	player->Init();


	PlayState ps = PlayState("PlayState");
	MenuState ms = MenuState("MenuState");

	
	TextureResource *letterT = new TextureResource("letter", "./../textures/Letter.png", false, false);
	letterT->CreateResourceFromFile();

	Weapon*w=new Weapon("rifle",1.f,1.f);
	w->weaponType = WEAPON_TYPE_TAD_RIFLE;
	w->ammoPerClip = 3;
	w->ammoInTheClip = w->ammoPerClip;
	w->sprite = sf::Sprite(letterT->texture);
	

	Weapon*w1 = new Weapon("pistol",0.00001f, 1.f);
	w1->weaponType = WEAPON_TYPE_TAD_PISTOL;
	w1->ammoPerClip = 4;
	w1->ammoInTheClip = w1->ammoPerClip;
	w1->sprite = sf::Sprite(letterT->texture);
	


	player->weapons->push_back(w);
	player->weapons->push_back(w1);
	
	ps.player = player;

	game.AddState(&ps);
	game.AddState(&ms);


	game.ActivateState(&ms);



	//loading and saving animationData to Game class-----begin

	Animation::Animation blood_a = Animation::Animation("blood_a");
	blood_a.FrameIndexes->push_back(Animation::CellIndex(0, 0));
	blood_a.FrameIndexes->push_back(Animation::CellIndex(0, 1));
	blood_a.FrameIndexes->push_back(Animation::CellIndex(0, 2));
	blood_a.FrameIndexes->push_back(Animation::CellIndex(0, 3));
	blood_a.FrameIndexes->push_back(Animation::CellIndex(0, 4));
	blood_a.FrameIndexes->push_back(Animation::CellIndex(0, 5));
	blood_a.FrameIndexes->push_back(Animation::CellIndex(0, 6));
	blood_a.FrameIndexes->push_back(Animation::CellIndex(0, 7));

	game.animationsData->addAnimationData(blood_a);


	Animation::Animation anim = Animation::Animation("run");
	anim.FrameIndexes->push_back(Animation::CellIndex(0, 0));
	anim.FrameIndexes->push_back(Animation::CellIndex(0, 1));
	anim.FrameIndexes->push_back(Animation::CellIndex(0, 2));
	anim.FrameIndexes->push_back(Animation::CellIndex(0, 3));
	anim.FrameIndexes->push_back(Animation::CellIndex(0, 4));
	anim.FrameIndexes->push_back(Animation::CellIndex(0, 5));
	anim.FrameIndexes->push_back(Animation::CellIndex(0, 6));
	anim.FrameIndexes->push_back(Animation::CellIndex(0, 7));



	game.animationsData->addAnimationData(anim);

	//loading and saving animationData to Game class-----end

	Actors->push_back(player);
	sf::Vector2f m;
	m.x = 0.01f;
	m.y = 0.0f;
	sf::Vector2f m2;
	m2.x = 0.0f;
	m2.y = 0.01f;

	for (int i = 0; i < 20; i++)
	{
		game.Resources->AddTextureResource(new TextureResource("solder_rifle_move_" + std::to_string(i), "./../textures/Top_Down_Survivor/rifle/move/survivor-move_rifle_" + std::to_string(i) + ".png", false, false));
	}
	for (int i = 0; i < 20; i++)
	{
		game.Resources->AddTextureResource(new TextureResource("solder_pistol_move_" + std::to_string(i), "./../textures/Top_Down_Survivor/handgun/move/survivor-move_handgun_" + std::to_string(i) + ".png", false, false));
	}

	for (int i = 0; i < 17; i++)
	{
		game.Resources->AddTextureResource(new TextureResource("skeleton-idle_" + std::to_string(i), "./../textures/zombie/idle/skeleton-idle_" + std::to_string(i) + ".png", false, false));
	}

	sf::View view;
	game.Resources->AddTextureResource(new TextureResource("solder", "./../textures/player_idle.gif",false,false));
	game.Resources->AddTextureResource(new TextureResource("textBoxTexture1","./../textures/textbox1.png" , false, false));
	game.Resources->AddTextureResource(new TextureResource("proj", "./../textures/projectile.png", false, false));
	game.Resources->AddTextureResource(new TextureResource("blood1", "./../textures/blood/blood_a_0002.png", false, false));
	game.Resources->AddTextureResource(new TextureResource("blood_yellow_1", "./../textures/blood/blood_yellow_b_0001.png", false, false));
	game.Resources->AddTextureResource(new TextureResource("blood_a_anim", "./../textures/blood/blood_a_anim.png", false, false));
	game.Resources->AddTextureResource(new TextureResource("solder_rifle", "./../textures/survivor-move_rifle_0.png", false, false));
	game.Resources->AddTextureResource(new TextureResource("officer_walk", "./../textures/bk_officer/officer_walk_strip.png", false, false));
	game.Resources->AddTextureResource(new TextureResource("zombie_right_hand", "./../textures/zombie/zombie_right_hand.png", false, false));
	game.Resources->AddTextureResource(new TextureResource("zombie_left_hand", "./../textures/zombie/zombie_left_hand.png", false, false));
	game.Resources->AddTextureResource(new TextureResource("zombie_head", "./../textures/zombie/zombie_head.png", false, false));
	game.Resources->AddTextureResource(new TextureResource("meat_chunk", "./../textures/zombie/meat_chunk.png", false, false));
	game.Resources->AddTextureResource(new TextureResource("letter", "./../textures/Letter.png", false, false));


	
	Item*let = new Item("letter");
	let->sprite = sf::Sprite(letterT->texture);
	

	//loading resources for game from tad_tileset.tsx
	using namespace tinyxml2;
	XMLDocument doc;
	doc.LoadFile("./../maps/tad_tileset.tsx");


	XMLElement* root = doc.FirstChildElement("tileset");


	for (tinyxml2::XMLElement* child = root->FirstChildElement(); child != NULL; child = child->NextSiblingElement())
	{
		if (child == nullptr) { XMLCheckResult(XML_ERROR_PARSING_ELEMENT); break; }
		std::string name = child->Name();
		if (name == "tile")
		{

			std::string id;
			id = child->ToElement()->FindAttribute("id")->Value();

			XMLElement*imageData = child->ToElement()->FirstChildElement("image");
			std::string source = imageData->FindAttribute("source")->Value();
			game.Resources->AddTextureResource(new TextureResource(id, "./" + source, false, false));
		}
	}
		



	//end of loading resources for game from tad_tileset.tsx

	

	game.Resources->AddFontResource(new FontResource("calibri", "./../fonts/calibri.ttf"));
	try
	{

		game.Init();
		game.Run();
	}
	catch (std::runtime_error e)
	{
		std::cout << e.what() << std::endl;
	}

}
#endif // COMPILE