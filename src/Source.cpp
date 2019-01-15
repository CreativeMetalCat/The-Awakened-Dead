#define COMPILE

#ifdef COMPILE


//#pragma region Mat Types
//
//
//
//#define MAT_TYPE_GENERIC 0x0
//
//#define MAT_TYPE_WOOD 0x1
//
//#define MAT_TYPE_WOOD_CRATE 0x2
//
//#define MAT_TYPE_WOOD_PLANK 0x3
//
//#define MAT_TYPE_WOOD_BOX 0x4
//
//#define MAT_TYPE_WOOD_CRATE_LIGHT 0x5
//
//#define MAT_TYPE_WOOD_CRATE_HARD 0x6
//
//#define MAT_TYPE_WOOD_PLANK_LIGHT 0x7
//
//#define MAT_TYPE_WOOD_PLANK_HARD 0x8
//
//#define MAT_TYPE_WOOD_BOX_LIGHT 0x9
//
//#define MAT_TYPE_WOOD_BOX_HARD 0xa
//
//#define MAT_TYPE_PLASTIC 0xb
//
//#define MAT_TYPE_PLASTIC_BARREL 0xc
//
//#define MAT_TYPE_PLASTIC_BOX 0xd
//
//#define MAT_TYPE_PLASTIC_BARREL_LIGHT 0xe
//
//#define MAT_TYPE_PLASTIC_BARREL_HARD 0xf
//
//#define MAT_TYPE_PLASTIC_BOX_LIGHT 0x10
//
//#define MAT_TYPE_PLASTIC_BOX_HARD 0x11
//
//#define MAT_TYPE_METAL 0xc
//
//#pragma endregion
//
//#pragma region Mat Types Names
//
//#define MAT_TYPE_WOOD_CRATE_LIGHT_NAME "wood_crate_scrape_rough_loop1"
//
//#define MAT_TYPE_WOOD_CRATE_HARD_NAME "wood_crate_scrape_rough_loop1"
//
//#define MAT_TYPE_WOOD_PLANK_LIGHT_NAME "wood_plank_scrape_light_loop1"
//
//#define MAT_TYPE_WOOD_PLANK_HARD_NAME "wood_plank_scrape_rough_loop1"
//
//#define MAT_TYPE_WOOD_BOX_LIGHT_NAME "wood_box_scrape_light_loop1"
//
//#define MAT_TYPE_WOOD_BOX_HARD_NAME "wood_box_scrape_rough_loop1"
//
//#define MAT_TYPE_PLASTIC_BARREL_LIGHT_NAME "plastic_barrel_scrape_smooth_loop1"
//
//#define MAT_TYPE_PLASTIC_BARREL_HARD_NAME "plastic_barrel_scrape_rough_loop1"
//
//#define MAT_TYPE_PLASTIC_BOX_LIGHT_NAME "plastic_box_scrape_smooth_loop1"
//
//#define MAT_TYPE_PLASTIC_BOX_HARD_NAME "plastic_box_scrape_rough_loop1"
//
//#pragma endregion


#include "GUI.h"

#include "SFML/Graphics.hpp"
#include "PixelParticleSystem.h"
#include "ParticleSystem.h"
#include "Animation.h"
#include "Object.h"
#include "SceneActor.h"
#include "SceneTile.h"
#include "ReverbObject.h"
#include "Decal.h"
#include "ObjectContactListener.h"
#include "SolidObject.h"
#include "ResourceHandling.h"
#include "State.h"
#include "PropPhysics.h"
#include "AnimationDataContainer.h"
#include "Game.h"
#include "MovingPawn.h"
#include "npc_moving_helper.h"
#include "npc_test_turret.h"
#include "Projectile.h"
#include "Item.h"
#include "Weapon.h"
#include "ItemContainer.h"
#include "Player.h"
#include "npc_zombie.h"
#include "State.h"
#include "TestProjectile.h"
#include "MenuState.h"
#include "PlayState.h"


#include <iostream>
#include <vector>
#include <functional>
#include <math.h>
#include <algorithm>

#include <Box2D.h>
#include <fmod.hpp>
#include <fmod_errors.h>

