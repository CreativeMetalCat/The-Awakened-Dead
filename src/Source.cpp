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



	player = new Player(sf::Vector2f(0,0), 150, 100, sf::Sprite(solder->texture));
	/*player->animations->push_back(animr);*/
	player->Init();


	PlayState ps = PlayState("PlayState");
	MenuState ms = MenuState("MenuState");

	
	TextureResource *letterT = new TextureResource("letter", "./../textures/Letter.png", false, false);
	letterT->CreateResourceFromFile();

	Weapon*w=new Weapon("rifle",1.f,20.f);
	w->weaponType = WEAPON_TYPE_TAD_RIFLE;
	w->ammoPerClip = 40;
	w->ammoInTheClip = w->ammoPerClip;
	w->inaccuracy = 1.f;
	w->projectile_texture_name = "proj";
	w->shoot_sound_name = "rifle_single1";
	w->empty_clip_sound = "rifle_empty";
	w->reload_sound_name = "rifle_reload";
	w->weapon_ammo_type = AMMO_TYPE_RIFLE;
	w->sprite = sf::Sprite(letterT->texture);
	

	/*Weapon*w1 = new Weapon("pistol",1.f, 15.f);
	w1->weaponType = WEAPON_TYPE_TAD_PISTOL;
	w1->ammoPerClip = 17;
	w1->ammoInTheClip = w1->ammoPerClip;
	w1->projectile_texture_name = "proj";
	w1->shoot_sound_name = "pistol_fire2";
	w1->empty_clip_sound = "pistol_empty";
	w1->weapon_ammo_type = AMMO_TYPE_PISTOL;
	w1->sprite = sf::Sprite(letterT->texture);
	w1->reload_sound_name = "pistol_reload";*/

	Weapon*w2 = new Weapon("shotgun", 1.f, 20.f);
	w2->weaponType = WEAPON_TYPE_TAD_SHOTGUN;
	w2->ammoPerClip = 8;
	w2->ammoInTheClip = w2->ammoPerClip;
	w2->inaccuracy = 1.f;
	w2->bullets_per_shot = 4;
	w2->projectile_texture_name = "proj";
	w2->shoot_sound_name = "shotgun_fire6";
	w2->empty_clip_sound = "shotgun_empty";
	w2->reload_sound_name = "rifle_reload";
	w2->weapon_ammo_type = AMMO_TYPE_SHOTGUN;
	w2->sprite = sf::Sprite(letterT->texture);
	


	player->weapons->push_back(w);
	/*player->weapons->push_back(w1);*/
	player->weapons->push_back(w2);
	
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

	for (int i = 0; i < 3; i++)
	{
		game.Resources->AddTextureResource(new TextureResource("solder_rifle_shoot_" + std::to_string(i), "./../textures/Top_Down_Survivor/rifle/shoot/survivor-shoot_rifle_" + std::to_string(i) + ".png", false, false));
	}

	for (int i = 0; i < 3; i++)
	{
		game.Resources->AddTextureResource(new TextureResource("solder_pistol_shoot_" + std::to_string(i), "./../textures/Top_Down_Survivor/handgun/shoot/survivor-shoot_handgun_" + std::to_string(i) + ".png", false, false));
	}

	for (int i = 0; i < 3; i++)
	{
		game.Resources->AddTextureResource(new TextureResource("solder_shotgun_shoot_" + std::to_string(i), "./../textures/Top_Down_Survivor/shotgun/shoot/survivor-shoot_shotgun_" + std::to_string(i) + ".png", false, false));
	}

	for (int i = 0; i < 20; i++)
	{
		game.Resources->AddTextureResource(new TextureResource("solder_rifle_move_" + std::to_string(i), "./../textures/Top_Down_Survivor/rifle/move/survivor-move_rifle_" + std::to_string(i) + ".png", false, false));
	}

	for (int i = 0; i < 20; i++)
	{
		game.Resources->AddTextureResource(new TextureResource("solder_rifle_reload_" + std::to_string(i), "./../textures/Top_Down_Survivor/rifle/reload/survivor-reload_rifle_" + std::to_string(i) + ".png", false, false));
	}

	for (int i = 0; i < 20; i++)
	{
		game.Resources->AddTextureResource(new TextureResource("solder_pistol_move_" + std::to_string(i), "./../textures/Top_Down_Survivor/handgun/move/survivor-move_handgun_" + std::to_string(i) + ".png", false, false));
	}

	for (int i = 0; i < 15; i++)
	{
		game.Resources->AddTextureResource(new TextureResource("solder_pistol_reload_" + std::to_string(i), "./../textures/Top_Down_Survivor/handgun/reload/survivor-reload_handgun_" + std::to_string(i) + ".png", false, false));
	}

	for (int i = 0; i < 20; i++)
	{
		game.Resources->AddTextureResource(new TextureResource("solder_shotgun_reload_" + std::to_string(i), "./../textures/Top_Down_Survivor/shotgun/reload/survivor-reload_shotgun_" + std::to_string(i) + ".png", false, false));
	}

	for (int i = 0; i < 20; i++)
	{
		game.Resources->AddTextureResource(new TextureResource("solder_shotgun_move_" + std::to_string(i), "./../textures/Top_Down_Survivor/shotgun/move/survivor-move_shotgun_" + std::to_string(i) + ".png", false, false));
	}

	for (int i = 0; i < 17; i++)
	{
		game.Resources->AddTextureResource(new TextureResource("skeleton-idle_" + std::to_string(i), "./../textures/zombie/idle/skeleton-idle_" + std::to_string(i) + ".png", false, false));
	}

	for (int i = 0; i <9; i++)
	{
		game.Resources->AddTextureResource(new TextureResource("skeleton-attack_" + std::to_string(i), "./../textures/zombie/attack/skeleton-attack_" + std::to_string(i) + ".png", false, false));
	}

	for (int i = 0; i < 17; i++)
	{
		game.Resources->AddTextureResource(new TextureResource("skeleton-move_" + std::to_string(i), "./../textures/zombie/move/skeleton-move_" + std::to_string(i) + ".png", false, false));
	}

	sf::View view;
	game.Resources->AddTextureResource(new TextureResource("solder", "./../textures/player_idle.gif",false,false));
	game.Resources->AddTextureResource(new TextureResource("textBoxTexture1","./../textures/textbox.png" , false, false));
	game.Resources->AddTextureResource(new TextureResource("proj", "./../textures/projectile.png", false, false));
	game.Resources->AddTextureResource(new TextureResource("blood1", "./../textures/blood/blood_a_0002.png", false, false));
	game.Resources->AddTextureResource(new TextureResource("blood_splat_1", "./../textures/blood/blood_splat_1.png", false, false));

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
	game.Resources->AddSoundResource(new SoundResource("pistol_fire2", "./../sounds/weapons/pistol/pistol_fire2.wav"));
	game.Resources->AddSoundResource(new SoundResource("rifle_single1", "./../sounds/weapons/m4/single1.wav"));
	game.Resources->AddSoundResource(new SoundResource("shotgun_fire6", "./../sounds/weapons/shotgun/shotgun_fire6.wav"));

	game.Resources->AddSoundResource(new SoundResource("pistol_empty", "./../sounds/weapons/pistol/pistol_empty.wav"));
	game.Resources->AddSoundResource(new SoundResource("rifle_empty", "./../sounds/weapons/mp5/empty.wav"));
	game.Resources->AddSoundResource(new SoundResource("shotgun_empty", "./../sounds/weapons/shotgun/empty.wav"));

	game.Resources->AddSoundResource(new SoundResource("rifle_reload", "./../sounds/weapons/mp5/reload.wav"));
	game.Resources->AddSoundResource(new SoundResource("pistol_reload", "./../sounds/weapons/pistol/pistol_reload1.wav"));
	game.Resources->AddSoundResource(new SoundResource("shotgun_reload1", "./../sounds/weapons/shotgun/shotgun_reload1.wav"));
	game.Resources->AddSoundResource(new SoundResource("shotgun_reload2", "./../sounds/weapons/shotgun/shotgun_reload2.wav"));
	game.Resources->AddSoundResource(new SoundResource("shotgun_reload3", "./../sounds/weapons/shotgun/shotgun_reload1.wav"));

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

	for (int i = 1; i < 5; i++)
	{
		game.Resources->AddSoundResource(new SoundResource(std::string(MAT_TYPE_WOOD_IMPACT_BULLET_SOUND_NAME) + std::to_string(i), "./../sounds/physics/wood/wood_box_impact_bullet"+std::to_string(i)+".wav"));
	}
	for (int i = 1; i < 5; i++)
	{
		game.Resources->AddSoundResource(new SoundResource(std::string(MAT_TYPE_WOOD_CRATE_IMPACT_BULLET_SOUND_NAME) + std::to_string(i), "./../sounds/physics/wood/wood_box_impact_bullet" + std::to_string(i) + ".wav"));
	}
	for (int i = 1; i < 5; i++)
	{
		game.Resources->AddSoundResource(new SoundResource(std::string(MAT_TYPE_WOOD_PLANK_IMPACT_BULLET_SOUND_NAME) + std::to_string(i), "./../sounds/physics/wood/wood_box_impact_bullet" + std::to_string(i) + ".wav"));
	}
	for (int i = 1; i < 6; i++)
	{
		game.Resources->AddSoundResource(new SoundResource(std::string(MAT_TYPE_PLASTIC_BOX_IMPACT_BULLET_SOUND_NAME) + std::to_string(i), "./../sounds/physics/plastic/plastic_box_impact_bullet" + std::to_string(i) + ".wav"));
	}
	
	for (int i = 1; i < 4; i++)
	{
		game.Resources->AddSoundResource(new SoundResource(std::string(MAT_TYPE_PLASTIC_BARREL_IMPACT_BULLET_SOUND_NAME) + std::to_string(i), "./../sounds/physics/plastic/plastic_barrel_impact_bullet" + std::to_string(i) + ".wav"));
	}


	game.Resources->AddSoundResource(new SoundResource("metal_box_scrape_rough_loop1", "./../sounds/physics/metal/metal_box_scrape_rough_loop1.wav"));
	game.Resources->AddSoundResource(new SoundResource("metal_box_scrape_rough_loop2", "./../sounds/physics/metal/metal_box_scrape_rough_loop2.wav"));
	game.Resources->AddSoundResource(new SoundResource("metal_box_scrape_smooth_loop1", "./../sounds/physics/metal/metal_box_scrape_smooth_loop1.wav"));

	game.Resources->AddSoundResource(new SoundResource("zombie_pain", "./../sounds/zo_pain1.wav"));
	game.Resources->AddSoundResource(new SoundResource("zombie_attack1", "./../sounds/zo_attack1.wav"));
	game.Resources->AddSoundResource(new SoundResource("zombie_attack2", "./../sounds/zo_attack2.wav"));
	game.Resources->AddSoundResource(new SoundResource("zombie_foot1", "./../sounds/npc/zombie/foot1.wav"));
	game.Resources->AddSoundResource(new SoundResource("zombie_foot2", "./../sounds/npc/zombie/foot2.wav"));
	game.Resources->AddSoundResource(new SoundResource("zombie_foot3", "./../sounds/npc/zombie/foot3.wav"));


	game.Resources->AddSoundResource(new SoundResource("plastic_box_impact_bullet1", "./../sounds/physics/plastic/plastic_box_impact_bullet1.wav"));

	for (int i = 1; i < 5; i++)
	{
		std::string name = MAT_SOUND_TYPE_CONCRETE_NAME;
		game.Resources->AddSoundResource(new SoundResource(name + std::to_string(i), "./../sounds/player/footsteps/concrete" + std::to_string(i) + ".wav"));
	}
	for (int i = 1; i < 5; i++)
	{
		std::string name = MAT_SOUND_TYPE_DIRT_NAME;
		game.Resources->AddSoundResource(new SoundResource(name + std::to_string(i), "./../sounds/player/footsteps/dirt" + std::to_string(i) + ".wav"));
	}
	for (int i = 1; i < 5; i++)
	{
		std::string name = MAT_SOUND_TYPE_CHAINLINK_NAME;
		game.Resources->AddSoundResource(new SoundResource(name + std::to_string(i), "./../sounds/player/footsteps/chainlink" + std::to_string(i) + ".wav"));
	}
	for (int i = 1; i < 5; i++)
	{
		std::string name = MAT_SOUND_TYPE_DUCT_NAME;
		game.Resources->AddSoundResource(new SoundResource(name + std::to_string(i), "./../sounds/player/footsteps/duct" + std::to_string(i) + ".wav"));
	}
	for (int i = 1; i < 5; i++)
	{
		std::string name = MAT_SOUND_TYPE_GRASS_NAME;
		game.Resources->AddSoundResource(new SoundResource(name + std::to_string(i), "./../sounds/player/footsteps/grass" + std::to_string(i) + ".wav"));
	}
	for (int i = 1; i < 5; i++)
	{
		std::string name = MAT_SOUND_TYPE_GRAVEL_NAME;
		game.Resources->AddSoundResource(new SoundResource(name + std::to_string(i), "./../sounds/player/footsteps/gravel" + std::to_string(i) + ".wav"));
	}
	for (int i = 1; i < 5; i++)
	{
		std::string name = MAT_SOUND_TYPE_LADDER_NAME;
		game.Resources->AddSoundResource(new SoundResource(name + std::to_string(i), "./../sounds/player/footsteps/ladder" + std::to_string(i) + ".wav"));
	}
	for (int i = 1; i < 5; i++)
	{
		std::string name = MAT_SOUND_TYPE_METAL_NAME;
		game.Resources->AddSoundResource(new SoundResource(name + std::to_string(i), "./../sounds/player/footsteps/metal" + std::to_string(i) + ".wav"));
	}
	for (int i = 1; i < 5; i++)
	{
		std::string name = MAT_SOUND_TYPE_METALGRATE_NAME;
		game.Resources->AddSoundResource(new SoundResource(name + std::to_string(i), "./../sounds/player/footsteps/metalgrate" + std::to_string(i) + ".wav"));
	}
	for (int i = 1; i < 5; i++)
	{
		std::string name = MAT_SOUND_TYPE_MUD_NAME;
		game.Resources->AddSoundResource(new SoundResource(name + std::to_string(i), "./../sounds/player/footsteps/mud" + std::to_string(i) + ".wav"));
	}
	for (int i = 1; i < 5; i++)
	{
		std::string name = MAT_SOUND_TYPE_SAND_NAME;
		game.Resources->AddSoundResource(new SoundResource(name + std::to_string(i), "./../sounds/player/footsteps/sand" + std::to_string(i) + ".wav"));
	}
	for (int i = 1; i < 5; i++)
	{
		std::string name = MAT_SOUND_TYPE_SLOSH_NAME;
		game.Resources->AddSoundResource(new SoundResource(name + std::to_string(i), "./../sounds/player/footsteps/slosh" + std::to_string(i) + ".wav"));
	}
	for (int i = 1; i < 5; i++)
	{
		std::string name = MAT_SOUND_TYPE_TILE_NAME;
		game.Resources->AddSoundResource(new SoundResource(name + std::to_string(i), "./../sounds/player/footsteps/tile" + std::to_string(i) + ".wav"));
	}
	for (int i = 1; i < 5; i++)
	{
		std::string name = MAT_SOUND_TYPE_WADE_NAME;
		game.Resources->AddSoundResource(new SoundResource(name + std::to_string(i), "./../sounds/player/footsteps/wade" + std::to_string(i) + ".wav"));
	}
	for (int i = 1; i < 5; i++)
	{
		std::string name = MAT_SOUND_TYPE_WOOD_NAME;
		game.Resources->AddSoundResource(new SoundResource(name + std::to_string(i), "./../sounds/player/footsteps/wood" + std::to_string(i) + ".wav"));
	}
	for (int i = 1; i < 5; i++)
	{
		std::string name = MAT_SOUND_TYPE_WOODPANEL_NAME;
		game.Resources->AddSoundResource(new SoundResource(name + std::to_string(i), "./../sounds/player/footsteps/woodpanel" + std::to_string(i) + ".wav"));
	}

	
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

			XMLElement*tileProps = child->FirstChildElement("properties");

			if (tileProps != NULL)
			{
				int mat_sound_id = 0;
				for (tinyxml2::XMLElement* Prop = tileProps->FirstChildElement(); Prop != NULL; Prop = Prop->NextSiblingElement())
				{
					if (Prop->FindAttribute("name") != NULL)
					{
						std::string f = Prop->FindAttribute("name")->Value();
						if (f == "mat_sound_type")
						{
							mat_sound_id = Prop->FindAttribute("value")->IntValue();
						}
				
					}
				}
				game.Resources->addTileData(TileData(std::stoi(id), mat_sound_id));
			}
			game.Resources->AddTextureResource(new TextureResource(id, "./" + source, false, false));
			
		}
	}
		



	//end of loading resources for game from tad_tileset.tsx

	

	game.Resources->AddFontResource(new FontResource("calibri", "./../fonts/calibri.ttf"));
	game.Resources->AddFontResource(new FontResource("Calibri", "./../fonts/calibri.ttf"));
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