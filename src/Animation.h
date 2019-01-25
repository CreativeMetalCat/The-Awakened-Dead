#pragma once
#include "SFML/Graphics.hpp"

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
		const int GetCurrentFrameIndex() { return FrameIndex; }
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




		SpritesAnimation(bool IsRepeated, float frameDuration, std::string Name, FrameChangeDirection frameChangeDir = FrameChangeDirection::Right) :IsRepeated(IsRepeated), FrameDuration(frameDuration)
		{
			this->frameChangeDir = frameChangeDir;
			this->Name = Name;
		}

		SpritesAnimation(bool IsRepeated, float frameDuration, std::string Name, std::vector<sf::Sprite>*&Sprites, FrameChangeDirection frameChangeDir = FrameChangeDirection::Right) :IsRepeated(IsRepeated), Sprites(Sprites), FrameDuration(frameDuration)
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
			if (name != "")
			{
				throw(std::runtime_error("Name is empty"));
			}
			if (!animations->empty())
			{
				for (size_t i = 0; i < animations->size(); i++)
				{
					if (animations->at(i)->Name == name) { return animations->at(i); }
				}
				throw(std::runtime_error("No animations with given name: " + name));
			}
			else
			{
				throw(std::runtime_error("No animations avalable"));
			}
		}
	};
}