#ifndef TINYXML2_INCLUDED
#include <tinyxml2.h>
#include <tinyxml2.cpp>
#endif // !TINYXML2_INCLUDED



#ifndef XMLCheckResult
#define XMLCheckResult(a_eResult) if (a_eResult != XML_SUCCESS) { printf("Error: %i\n", a_eResult);/* return a_eResult;*/ }
#endif

#ifndef M_PI
#define M_PI           3.14159265358979323846  /* pi */
#endif // M_PI




////sf::Font font;
//sf::Texture solder;
//sf::Texture textBoxTexture1;
//sf::Texture proj;
////sf::Texture Officer1;
//sf::Texture blood1;
//sf::Texture blood_yellow_1;
//sf::Texture blood_a_anim;
//sf::Texture solder_rifle;

sf::IntRect ToIntRect(sf::FloatRect rect)
{
	sf::IntRect res;
	res.height = rect.height;
	res.width = rect.width;
	res.top = rect.top;
	res.left = rect.left;
	return res;
}

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



class State;
class npc_zombie;
class Player;


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
	game.Resources->AddTextureResource(new TextureResource("textBoxTexture1","./../textures/textbox.png" , false, false));
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

	game.Resources->AddSoundResource(new SoundResource("ambience_base", "./../sounds/ambience_base.wav"));
	game.Resources->AddSoundResource(new SoundResource("pistol_fire2", "./../sounds//pistol_fire2.wav"));
	game.Resources->AddSoundResource(new SoundResource("fire1", "./../sounds/fire1.wav"));

	game.Resources->AddSoundResource(new SoundResource(MAT_TYPE_PLASTIC_BOX_LIGHT_NAME, "./../sounds/physics/plastic/plastic_box_scrape_smooth_loop1.wav"));
	game.Resources->AddSoundResource(new SoundResource(MAT_TYPE_PLASTIC_BOX_HARD_NAME, "./../sounds/physics/plastic/plastic_box_scrape_rough_loop1.wav"));
	game.Resources->AddSoundResource(new SoundResource(MAT_TYPE_PLASTIC_BARREL_LIGHT_NAME, "./../sounds/physics/plastic/plastic_barrel_scrape_smooth_loop1.wav"));
	game.Resources->AddSoundResource(new SoundResource(MAT_TYPE_PLASTIC_BARREL_HARD_NAME, "./../sounds/physics/plastic/plastic_barrel_scrape_rough_loop1.wav"));

	game.Resources->AddSoundResource(new SoundResource(MAT_TYPE_WOOD_BOX_HARD_NAME, "./../sounds/physics/wood/wood_box_scrape_rough_loop1.wav"));
	game.Resources->AddSoundResource(new SoundResource(MAT_TYPE_WOOD_BOX_LIGHT_NAME, "./../sounds/physics/wood/wood_box_scrape_light_loop1.wav"));
	game.Resources->AddSoundResource(new SoundResource("wood_solid_scrape_rough_loop1", "./../sounds/physics/wood/wood_solid_scrape_rough_loop1.wav"));
	game.Resources->AddSoundResource(new SoundResource(MAT_TYPE_WOOD_CRATE_HARD_NAME, "./../sounds/physics/wood/wood_crate_scrape_rough_loop1.wav"));
	game.Resources->AddSoundResource(new SoundResource(MAT_TYPE_WOOD_PLANK_HARD_NAME, "./../sounds/physics/wood/wood_plank_scrape_rough_loop1.wav"));
	game.Resources->AddSoundResource(new SoundResource(MAT_TYPE_WOOD_PLANK_LIGHT_NAME, "./../sounds/physics/wood/wood_plank_scrape_light_loop1.wav"));

	game.Resources->AddSoundResource(new SoundResource("metal_box_scrape_rough_loop1", "./../sounds/physics/metal/metal_box_scrape_rough_loop1.wav"));
	game.Resources->AddSoundResource(new SoundResource("metal_box_scrape_rough_loop2", "./../sounds/physics/metal/metal_box_scrape_rough_loop2.wav"));
	game.Resources->AddSoundResource(new SoundResource("metal_box_scrape_smooth_loop1", "./../sounds/physics/metal/metal_box_scrape_smooth_loop1.wav"));

	
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