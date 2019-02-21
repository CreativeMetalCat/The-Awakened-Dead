#pragma once

#include "MaterialTypes.h"

#include "State.h"
#include "GUI.h"
#include "Player.h"
#include"npc_zombie.h"
#include "PixelParticleSystem.h"
#include "PixelParticleSystemObject.h"
#include "ObjectContactListener.h"
#include "SceneTile.h"
#include "ReverbObject.h"
#include "TestProjectile.h"
#include "SolidObject.h"
#include "TextObject.h"
#include "SoundSourceObject.h"
#include "trigger_change_area_id.h"
#include "ammo_object.h"
#include "weapon_pickup_object.h"
#include "ai_node.h"
#include "knife_attack_projectile.h"
#include "trigger_change_level.h"

#ifndef _RANDOM_
#include <random>
#endif // !_RANDOM_



#ifndef TINYXML2_INCLUDED
#include <tinyxml2.h>
#include <tinyxml2.cpp>
#endif // !TINYXML2_INCLUDED



#include <fmod.hpp>
#include <fmod_errors.h>

#ifndef XMLCheckResult
#define XMLCheckResult(a_eResult) if (a_eResult != XML_SUCCESS) { printf("Error: %i\n", a_eResult);/* return a_eResult;*/ }
#endif

#ifndef MAX_SOUND_CHANNELS_COUNT
#define MAX_SOUND_CHANNELS_COUNT 1024
#endif // !MAX_SOUND_CHANNELS_COUNT


bool DEBUG_DRAWCOLLISION = false;

bool DEBUG_DRAWREVERB = false;

const int SCREENWIDTH = 1080;
const int SCREENHEIGHT = 720;

//state(mode) when player & everything is moves
class PlayState :public State
{
protected:
	

	//Currently only for this state
	//min - min of range
	//max - max of range
	int m_get_random_number(int min, int max)
	{
		std::random_device rd; // obtain a random number from hardware
		std::mt19937 eng(rd()); // seed the generator
		std::uniform_int_distribution<> distr(min, max); // define the range

		return distr(eng);
	}

public:

	

	
	int test = 0;
	int Current_area_id = 0;
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

	/*FMOD::Sound *shoot;
	FMOD::Sound *shoot2;
	FMOD::Sound*ambience1;*/

	b2World world;
	/*FMOD::Channel *channel;*/
	/*std::vector<FMOD::Channel*>*Channels = new std::vector<FMOD::Channel*>(MAX_SOUND_CHANNELS_COUNT);*/
	std::vector<PixelParticleSystemObject>*pixelParticleSystems = new std::vector<PixelParticleSystemObject>();

	std::vector<FMOD::Reverb3D*>*reverbs = new std::vector<FMOD::Reverb3D*>();

	//creates physical body for object using given data
	//Data HAS to be created before calling
	void CreatePhysicsObject(Object*object, b2BodyDef bodyDef, b2FixtureDef fixture, bool addToStateObjectContainer, float mass = 1.f) override
	{
		MAT_TYPE mt = static_cast<MAT_TYPE>(2);

		b2MassData propMass;
		propMass.mass = mass;
		propMass.center = b2Vec2(object->GetObjectRectangle().left + object->GetObjectRectangle().width / 2, object->GetObjectRectangle().top + object->GetObjectRectangle().height / 2);

		world.Step(0, 0, 0);

		object->Init();
		object->body = world.CreateBody(&bodyDef);

		object->body->SetMassData(&propMass);
		object->body->CreateFixture(&fixture);

		object->body->SetUserData(object);

		object->physBodyInitialized = true;

		if (addToStateObjectContainer)
		{
			StateObjects->push_back(object);
		}
	}

	//uses Game::PlaySound
	void PlaySound(std::string name)override
	{
		context->game->PlaySound(name);
	}

	//uses Game::PlaySound
	//@param
	//channel_id - id of channel that was used for this sound 
	 void PlaySound(std::string name, int &channel_id) override
	{
		 context->game->PlaySound(name,channel_id);
	}

	//creates physical body for object using given data
	//Data HAS to be created before calling
	void CreatePhysicsObject(Object*&object, b2BodyDef bodyDef, std::vector<b2FixtureDef>*&fixtures, bool addToStateObjectContainer, float mass = 1.f)override
	{

		object->Init();
		object->body = world.CreateBody(&bodyDef);

		if (!fixtures->empty())
		{
			for (size_t i = 0; i < fixtures->size(); i++)
			{
				object->body->CreateFixture(&fixtures->at(i));
			}
		}
		object->body->SetUserData(object);

		if (addToStateObjectContainer)
		{
			StateObjects->push_back(object);
		}

	}

	void LoadMap(std::string name)
	{
		context->window->setFramerateLimit(100);

		world.Step(0, 0, 0);

		reverbs->clear();

		if (player->physBodyInitialized)
		{
			player->body->GetWorld()->DestroyBody(player->body);
		}
		


		for (size_t i = 0; i < StateObjects->size(); i++)
		{
			if (StateObjects->at(i)->physBodyInitialized == true)
			{
				StateObjects->at(i)->body->GetWorld()->DestroyBody(StateObjects->at(i)->body);
			}
			delete StateObjects->at(i);
		}

		StateObjects->clear();
		
		//loading resources for game from tad_tileset.tsx
		using namespace tinyxml2;
		XMLDocument doc;

		const char* mn = "./../maps/";

		char result[100];   // array to hold the result.

		strcpy(result, mn); // copy string one into the result.
		strcat(result, name.c_str()); // append string two to the result.

		doc.LoadFile(result);


		XMLElement* root = doc.FirstChildElement("map");

		//current tile id x
		int cTileIdx = 0;
		//current tile id y
		int cTileIdy = 0;

		int layerMaxWidth = 0;
		int layerMaxHeight = 0;


		for (tinyxml2::XMLElement* child = root->FirstChildElement(); child != NULL; child = child->NextSiblingElement())
		{
			if (child == nullptr) { XMLCheckResult(XML_ERROR_PARSING_ELEMENT); break; }
			std::string name = child->Name();
			if (name == "layer")
			{
				int layer_area_id = 0;

				XMLElement*tileProps = child->FirstChildElement("properties");

				if (tileProps != NULL)
				{
					for (tinyxml2::XMLElement* Prop = tileProps->FirstChildElement(); Prop != NULL; Prop = Prop->NextSiblingElement())
					{
						if (Prop->FindAttribute("name") != NULL)
						{
							std::string f = Prop->FindAttribute("name")->Value();
							if (f == "area_id")
							{
								layer_area_id = Prop->FindAttribute("value")->IntValue();
							}
						}
					}
				}
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

								int mat_sound_type_id = 0;
								try
								{
									TileData td = context->game->Resources->getTileDataById(gid);
									mat_sound_type_id = td.MatSoundtype;
								}
								catch (std::runtime_error e)
								{

								}

								sf::Vector2f pos = sf::Vector2f(64 * cTileIdx, 64 * cTileIdy);

								SceneTile*ta = new SceneTile
								(
									pos,
									sf::Sprite(context->game->Resources->getTextureResourceDataByName(std::to_string(gid))->texture),
									context->game->Resources->getTextureResourceDataByName(std::to_string(gid))->texture.getSize().x,
									context->game->Resources->getTextureResourceDataByName(std::to_string(gid))->texture.getSize().y,
									layer_area_id,
									mat_sound_type_id
								);
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

								int mat_sound_type_id = 0;
								try
								{
									TileData td = context->game->Resources->getTileDataById(gid);
									mat_sound_type_id = td.MatSoundtype;
								}
								catch (std::runtime_error e)
								{

								}


								sf::Vector2f pos = sf::Vector2f(64 * cTileIdx, 64 * cTileIdy);
								SceneTile*ta = new SceneTile
								(
									pos,
									sf::Sprite(context->game->Resources->getTextureResourceDataByName(std::to_string(gid))->texture),
									context->game->Resources->getTextureResourceDataByName(std::to_string(gid))->texture.getSize().x,
									context->game->Resources->getTextureResourceDataByName(std::to_string(gid))->texture.getSize().y,
									layer_area_id,
									mat_sound_type_id
								);
								ta->Init();
								StateObjects->push_back(ta);
							}
							cTileIdx++;
							if (cTileIdx >= layerMaxWidth)
							{
								cTileIdx = 0;
								cTileIdy++;
								if (cTileIdy > layerMaxHeight) { cTileIdy = 0; }
							}
						}

						cTileIdx = 0;
						cTileIdy = 0;
					}
				}

			}

			else if (name == "objectgroup")
			{
				int layer_area_id = 0;

				XMLElement*tileProps = child->FirstChildElement("properties");

				if (tileProps != NULL)
				{
					for (tinyxml2::XMLElement* Prop = tileProps->FirstChildElement(); Prop != NULL; Prop = Prop->NextSiblingElement())
					{
						if (Prop->FindAttribute("name") != NULL)
						{
							std::string f = Prop->FindAttribute("name")->Value();
							if (f == "area_id")
							{
								layer_area_id = Prop->FindAttribute("value")->IntValue();
							}
						}
					}
				}

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
							this->StateObjects->push_back(new SolidObject(sf::Vector2f(posX, posY), sf::Sprite(), width, height,layer_area_id));
						}

						if (type == "trigger_change_area")
						{
							float posX = obj->FindAttribute("x")->FloatValue();
							float posY = obj->FindAttribute("y")->FloatValue();
							float width = obj->FindAttribute("width")->FloatValue();
							float height = obj->FindAttribute("height")->FloatValue();

							int area_to_change_to = 0;
							XMLElement*objProps = obj->FirstChildElement("properties");

							if (objProps != NULL)
							{
								for (tinyxml2::XMLElement* Prop = objProps->FirstChildElement(); Prop != NULL; Prop = Prop->NextSiblingElement())
								{
									if (Prop->FindAttribute("name") != NULL)
									{
										std::string f = Prop->FindAttribute("name")->Value();
										if (f == "area_to_change_to")
										{
											area_to_change_to = Prop->FindAttribute("value")->IntValue();
										}
									}
								}
							}
							this->StateObjects->push_back(new trigger_change_area_id(sf::Vector2f(posX,posY),width,height,area_to_change_to,layer_area_id));
						}
						if (type == "SoundSource")
						{
							float posX = obj->FindAttribute("x")->FloatValue();
							float posY = obj->FindAttribute("y")->FloatValue();

							int min = 100;
							int max = 1000;

							std::string sound_filename = "";
							std::string sound_name = "";

							bool is_looped = false;

							XMLElement*objProps = obj->FirstChildElement("properties");

							if (objProps != NULL)
							{
								for (tinyxml2::XMLElement* Prop = objProps->FirstChildElement(); Prop != NULL; Prop = Prop->NextSiblingElement())
								{
									if (Prop->FindAttribute("name") != NULL)
									{
										std::string f = Prop->FindAttribute("name")->Value();
										if (f == "MaxDistance")
										{
											max = Prop->FindAttribute("value")->IntValue();
										}
										if (f == "MinDistance")
										{
											min = Prop->FindAttribute("value")->IntValue();
										}
										if (f == "SoundFile")
										{
											sound_filename = Prop->FindAttribute("value")->Value();
										}
										if (f == "SoundName")
										{
											sound_name = Prop->FindAttribute("value")->Value();
										}
										if (f == "Looped")
										{
											is_looped = Prop->FindAttribute("value")->BoolValue();
										}
										
									}
								}
							}

							if (sound_name != "")
							{
								try
								{
									context->game->Resources->getSoundResourceDataByName(sound_name);
								}
								catch (std::runtime_error e)
								{
									if (sound_filename != "")
									{

										SoundResource* sr = new SoundResource(sound_name, "./" + sound_filename);

										

										FMOD_RESULT res;
										res = context->game->lowSoundSystem->createSound(sr->filename.c_str(), FMOD_3D_LINEARROLLOFF, 0, &sr->sound);
										if (res != FMOD_OK)
										{
											std::cout << "Error creating sound. Name: " << sr->name.c_str() << "Filename: " << sr->filename.c_str() << "Error: " << FMOD_ErrorString(res) << std::endl;
										}
										else
										{
											context->game->Resources->AddSoundResource(sr);
										}
									}
								}
								
							}
							
							this->StateObjects->push_back(new SoundSourceObject(sf::Vector2f(posX,posY),sound_name, is_looped,max,min,layer_area_id));
						}
						if (type == "TextObject")
						{
							float posX = obj->FindAttribute("x")->FloatValue();
							float posY = obj->FindAttribute("y")->FloatValue();
							float width = obj->FindAttribute("width")->FloatValue();
							float height = obj->FindAttribute("height")->FloatValue();

							int r = 255;
							int g = 255;
							int b = 255;
							int a = 255;
							
							std::string text; 
							std::string fontName="calibri";
							int pixelSize = 25;
							sf::Color color = sf::Color::Black;
							
							if (obj->FirstChildElement("text") != NULL)
							{
								XMLElement*textProps = obj->FirstChildElement("text");

								if (textProps->FindAttribute("fontfamily") != NULL)
								{
									fontName = textProps->FindAttribute("fontfamily")->Value();
								}
								if (textProps->FindAttribute("pixelsize") != NULL)
								{
									pixelSize = textProps->FindAttribute("pixelsize")->IntValue();
								}
								text = textProps->GetText();
							}
							XMLElement*objProps = obj->FirstChildElement("properties");

							if (objProps != NULL)
							{
								for (tinyxml2::XMLElement* Prop = objProps->FirstChildElement(); Prop != NULL; Prop = Prop->NextSiblingElement())
								{
									if (Prop->FindAttribute("name") != NULL)
									{
										std::string f = Prop->FindAttribute("name")->Value();
										if (f == "Red")
										{
											r = Prop->FindAttribute("value")->IntValue();
										}
										if (f == "Green")
										{
											g = Prop->FindAttribute("value")->IntValue();
										}
										if (f == "Blue")
										{
											b = Prop->FindAttribute("value")->IntValue();
										}
										if (f == "Alpha")
										{
											a = Prop->FindAttribute("value")->IntValue();
										}
									}
								}
							}
							
							color = sf::Color(r, g, b, a);
							this->StateObjects->push_back(new TextObject(sf::Vector2f(posX,posY),text,color,sf::Text(text,context->game->Resources->getFontResourceDataByName(fontName)->font, pixelSize),true,layer_area_id));
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


								int gid = 1;
								if (objData->FindAttribute("gid") != NULL)
								{
									gid = objData->FindAttribute("gid")->IntValue();
								}
								gid--;
								if (type == "ai_node")
								{
									float width = 0;
									float height = 0;
									float posX = obj->FindAttribute("x")->FloatValue();
									float posY = obj->FindAttribute("y")->FloatValue();
									if (obj->FindAttribute("width") == NULL)
									{
										width = objData->FindAttribute("width")->FloatValue();
									}
									else
									{
										width = obj->FindAttribute("width")->FloatValue();
									}

									if (obj->FindAttribute("height") == NULL)
									{
										height = objData->FindAttribute("height")->FloatValue();
									}
									else
									{
										height = obj->FindAttribute("height")->FloatValue();
									}

									std::string name = obj->FindAttribute("name")->Value();
									XMLElement*props = obj->FirstChildElement("properties");

									std::string next_name ="";

									if (props != NULL)
									{
										for (tinyxml2::XMLElement* prop = props->FirstChildElement(); prop != NULL; prop = prop->NextSiblingElement())
										{

											if (prop->FindAttribute("name") != NULL)
											{
												std::string f = prop->FindAttribute("name")->Value();
												if (f == "Next")
												{
													next_name = prop->FindAttribute("value")->Value();
												}
											}
										}
									}


									

									ai_node*n = new ai_node({ posX,posY }, name, 50.f, 50.f, width, height, layer_area_id);

									if (next_name != "")
									{
										n->next_name = next_name;
									}
									this->StateObjects->push_back(n);
								}
								if (type == "info_player_start")
								{
									float posX = obj->FindAttribute("x")->FloatValue();
									float posY = obj->FindAttribute("y")->FloatValue();
									this->player->SetObjectPosition(sf::Vector2f(posX, posY));
								}
								else if (type == "TriggerChangeLevel")
								{
									float posX = obj->FindAttribute("x")->FloatValue();
									float posY = obj->FindAttribute("y")->FloatValue();

									float width = 0;
									float height = 0;

									if (obj->FindAttribute("y") != NULL)
									{
										width = obj->FindAttribute("width")->FloatValue();
									}
									else
									{
										width = objData->FindAttribute("width")->FloatValue();
									}
									if (obj->FindAttribute("x") != NULL)
									{
										height = obj->FindAttribute("height")->FloatValue();
									}
									else
									{
										height=objData->FindAttribute("height")->FloatValue();
									}
									std::string lvl_name = "";

									XMLElement*props = obj->FirstChildElement("properties");

									if (props != NULL)
									{
										for (tinyxml2::XMLElement* prop = props->FirstChildElement(); prop != NULL; prop = prop->NextSiblingElement())
										{

											if (prop->FindAttribute("name") != NULL)
											{
												std::string f = prop->FindAttribute("name")->Value();
												if (f == "level_name")
												{
													lvl_name = prop->FindAttribute("value")->Value();
												}
											}
										}
									}

									this->StateObjects->push_back(new TriggerChangeLevel({ posX,posY }, lvl_name + ".tmx", width, height, layer_area_id));
								}
								if (type == "npc_zombie")
								{
									float posX = obj->FindAttribute("x")->FloatValue();
									float posY = obj->FindAttribute("y")->FloatValue();

									float health = 100.f;
									float speed = 0.01f;

									XMLElement*objProps = objData->FirstChildElement("properties");

									if (objProps != NULL)
									{
										for (tinyxml2::XMLElement* Prop = objProps->FirstChildElement(); Prop != NULL; Prop = Prop->NextSiblingElement())
										{

											if (Prop->FindAttribute("name") != NULL)
											{
												std::string f = Prop->FindAttribute("name")->Value();
												if (f == "health")
												{
													health = Prop->FindAttribute("value")->FloatValue();
												}
												else if (f == "speed")
												{
													speed = Prop->FindAttribute("value")->FloatValue();
												}
											}
										}
									}
									this->StateObjects->push_back(new npc_zombie(sf::Vector2f(posX, posY), speed, 100, 100,layer_area_id));



								}
								if (type == "SoundReverbPreset")
								{
									float width;
									float height;
									FMOD_REVERB_PROPERTIES props = FMOD_PRESET_OFF;
									float posX = obj->FindAttribute("x")->FloatValue();
									float posY = obj->FindAttribute("y")->FloatValue();
									if (obj->FindAttribute("width") != NULL)
									{
										width = obj->FindAttribute("width")->FloatValue();
									}
									else
									{
										width = objData->FindAttribute("width")->FloatValue();
									}

									if (obj->FindAttribute("height") != NULL)
									{
										height = obj->FindAttribute("height")->FloatValue();
									}
									else
									{
										height = objData->FindAttribute("height")->FloatValue();
									}

									XMLElement*objProps = objData->FirstChildElement("properties");

									if (objProps != NULL)
									{
										for (tinyxml2::XMLElement* Prop = objProps->FirstChildElement(); Prop != NULL; Prop = Prop->NextSiblingElement())
										{

											if (Prop->FindAttribute("name") != NULL)
											{
												std::string f = Prop->FindAttribute("name")->Value();
												if (f == "PresetName")
												{
													std::string reverbPreset = Prop->FindAttribute("value")->Value();

													if (reverbPreset == "Off")
													{
														props = FMOD_PRESET_OFF;
													}
													if (reverbPreset == "Generic")
													{
														props = FMOD_PRESET_GENERIC;
													}
													if (reverbPreset == "PaddedCell")
													{
														props = FMOD_PRESET_PADDEDCELL;
													}
													if (reverbPreset == "Room")
													{
														props = FMOD_PRESET_ROOM;
													}
													if (reverbPreset == "Bathroom")
													{
														props = FMOD_PRESET_BATHROOM;
													}
													if (reverbPreset == "LivingRoom")
													{
														props = FMOD_PRESET_LIVINGROOM;
													}
													if (reverbPreset == "StoneRoom")
													{
														props = FMOD_PRESET_STONEROOM;
													}
													if (reverbPreset == "Auditorium")
													{
														props = FMOD_PRESET_AUDITORIUM;
													}
													if (reverbPreset == "ConcertHall")
													{
														props = FMOD_PRESET_CONCERTHALL;
													}
													if (reverbPreset == "Cave")
													{
														props = FMOD_PRESET_CAVE;
													}
													if (reverbPreset == "Arena")
													{
														props = FMOD_PRESET_ARENA;
													}
													if (reverbPreset == "Hangar")
													{
														props = FMOD_PRESET_HANGAR;
													}
													if (reverbPreset == "CarpettedHallway")
													{
														props = FMOD_PRESET_CARPETTEDHALLWAY;
													}
													if (reverbPreset == "Hallway")
													{
														props = FMOD_PRESET_HALLWAY;
													}
													if (reverbPreset == "StoneCorridor")
													{
														props = FMOD_PRESET_STONECORRIDOR;
													}
													if (reverbPreset == "Alley")
													{
														props = FMOD_PRESET_ALLEY;
													}
													if (reverbPreset == "Forest")
													{
														props = FMOD_PRESET_FOREST;
													}
													if (reverbPreset == "City")
													{
														props = FMOD_PRESET_CITY;
													}
													if (reverbPreset == "Mountains")
													{
														props = FMOD_PRESET_MOUNTAINS;
													}
													if (reverbPreset == "Quarry")
													{
														props = FMOD_PRESET_QUARRY;
													}
													if (reverbPreset == "Plain")
													{
														props = FMOD_PRESET_PLAIN;
													}
													if (reverbPreset == "ParkingLot")
													{
														props = FMOD_PRESET_PARKINGLOT;
													}
													if (reverbPreset == "SewerPipe")
													{
														props = FMOD_PRESET_SEWERPIPE;
													}
													if (reverbPreset == "Underwater")
													{
														props = FMOD_PRESET_UNDERWATER;
													}
												}

											}

										}
									}
									ReverbObject *Reverb = new ReverbObject(sf::Vector3f(posX + width / 2, -posY - height / 2, 0), width, width + width / 10.f, props);
									Reverb->createReverb(context->game->lowSoundSystem);
									reverbs->push_back(Reverb->reverb);
								}
								if (type == "SoundReverb")
								{
									float width;
									float height;

									float decay = 0;
									float Densty = 0;
									float Diffus = 0;
									float elmix = 0;
									float earlyDly = 0;
									float hfref = 0;
									float hfdecay = 0;
									float hicut = 0;
									float latedly = 0;
									float lofreg = 0;
									float logain = 0;
									float wetlvl = 0.f;

									float posX = obj->FindAttribute("x")->FloatValue();
									float posY = obj->FindAttribute("y")->FloatValue();
									if (obj->FindAttribute("width") != NULL)
									{
										width = obj->FindAttribute("width")->FloatValue();
									}
									else
									{
										width = objData->FindAttribute("width")->FloatValue();
									}

									if (obj->FindAttribute("height") != NULL)
									{
										height = obj->FindAttribute("height")->FloatValue();
									}
									else
									{
										height = objData->FindAttribute("height")->FloatValue();
									}

									XMLElement*objProps = obj->FirstChildElement("properties");




									if (objProps != NULL)
									{
										for (tinyxml2::XMLElement* Prop = objProps->FirstChildElement(); Prop != NULL; Prop = Prop->NextSiblingElement())
										{

											if (Prop->FindAttribute("name") != NULL)
											{
												std::string f = Prop->FindAttribute("name")->Value();

												if (f == "Decay")
												{
													decay = Prop->FindAttribute("value")->FloatValue();
												}
												if (f == "Densty")
												{
													Densty = Prop->FindAttribute("value")->FloatValue();
												}
												if (f == "Diffus")
												{
													Diffus = Prop->FindAttribute("value")->FloatValue();
												}
												if (f == "E/L-Mix")
												{
													elmix = Prop->FindAttribute("value")->FloatValue();
												}
												if (f == "EarlyDly")
												{
													earlyDly = Prop->FindAttribute("value")->FloatValue();
												}
												if (f == "HFRef")
												{
													hfref = Prop->FindAttribute("value")->FloatValue();
												}
												if (f == "HFDecay")
												{
													hfdecay = Prop->FindAttribute("value")->FloatValue();
												}
												if (f == "HiCut")
												{
													hicut = Prop->FindAttribute("value")->FloatValue();
												}
												if (f == "LateDly")
												{
													latedly = Prop->FindAttribute("value")->FloatValue();
												}
												if (f == "LoFreq")
												{
													lofreg = Prop->FindAttribute("value")->FloatValue();
												}
												if (f == "LoGain")
												{
													logain = Prop->FindAttribute("value")->FloatValue();
												}
												if (f == "WetLvl")
												{
													wetlvl = Prop->FindAttribute("value")->FloatValue();
												}
											}

										}
									}
									ReverbObject *Reverb = new ReverbObject(sf::Vector3f(posX + width / 2, -posY - height / 2, 0), width, width + width / 10.f, { decay,earlyDly,latedly,hfref,hfdecay,Diffus,Densty,lofreg,logain,hicut,elmix,wetlvl });
									Reverb->createReverb(context->game->lowSoundSystem);
									reverbs->push_back(Reverb->reverb);
								}
								if (type == "solid_object")
								{
									float posX = obj->FindAttribute("x")->FloatValue();
									float posY = obj->FindAttribute("y")->FloatValue();
									float width = objData->FindAttribute("width")->FloatValue();
									float height = objData->FindAttribute("height")->FloatValue();
									this->StateObjects->push_back(new SolidObject(sf::Vector2f(posX, posY), sf::Sprite(), width, height,layer_area_id));
								}
								if (type == "PropPhysics")
								{
									float posX = obj->FindAttribute("x")->FloatValue();
									float posY = obj->FindAttribute("y")->FloatValue();
									float width = objData->FindAttribute("width")->FloatValue();
									float height = objData->FindAttribute("height")->FloatValue();

									XMLElement*props = objData->FirstChildElement("properties");
									float mass = 1.f;
									int mat_type = 0;
									bool massDepend = true;
									if (props != NULL)
									{
										for (tinyxml2::XMLElement* prop = props->FirstChildElement(); prop != NULL; prop = prop->NextSiblingElement())
										{

											if (prop->FindAttribute("name") != NULL)
											{
												std::string f = prop->FindAttribute("name")->Value();
												if (f == "mass")
												{
													mass = prop->FindAttribute("value")->FloatValue();
												}
												f = prop->FindAttribute("name")->Value();
												if (f == "mat_type")
												{
													mat_type = prop->FindAttribute("value")->IntValue();
												}
												f = prop->FindAttribute("name")->Value();
												if (f == "sounddependsonmass")
												{
													massDepend = prop->FindAttribute("value")->BoolValue();
												}
											}

										}
									}
									this->StateObjects->push_back(new PropPhysics(sf::Vector2f(posX, posY), sf::Sprite(context->game->Resources->getTextureResourceDataByName(std::to_string(gid))->texture), width, height, mass, mat_type, massDepend,layer_area_id));
								}

								if (type == "TextObject")
								{

									int r = 255;
									int g = 255;
									int b = 255;
									int a = 255;

									std::string text;
									std::string fontName = "calibri";
									int pixelSize = 25;
									sf::Color color = sf::Color::Black;

									float posX = 0.f;
									float posY = 0.f;
									float width = 0.f;
									float height = 0.f;
									posX = obj->FindAttribute("x")->FloatValue();
									posY = obj->FindAttribute("y")->FloatValue();
									if (obj->FindAttribute("width") != NULL)
									{
										float width = obj->FindAttribute("width")->FloatValue();
									}
									else
									{
										float width = objData->FindAttribute("width")->FloatValue();
									}

									if (obj->FindAttribute("height") != NULL)
									{
										float height = obj->FindAttribute("height")->FloatValue();
									}
									else
									{
										float height = objData->FindAttribute("height")->FloatValue();
									}

									if (objData->FirstChildElement("text") != NULL)
									{
										XMLElement*textData = objData->FirstChildElement("text");

										if (obj->FirstChildElement("text") != NULL)
										{
											XMLElement*textProps = obj->FirstChildElement("text");

											if (textProps->FindAttribute("fontfamily") != NULL)
											{
												fontName = textProps->FindAttribute("fontfamily")->Value();
											}
											else if (textData->FindAttribute("fontfamily") != NULL)
											{
												fontName = textData->FindAttribute("fontfamily")->Value();
											}

											if (textProps->FindAttribute("pixelsize") != NULL)
											{
												pixelSize = textProps->FindAttribute("pixelsize")->IntValue();
											}
											else if (textData->FindAttribute("pixelsize") != NULL)
											{
												pixelSize = textData->FindAttribute("pixelsize")->IntValue();
											}

											text = textProps->GetText();
										}

									}
									else
									{
										if (obj->FirstChildElement("text") != NULL)
										{
											XMLElement*textProps = obj->FirstChildElement("text");

											if (textProps->FindAttribute("fontfamily") != NULL)
											{
												fontName = textProps->FindAttribute("fontfamily")->Value();
											}
											if (textProps->FindAttribute("pixelsize") != NULL)
											{
												pixelSize = textProps->FindAttribute("pixelsize")->IntValue();
											}
											text = textProps->GetText();
										}
									}


									
									XMLElement*objProps = obj->FirstChildElement("properties");

									if (objProps != NULL)
									{
										for (tinyxml2::XMLElement* Prop = objProps->FirstChildElement(); Prop != NULL; Prop = Prop->NextSiblingElement())
										{
										if (Prop->FindAttribute("name") != NULL)
										{
											std::string f = Prop->FindAttribute("name")->Value();
											if (f == "Red")
											{
												r = Prop->FindAttribute("value")->IntValue();
											}
											if (f == "Green")
											{
												g = Prop->FindAttribute("value")->IntValue();
											}
											if (f == "Blue")
											{
												b = Prop->FindAttribute("value")->IntValue();
											}
											if (f == "Alpha")
											{
												a = Prop->FindAttribute("value")->IntValue();
											}
										}
										}
									}

									color = sf::Color(r, g, b, a);
									this->StateObjects->push_back(new TextObject(sf::Vector2f(posX, posY), text, color, sf::Text(text, context->game->Resources->getFontResourceDataByName(fontName)->font, pixelSize), true, layer_area_id));
								}
							}
						}
					}
				}

			}
		}
		//end of loading resources for game from tad_tileset.tsx

		b2Filter filter;
		filter.categoryBits = 0x1;


		ammo_pickup_object*apo = new ammo_pickup_object({ static_cast<int>(AMMO_TYPE_SHOTGUN),8 },sf::Vector2f(0,0), sf::Sprite(context->game->Resources->getTextureResourceDataByName("shotgun_ammopack_big")->texture),32,20, 0);
		apo->OnCollision = [this, apo](Object*object, b2Fixture *fixtureA, b2Fixture *fixtureB)
		{
			apo->onCollision(object, fixtureA, fixtureB,this->context,this->Name);
		};

		apo->LeftCollision = [this, apo](Object*object, b2Fixture *fixtureA, b2Fixture *fixtureB)
		{
			apo->leftCollision(object, fixtureA, fixtureB, this->context, this->Name);
		};

		this->StateObjects->push_back(apo);



		Weapon*w1 = new Weapon("Pistol", 1.2f, 15.f);
		w1->weaponType = WEAPON_TYPE_TAD_PISTOL;
		w1->ammoPerClip = 17;
		w1->ammoInTheClip = w1->ammoPerClip;
		w1->projectile_texture_name = "proj";
		w1->shoot_sound_name = "pistol_fire2";
		w1->empty_clip_sound = "pistol_empty";
		w1->weapon_ammo_type = AMMO_TYPE_PISTOL;
		w1->sprite = sf::Sprite();
		w1->reload_sound_name = "pistol_reload";

		weapon_pickup_object*wpo= new weapon_pickup_object(w1, sf::Vector2f(100, 100), sf::Sprite(), 50, 10,true,0);
		wpo->OnCollision = [this, wpo](Object*object, b2Fixture *fixtureA, b2Fixture *fixtureB)
		{
			wpo->onCollision(object, fixtureA, fixtureB, this->context, this->Name);
		};

		wpo->LeftCollision = [this, wpo](Object*object, b2Fixture *fixtureA, b2Fixture *fixtureB)
		{
			wpo->leftCollision(object, fixtureA, fixtureB, this->context, this->Name);
		};

		this->StateObjects->push_back(wpo);

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
				else if (SceneTile*obj = dynamic_cast<SceneTile*>(StateObjects->at(i)))
				{
					b2BodyDef def;
					def.position.Set(StateObjects->at(i)->GetObjectPosition().x + StateObjects->at(i)->GetObjectRectangle().width / 2, StateObjects->at(i)->GetObjectPosition().y + StateObjects->at(i)->GetObjectRectangle().height / 2);
					def.type = b2BodyType::b2_staticBody;

					StateObjects->at(i)->body = world.CreateBody(&def);

					b2PolygonShape shape;
					shape.SetAsBox(StateObjects->at(i)->GetObjectRectangle().width / 2, StateObjects->at(i)->GetObjectRectangle().height / 2);

					b2FixtureDef TriggerFixture;
					TriggerFixture.filter = filter;
					TriggerFixture.density = 0.f;
					TriggerFixture.shape = &shape;
					TriggerFixture.isSensor = 1;

					StateObjects->at(i)->body->CreateFixture(&TriggerFixture);
					StateObjects->at(i)->body->SetUserData(StateObjects->at(i));

					StateObjects->at(i)->physBodyInitialized = true;
					StateObjects->at(i)->bodyIsSensor = TriggerFixture.isSensor;

					obj->OnCollision = [this, obj](Object*object, b2Fixture *fixtureA, b2Fixture *fixtureB)
					{
						if (Player*p = dynamic_cast<Player*>(object))
						{
							if (p->footstep_sound_type != obj->mat_sound_type_id)
							{
								p->footstep_sound_type = obj->mat_sound_type_id;
							}
						}

						if (npc_zombie_base*p = dynamic_cast<npc_zombie_base*>(object))
						{
							if (p->footstep_sound_type != obj->mat_sound_type_id)
							{
								p->footstep_sound_type = obj->mat_sound_type_id;
							}
						}
					};
				}
				else if (dynamic_cast<TextObject*>(StateObjects->at(i)))
				{

				}

				else if (SoundSourceObject*sso = dynamic_cast<SoundSourceObject*>(StateObjects->at(i)))
				{
					b2BodyDef def;
					def.position.Set(StateObjects->at(i)->GetObjectPosition().x + StateObjects->at(i)->GetObjectRectangle().width / 2, StateObjects->at(i)->GetObjectPosition().y + StateObjects->at(i)->GetObjectRectangle().height / 2);
					def.type = b2BodyType::b2_staticBody;

					StateObjects->at(i)->body = world.CreateBody(&def);

					b2PolygonShape shape;
					shape.SetAsBox(sso->max_distance , sso->max_distance );

					b2FixtureDef TriggerFixture;
					TriggerFixture.filter = filter;
					TriggerFixture.density = 0.f;
					TriggerFixture.shape = &shape;
					TriggerFixture.isSensor = 1;

					StateObjects->at(i)->body->CreateFixture(&TriggerFixture);
					StateObjects->at(i)->body->SetUserData(StateObjects->at(i));

					StateObjects->at(i)->physBodyInitialized = true;
					StateObjects->at(i)->bodyIsSensor = TriggerFixture.isSensor;

					StateObjects->at(i)->OnCollision = [this,sso](Object*object, b2Fixture *fixtureA, b2Fixture *fixtureB)
					{
						sso->onCollision(object, fixtureA, fixtureB);
					};

					StateObjects->at(i)->LeftCollision = [this,sso](Object*object, b2Fixture *fixtureA, b2Fixture *fixtureB)
					{
						sso->leftCollision(object,fixtureA,fixtureB);
					};

				}
				else if (TriggerChangeLevel*tcl = dynamic_cast<TriggerChangeLevel*>(StateObjects->at(i)))
				{
					b2BodyDef def;
					def.position.Set(StateObjects->at(i)->GetObjectPosition().x + StateObjects->at(i)->GetObjectRectangle().width / 2, StateObjects->at(i)->GetObjectPosition().y + StateObjects->at(i)->GetObjectRectangle().height / 2);
					def.type = b2BodyType::b2_staticBody;

					StateObjects->at(i)->body = world.CreateBody(&def);

					b2PolygonShape shape;
					shape.SetAsBox(StateObjects->at(i)->GetObjectRectangle().width / 2, StateObjects->at(i)->GetObjectRectangle().height / 2);

					b2FixtureDef TriggerFixture;
					TriggerFixture.filter = filter;
					TriggerFixture.density = 0.f;
					TriggerFixture.shape = &shape;
					TriggerFixture.isSensor = 1;

					StateObjects->at(i)->body->CreateFixture(&TriggerFixture);
					StateObjects->at(i)->body->SetUserData(StateObjects->at(i));

					StateObjects->at(i)->physBodyInitialized = true;
					StateObjects->at(i)->bodyIsSensor = TriggerFixture.isSensor;

					StateObjects->at(i)->OnCollision = [this, tcl](Object*object, b2Fixture *fixtureA, b2Fixture *fixtureB)
					{
						tcl->onCollision(object, fixtureA, fixtureB,this->context,this->Name);
					};

					StateObjects->at(i)->LeftCollision = [this, tcl](Object*object, b2Fixture *fixtureA, b2Fixture *fixtureB)
					{
						tcl->leftCollision(object, fixtureA, fixtureB);
					};
				}
				else if (trigger_change_area_id* tcai = dynamic_cast<trigger_change_area_id*>(StateObjects->at(i)))
				{
					b2BodyDef def;
					def.position.Set(StateObjects->at(i)->GetObjectPosition().x + StateObjects->at(i)->GetObjectRectangle().width / 2, StateObjects->at(i)->GetObjectPosition().y + StateObjects->at(i)->GetObjectRectangle().height / 2);
					def.type = b2BodyType::b2_staticBody;

					StateObjects->at(i)->body = world.CreateBody(&def);

					b2PolygonShape shape;
					shape.SetAsBox(StateObjects->at(i)->GetObjectRectangle().width / 2, StateObjects->at(i)->GetObjectRectangle().height / 2);

					b2FixtureDef TriggerFixture;
					TriggerFixture.filter = filter;
					TriggerFixture.density = 0.f;
					TriggerFixture.shape = &shape;
					TriggerFixture.isSensor = 1;

					StateObjects->at(i)->body->CreateFixture(&TriggerFixture);
					StateObjects->at(i)->body->SetUserData(StateObjects->at(i));

					StateObjects->at(i)->physBodyInitialized = true;
					StateObjects->at(i)->bodyIsSensor = TriggerFixture.isSensor;

					StateObjects->at(i)->OnCollision = [this, tcai](Object*object, b2Fixture *fixtureA, b2Fixture *fixtureB)
					{
						tcai->onCollision(object, fixtureA, fixtureB);
					};

					StateObjects->at(i)->LeftCollision = [this, tcai](Object*object, b2Fixture *fixtureA, b2Fixture *fixtureB)
					{
						tcai->leftCollision(object, fixtureA, fixtureB);
					};
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
					senseShape.SetAsBox(StateObjects->at(i)->GetObjectRectangle().width*2, StateObjects->at(i)->GetObjectRectangle().height*2);

					b2FixtureDef TriggerFixture;
					TriggerFixture.filter = filter;
					TriggerFixture.density = 0.f;
					TriggerFixture.shape = &shape;
					TriggerFixture.isSensor = 0;
					TriggerFixture.userData = new FixtureData(FixtureActionType::Collision);
					

					b2FixtureDef senceFixture;
					senceFixture.filter = filter;
					senceFixture.density = 0.f;
					senceFixture.shape = &senseShape;
					senceFixture.isSensor = 1;
					senceFixture.userData = new FixtureData(FixtureActionType::Sense);

					StateObjects->at(i)->body->CreateFixture(&TriggerFixture);
					StateObjects->at(i)->body->CreateFixture(&senceFixture);
					StateObjects->at(i)->body->SetUserData(StateObjects->at(i));

					StateObjects->at(i)->physBodyInitialized = true;
					//is set by main fixture and/or purpose of the object itself
					StateObjects->at(i)->bodyIsSensor = TriggerFixture.isSensor;

					

					z->OnCollision = [this, z](Object*object, b2Fixture *fixtureA, b2Fixture *fixtureB)
					{
						z->onCollision(object, fixtureA, fixtureB, this->context, "PlayState");
					};

					z->LeftCollision = [this, z](Object*object, b2Fixture *fixtureA, b2Fixture *fixtureB)
					{
						z->leftCollision(object, fixtureA, fixtureB, this->context, "PlayState");
					};


					Animation::SpritesAnimation*zombie1_idle = new  Animation::SpritesAnimation(true, 0.2f, "skeleton_idle");
					for (int i = 0; i < 17; i++)
					{
						zombie1_idle->AddFrame(sf::Sprite(context->game->Resources->getTextureResourceDataByName("skeleton-idle_" + std::to_string(i))->texture));
					}


					Animation::SpritesAnimation*zombie1_move = new  Animation::SpritesAnimation(true, 0.2f, "skeleton_move");
					for (int i = 0; i < 17; i++)
					{
						zombie1_move->AddFrame(sf::Sprite(context->game->Resources->getTextureResourceDataByName("skeleton-move_" + std::to_string(i))->texture));
					}


					Animation::SpritesAnimation*zombie1_attack = new  Animation::SpritesAnimation(true, 0.1f, "skeleton_attack");
					for (int i = 0; i < 9; i++)
					{
						zombie1_attack->AddFrame(sf::Sprite(context->game->Resources->getTextureResourceDataByName("skeleton-attack_" + std::to_string(i))->texture));
					}


					z->spritesAnimations->addAnimation(zombie1_idle);

					z->spritesAnimations->addAnimation(zombie1_move);

					z->spritesAnimations->addAnimation(zombie1_attack);

					z->addRelation({ RelationType::Noone ,npc_zombie::Type() });
					z->Init();
					z->SetAnimation("skeleton_attack");

				}
				else if (PropPhysics*pp = dynamic_cast<PropPhysics*>(StateObjects->at(i)))
				{
					b2MassData propMass;
					propMass.mass = pp->mass / 10;
					propMass.center = b2Vec2(pp->GetObjectRectangle().left + pp->GetObjectRectangle().width / 2, pp->GetObjectRectangle().top + pp->GetObjectRectangle().height / 2);
					b2PolygonShape shapeA;
					shapeA.SetAsBox(StateObjects->at(i)->GetObjectRectangle().width / 2, StateObjects->at(i)->GetObjectRectangle().height / 2);


					b2BodyDef defA;
					defA.position.Set(StateObjects->at(i)->GetObjectPosition().x + StateObjects->at(i)->GetObjectRectangle().width / 2, StateObjects->at(i)->GetObjectPosition().y + StateObjects->at(i)->GetObjectRectangle().height / 2);
					defA.type = b2BodyType::b2_dynamicBody;

					StateObjects->at(i)->body = world.CreateBody(&defA);

					StateObjects->at(i)->body->CreateFixture(&shapeA, 1.f);
					StateObjects->at(i)->body->SetUserData(StateObjects->at(i));
					StateObjects->at(i)->Init();
					StateObjects->at(i)->OnCollision = [this, pp](Object*object, b2Fixture *fixtureA, b2Fixture *fixtureB)
					{
						pp->onCollision(object, this->context, "PlayState");
					};
					pp->LeftCollision = [this, pp](Object*object, b2Fixture *fixtureA, b2Fixture *fixtureB)
					{
						pp->leftCollision(object, this->context, "PlayState");
					};
					StateObjects->at(i)->physBodyInitialized = true;
					StateObjects->at(i)->bodyIsSensor = false;

					StateObjects->at(i)->body->SetMassData(&propMass);
					StateObjects->at(i)->Init();
				}

				else if (ammo_pickup_object*apo = dynamic_cast<ammo_pickup_object*>(StateObjects->at(i)))
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
					TriggerFixture.isSensor = 1;

					StateObjects->at(i)->body->CreateFixture(&TriggerFixture);
					StateObjects->at(i)->body->SetUserData(StateObjects->at(i));

					StateObjects->at(i)->physBodyInitialized = true;
					StateObjects->at(i)->bodyIsSensor = TriggerFixture.isSensor;

					StateObjects->at(i)->Init();
				}

				else if (weapon_pickup_object*apo = dynamic_cast<weapon_pickup_object*>(StateObjects->at(i)))
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
					TriggerFixture.isSensor = 1;

					StateObjects->at(i)->body->CreateFixture(&TriggerFixture);
					StateObjects->at(i)->body->SetUserData(StateObjects->at(i));

					StateObjects->at(i)->physBodyInitialized = true;
					StateObjects->at(i)->bodyIsSensor = TriggerFixture.isSensor;

					StateObjects->at(i)->Init();
				}

				else if (ai_node*ain = dynamic_cast<ai_node*>(StateObjects->at(i)))
				{
					if (this->GetObjectByName(ain->next_name) != NULL && ain->next_name.c_str() != "")
					{
						ain->next = dynamic_cast<ai_node*>(this->GetObjectByName(ain->next_name));
					}

					b2BodyDef def;
					def.position.Set(StateObjects->at(i)->GetObjectPosition().x + StateObjects->at(i)->GetObjectRectangle().width / 2, StateObjects->at(i)->GetObjectPosition().y + StateObjects->at(i)->GetObjectRectangle().height / 2);
					def.type = b2BodyType::b2_staticBody;
					StateObjects->at(i)->body = world.CreateBody(&def);

					b2PolygonShape shape;
					shape.SetAsBox(StateObjects->at(i)->GetObjectRectangle().width / 2, StateObjects->at(i)->GetObjectRectangle().height / 2);

					b2PolygonShape Actshape;
					Actshape.SetAsBox(ain->actionRectWidth / 2, ain->actionRectHeight / 2);

					b2FixtureDef actionFixture;
					actionFixture.filter = filter;
					actionFixture.density = 0.f;
					actionFixture.shape = &Actshape;
					actionFixture.isSensor = 1;
					actionFixture.userData = new FixtureData(FixtureActionType::AI_Node_Action);

					b2FixtureDef TriggerFixture;
					TriggerFixture.filter = filter;
					TriggerFixture.density = 0.f;
					TriggerFixture.shape = &shape;
					TriggerFixture.isSensor = 1;
					TriggerFixture.userData = new FixtureData(FixtureActionType::Trigger);



					StateObjects->at(i)->body->CreateFixture(&actionFixture);
					StateObjects->at(i)->body->CreateFixture(&TriggerFixture);

					StateObjects->at(i)->body->SetUserData(StateObjects->at(i));

					StateObjects->at(i)->physBodyInitialized = true;
					//ai_nodes can not have collision of the solid object
					StateObjects->at(i)->bodyIsSensor = true;

					ain->OnCollision = [this, ain](Object*object, b2Fixture *fixtureA, b2Fixture *fixtureB)
					{
						ain->onCollision(object, fixtureA, fixtureB, this->context, "PlayState");
					};

					ain->LeftCollision = [this, ain](Object*object, b2Fixture *fixtureA, b2Fixture *fixtureB)
					{
						ain->leftCollision(object, fixtureA, fixtureB, this->context, "PlayState");
					};


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
		defP.bullet = true;
		player->body = world.CreateBody(&defP);
		

		/*b2PolygonShape shape;
		shape.SetAsBox(player->GetObjectRectangle().width / 2, player->GetObjectRectangle().height / 2);*/
		b2PolygonShape shapeP;
		shapeP.SetAsBox(player->GetObjectRectangle().width / 2.5, player->GetObjectRectangle().height / 2.5);

		b2FixtureDef TriggerFixtureP;
		TriggerFixtureP.filter = filter2;
		TriggerFixtureP.density = 1.f;
		TriggerFixtureP.shape = &shapeP;


		player->body->CreateFixture(&TriggerFixtureP);
		player->body->SetUserData(player);



		//player body end
		player->Init();

		_map_is_loaded = true;
	}


	void Init()override
	{
		context->window->setFramerateLimit(300);

		Animation::SpritesAnimation*rifle_move = new  Animation::SpritesAnimation(true, 0.2f, "solder_rifle_move");
		for (int i = 0; i < 20; i++)
		{
			rifle_move->AddFrame(sf::Sprite(context->game->Resources->getTextureResourceDataByName("solder_rifle_move_" + std::to_string(i))->texture));
		}
		player->spritesAnimations->addAnimation(rifle_move);

		Animation::SpritesAnimation*rifle_reload = new  Animation::SpritesAnimation(true, 0.05f, "solder_rifle_reload");
		for (int i = 0; i < 20; i++)
		{
			rifle_reload->AddFrame(sf::Sprite(context->game->Resources->getTextureResourceDataByName("solder_rifle_reload_" + std::to_string(i))->texture));
		}
		player->spritesAnimations->addAnimation(rifle_reload);

		Animation::SpritesAnimation*pistol_reload = new  Animation::SpritesAnimation(true, 0.06f, "solder_pistol_reload");
		for (int i = 0; i < 15; i++)
		{
			pistol_reload->AddFrame(sf::Sprite(context->game->Resources->getTextureResourceDataByName("solder_pistol_reload_" + std::to_string(i))->texture));
		}
		player->spritesAnimations->addAnimation(pistol_reload);

		//
		Animation::SpritesAnimation*pistol_shoot = new  Animation::SpritesAnimation(false, 0.3f, "solder_pistol_shoot");
		for (int i = 0; i < 3; i++)
		{
			pistol_shoot->AddFrame(sf::Sprite(context->game->Resources->getTextureResourceDataByName("solder_pistol_shoot_" + std::to_string(i))->texture));
		}
		player->spritesAnimations->addAnimation(pistol_shoot);

		//
		Animation::SpritesAnimation*rifle_shoot = new  Animation::SpritesAnimation(false, 0.3f, "solder_rifle_shoot");
		for (int i = 0; i < 3; i++)
		{
			rifle_shoot->AddFrame(sf::Sprite(context->game->Resources->getTextureResourceDataByName("solder_rifle_shoot_" + std::to_string(i))->texture));
		}
		player->spritesAnimations->addAnimation(rifle_shoot);


		//
		Animation::SpritesAnimation*shotgun_shoot = new  Animation::SpritesAnimation(false, 0.3f, "solder_shotgun_shoot");
		for (int i = 0; i < 3; i++)
		{
			shotgun_shoot->AddFrame(sf::Sprite(context->game->Resources->getTextureResourceDataByName("solder_shotgun_shoot_" + std::to_string(i))->texture));
		}
		player->spritesAnimations->addAnimation(shotgun_shoot);



		//
		Animation::SpritesAnimation*shotgun_reload = new  Animation::SpritesAnimation(true, 0.05f, "solder_shotgun_reload");
		for (int i = 0; i < 20; i++)
		{
			shotgun_reload->AddFrame(sf::Sprite(context->game->Resources->getTextureResourceDataByName("solder_shotgun_reload_" + std::to_string(i))->texture));
		}
		player->spritesAnimations->addAnimation(shotgun_reload);

		//
		Animation::SpritesAnimation*shotgun_move = new  Animation::SpritesAnimation(true, 0.05f, "solder_shotgun_move");
		for (int i = 0; i < 20; i++)
		{
			shotgun_move->AddFrame(sf::Sprite(context->game->Resources->getTextureResourceDataByName("solder_shotgun_move_" + std::to_string(i))->texture));
		}
		player->spritesAnimations->addAnimation(shotgun_move);


		Animation::SpritesAnimation*pistol_move = new  Animation::SpritesAnimation(true, 0.2f, "solder_pistol_move");
		for (int i = 0; i < 20; i++)
		{
			pistol_move->AddFrame(sf::Sprite(context->game->Resources->getTextureResourceDataByName("solder_pistol_move_" + std::to_string(i))->texture));
		}
		player->spritesAnimations->addAnimation(pistol_move);

		//
		Animation::SpritesAnimation*knife_move = new  Animation::SpritesAnimation(true, 0.2f, "solder_knife_move");
		for (int i = 0; i < 20; i++)
		{
			knife_move->AddFrame(sf::Sprite(context->game->Resources->getTextureResourceDataByName("solder_knife_move_" + std::to_string(i))->texture));
		}
		player->spritesAnimations->addAnimation(knife_move);

		//
		Animation::SpritesAnimation*knife_idle = new  Animation::SpritesAnimation(true, 0.05f, "solder_knife_idle");
		for (int i = 0; i < 20; i++)
		{
			knife_idle->AddFrame(sf::Sprite(context->game->Resources->getTextureResourceDataByName("solder_knife_idle_" + std::to_string(i))->texture));
		}
		player->spritesAnimations->addAnimation(knife_idle);

		//
		Animation::SpritesAnimation*knife_attack = new  Animation::SpritesAnimation(false, 0.05f, "solder_knife_attack");
		for (int i = 0; i <15; i++)
		{
			knife_attack->AddFrame(sf::Sprite(context->game->Resources->getTextureResourceDataByName("solder_knife_attack_" + std::to_string(i))->texture));
		}
		knife_attack->ForceScale = true;
		player->spritesAnimations->addAnimation(knife_attack);

		//
		Animation::SpritesAnimation*pistol_idle = new  Animation::SpritesAnimation(true, 0.05f, "solder_pistol_idle");
		for (int i = 0; i < 20; i++)
		{
			pistol_idle->AddFrame(sf::Sprite(context->game->Resources->getTextureResourceDataByName("solder_pistol_idle_" + std::to_string(i))->texture));
		}
		player->spritesAnimations->addAnimation(pistol_idle);

		//
		Animation::SpritesAnimation*rifle_idle = new  Animation::SpritesAnimation(true, 0.05f, "solder_rifle_idle");
		for (int i = 0; i < 20; i++)
		{
			rifle_idle->AddFrame(sf::Sprite(context->game->Resources->getTextureResourceDataByName("solder_rifle_idle_" + std::to_string(i))->texture));
		}
		player->spritesAnimations->addAnimation(rifle_idle);

		//
		Animation::SpritesAnimation*shotgun_idle = new  Animation::SpritesAnimation(true, 0.05f, "solder_shotgun_idle");
		for (int i = 0; i < 20; i++)
		{
			shotgun_idle->AddFrame(sf::Sprite(context->game->Resources->getTextureResourceDataByName("solder_shotgun_idle_" + std::to_string(i))->texture));
		}
		player->spritesAnimations->addAnimation(shotgun_idle);

		player->Init();


		projObj = new projectile(sf::Vector2f(0, 0), 100.f, 100.f, 50.0f, 2.0f, sf::Sprite(context->game->Resources->getTextureResourceDataByName("proj")->texture));


		


		player->AddAmmo({ AMMO_TYPE_PISTOL,5});
		player->AddAmmo({ AMMO_TYPE_RIFLE,5 });
		player->AddAmmo({ AMMO_TYPE_SHOTGUN,5 });
		player->AddAmmo({ AMMO_TYPE_SHOTGUN,1 });






		//GUI

		GUI::Label*l1 = new GUI::Label("weapon_name", "0", sf::Color::Red, context->game->Resources->getFontResourceDataByName("calibri")->font, 80, context->game->Resources->getTextureResourceDataByName("textBoxTexture1")->texture);
		l1->Init();
		this->PlayerUI->Components->push_back(l1);

		GUI::Label*l2 = new GUI::Label("health", "0", sf::Color::Red, context->game->Resources->getFontResourceDataByName("calibri")->font, 80, context->game->Resources->getTextureResourceDataByName("textBoxTexture1")->texture);
		l2->SetPosition(sf::Vector2f(10, SCREENHEIGHT - 200));
		l2->Init();
		this->PlayerUI->Components->push_back(l2);

		GUI::Label*l_ammo = new GUI::Label("ammo_in_clip", "9999", sf::Color::Red, context->game->Resources->getFontResourceDataByName("calibri")->font, 80, context->game->Resources->getTextureResourceDataByName("textBoxTexture1")->texture);
		l_ammo->SetPosition(sf::Vector2f(SCREENWIDTH-400, SCREENHEIGHT - 200));
		l_ammo->Init();
		this->PlayerUI->Components->push_back(l_ammo);

		GUI::Label*l_clips = new GUI::Label("clips", "9999", sf::Color::Red, context->game->Resources->getFontResourceDataByName("calibri")->font, 80, context->game->Resources->getTextureResourceDataByName("textBoxTexture1")->texture);
		l_clips->SetPosition(sf::Vector2f(SCREENWIDTH - 220, SCREENHEIGHT - 200));
		l_clips->Init();
		this->PlayerUI->Components->push_back(l_clips);


		cursorParticles.Stop();
		projObj->Init();
		//GUI


		b2Filter filter;
		filter.categoryBits = 0x1;

		b2Filter filter2;
		filter2.categoryBits = 0x1;
		/*this->LoadMap("td_reverb.tmx");*/
		//player body begin
		player->Init();

		b2BodyDef defP;
		defP.position.Set(player->GetObjectPosition().x + player->GetObjectRectangle().width / 2, player->GetObjectPosition().y + player->GetObjectRectangle().height / 2);
		defP.type = b2BodyType::b2_dynamicBody;
		defP.bullet = true;
		player->body = world.CreateBody(&defP);

		/*b2PolygonShape shape;
		shape.SetAsBox(player->GetObjectRectangle().width / 2, player->GetObjectRectangle().height / 2);*/
		b2PolygonShape shapeP;
		shapeP.SetAsBox(player->GetObjectRectangle().width / 2.5, player->GetObjectRectangle().height / 2.5);

		b2FixtureDef TriggerFixtureP;
		TriggerFixtureP.filter = filter2;
		TriggerFixtureP.density = 1.f;
		TriggerFixtureP.shape = &shapeP;


		player->body->CreateFixture(&TriggerFixtureP);
		player->body->SetUserData(player);



		//player body end
		player->Init();



		/*context->game->lowSoundSystem->createSound("./sounds/pistol_fire2.wav", FMOD_3D, 0, &shoot);
		context->game->lowSoundSystem->createSound("./sounds/fire1.wav", FMOD_3D, 0, &shoot2);
		context->game->lowSoundSystem->createSound("./sounds/ambience_base.wav", FMOD_3D, 0, &ambience1);*/
		context->game->Resources->getSoundResourceDataByName("ambience_base")->sound->setMode(FMOD_LOOP_NORMAL);
		context->game->Resources->getSoundResourceDataByName("ambience_base")->sound->setLoopCount(-1);


		Animation::Animation rhand_anim = Animation::Animation("rhand");
		rhand_anim.FrameIndexes->push_back(Animation::CellIndex(0, 0));


	}
	PlayState(std::string Name) :State(Name), world(b2Vec2(0.f, 0.f))
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
				if (this->StateObjects->at(i)->area_id == Current_area_id)
				{
					this->StateObjects->at(i)->Draw(context->window);

				}
				if (DEBUG_DRAWCOLLISION)
				{

					if (!dynamic_cast<SceneTile*>(StateObjects->at(i)))
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
				}
				if (DEBUG_DRAWREVERB)
				{

					sf::CircleShape cs;
					sf::CircleShape csm;
					if (!reverbs->empty())
					{
						for (size_t i = 0; i < reverbs->size(); i++)
						{

							cs.setFillColor(sf::Color::Red);
							FMOD_VECTOR pos;
							float maxD = 0.f;
							float minD = 0.f;
							reverbs->at(i)->get3DAttributes(&pos, &minD, &maxD);
							cs.setPosition(sf::Vector2f(pos.x, pos.y));
							cs.setRadius(minD);

							csm.setFillColor(sf::Color::Green);

							reverbs->at(i)->get3DAttributes(&pos, &minD, &maxD);
							csm.setPosition(sf::Vector2f(pos.x, pos.y));
							csm.setRadius(maxD);

							context->window->draw(cs);
							context->window->draw(csm);
						}

					}
				}


				/*context->window->draw(projObj->sprite);*/

			}

		}
		player->Draw(context->window);

		sf::Transform tr;
		tr.rotate(player->RotationAngle, 0, 0);

		sf::Vector2f pointPos = player->GetChildByName("rifle_shoot_point")->GetObjectPosition();
		pointPos = tr.transformPoint(pointPos);
		/*pointPos = RotatePoint(pointPos, player->RotationAngle*(180 / M_PI), { 0,0 });*/
		
		pointPos += sf::Vector2f(player->body->GetPosition().x, player->body->GetPosition().y)/*{player->GetObjectPosition().x - player->GetObjectRectangle().width, player->GetObjectPosition().y - player->GetObjectRectangle().height}*/;
		sf::Vertex p[2];
		p[0] = sf::Vector2f(player->body->GetPosition().x, player->body->GetPosition().y);
		p[1]= pointPos;
		
		
		context->window->draw(p, 2, sf::PrimitiveType::Lines);
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
				if (player->body->GetFixtureList()->GetType() == b2Shape::Type::e_polygon)
				{
					b2PolygonShape*coll = dynamic_cast<b2PolygonShape*>(player->body->GetFixtureList()->GetShape());

					sf::VertexArray va = sf::VertexArray(sf::PrimitiveType::LineStrip);

					for (int ind = 0; ind < coll->m_count; ind++)
					{
						va.append(sf::Vertex(sf::Vector2f(coll->m_vertices[ind].x + player->body->GetPosition().x, coll->m_vertices[ind].y + player->body->GetPosition().y), sf::Color::Red));
					}
					va.append(sf::Vertex(sf::Vector2f(coll->m_vertices[0].x + player->body->GetPosition().x, coll->m_vertices[0].y + player->body->GetPosition().y), sf::Color::Red));
					context->window->draw(va);
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


		if (!player->items->isEmpty() && player->items->isVisible)
		{
			player->items->drawItems(context->window);
		}
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
			context->game->close();
			return;
		}
		else if (event.key.code == sf::Keyboard::Tab&&event.type == sf::Event::EventType::KeyPressed)
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
		else if (event.key.code == sf::Keyboard::Num1&&event.type == sf::Event::EventType::KeyPressed)
		{
			LoadMap("td_free_tv.tmx");
		}

		else if (event.key.code == sf::Keyboard::Num2&&event.type == sf::Event::EventType::KeyPressed)
		{
			LoadMap("t_t1.tmx");
		}

		else if (event.key.code == sf::Keyboard::Num3&&event.type == sf::Event::EventType::KeyPressed)
		{
			LoadMap("ai_zombie_test.tmx");
		}
		else if (event.key.code == sf::Keyboard::I&&event.type == sf::Event::EventType::KeyPressed)
		{
			player->AddAmmo({ static_cast<int>(player->currentWeapon->weapon_ammo_type),5 });
		}
		else if (event.key.code == sf::Keyboard::P&&event.type == sf::Event::EventType::KeyPressed)
		{
			std::cout << player->body->GetPosition().x << std::endl;
			std::cout << player->body->GetPosition().y << std::endl;
			std::cout << std::endl;
			std::cout << this->current_map << std::endl;
			std::cout << std::endl;
		}
		else if (event.mouseButton.button == sf::Mouse::Left&&event.type == sf::Event::EventType::MouseButtonPressed)
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


			this->world;
			this->context->game->GetStateByName("PlayState")->world;
			Animation::Animation blood_a = context->game->animationsData->getAnimationDataByName("blood_a");

			b2Filter filter;
			filter.categoryBits = 1;

			sf::Vector2f mousePos = sf::Vector2f(event.mouseButton.x + player->GetObjectPosition().x - (SCREENWIDTH / 2), event.mouseButton.y + player->GetObjectPosition().y - (SCREENHEIGHT / 2));
			sf::Vector2f diff;
			diff.x = mousePos.x - player->GetObjectPosition().x;
			diff.y = mousePos.y - player->GetObjectPosition().y;



			if (player->currentWeapon->weaponType != WEAPON_TYPE_TAD_KNIFE)
			{
				if (player->currentWeapon->ammoInTheClip <= 0 && player->currentWeapon->clips > 0)
				{
					player->is_reloading = true;

					if (player->reload_sound_channel_id == -1)
					{
						this->PlaySound(player->currentWeapon->reload_sound_name, player->reload_sound_channel_id);

						if (player->reload_sound_channel_id != -1)
						{
							bool isPlaying = false;
							context->game->Channels->at(player->reload_sound_channel_id)->isPlaying(&isPlaying);
							if (isPlaying)
							{
								FMOD_VECTOR pos;
								pos.z = 0;
								pos.x = player->body->GetPosition().x;
								pos.y = player->body->GetPosition().y;
								FMOD_RESULT r = context->game->Channels->at(player->reload_sound_channel_id)->set3DAttributes(&pos, 0, 0);
								if (r != FMOD_OK)
								{
									std::cout << FMOD_ErrorString(r) << " -\"Reload\" Sound 3D positioning on Handle Event" << std::endl;
								}
							}
						}
					}
				}
			}

			if (!player->is_shooting)
			{

				if (player->currentWeapon->weaponType == WEAPON_TYPE_TAD_KNIFE)
				{
					//knifes hit is made by shooting projectile in the direction of the attack
					//that's the sipliest way of doing that

					player->is_shooting = true;

					sf::Transform tr;
					tr.rotate(player->RotationAngle, 0, 0);

					sf::Vector2f pointPos = player->GetChildByName("rifle_shoot_point")->GetObjectPosition();
					pointPos = tr.transformPoint(pointPos);

					pointPos += sf::Vector2f(player->body->GetPosition().x, player->body->GetPosition().y);
					/*diff.x = mousePos.x - pointPos.x;
					diff.y = mousePos.y - pointPos.y;*/

					//player can only hit few dm(meter/10) in front of him/her-self
					knife_attack_projectile* bullet = new knife_attack_projectile(sf::Vector2f(0, 0), 15.f, 15.f, 5.0f, player->currentWeapon->projectileSpeed * 10);
					bullet->owner = player;
					bullet->OnCollision = [this, blood_a, bullet](Object*object, b2Fixture *fixtureA, b2Fixture *fixtureB)
					{
						bullet->projectileOnCollision(object, fixtureA, fixtureB, this->context, "PlayState");
					};
					bullet->LeftCollision = [this, bullet](Object*object, b2Fixture *fixtureA, b2Fixture *fixtureB)
					{
						bullet->projectileOnLeftCollision(object, this->context, "PlayState");
					};

					bullet->Launch(static_cast<float>((atan2(diff.y, diff.x)/**(180 / M_PI)*/)), pointPos/*sf::Vector2f(player->body->GetPosition().x, player->body->GetPosition().y)*/, this->world, filter);
					player->Projectiles->push_back(bullet);
					/*player->currentWeapon->ammoInTheClip -= 1;*/

					int channel_id = -1;
					int num = m_get_random_number(1, 3);
					PlaySound(player->currentWeapon->shoot_sound_name + std::to_string(num), channel_id);

					if (channel_id != -1)
					{
						if (channel_id != -1)
						{
							player->shooting_sound_channel_ids->push_back(channel_id);

							bool isPlaying = false;

							if (context->game->Channels->at(channel_id) != NULL)
							{
								context->game->Channels->at(channel_id)->isPlaying(&isPlaying);
								if (isPlaying)
								{
									FMOD_VECTOR pos;
									pos.z = 0;
									pos.x = player->body->GetPosition().x;
									pos.y = player->body->GetPosition().y;
									context->game->Channels->at(channel_id)->set3DAttributes(&pos, 0);
								}
							}
						}
					}
				}
				if (!player->is_reloading)
				{
					player->is_shooting = true;
					if (player->currentWeapon->weaponType == WEAPON_TYPE_TAD_SHOTGUN)
					{
						if (player->currentWeapon->ammoInTheClip > 0)
						{
							for (int i = 0; i < player->currentWeapon->bullets_per_shot; i++)
							{

								if (player->currentWeapon->ammoInTheClip <= 0)
								{
									int channel_id = -1;
									this->PlaySound(player->currentWeapon->empty_clip_sound, channel_id);
									if (channel_id != -1)
									{
										player->shooting_sound_channel_ids->push_back(channel_id);
									}



									break;
								}

								projectile* bullet = new projectile(sf::Vector2f(0, 0), 10.f, 10.f, 500.0f, player->currentWeapon->projectileSpeed * 10, sf::Sprite(context->game->Resources->getTextureResourceDataByName("proj")->texture));

								bullet->OnCollision = [this, blood_a, bullet](Object*object, b2Fixture *fixtureA, b2Fixture *fixtureB)
								{

									bullet->projectileOnCollision(object, fixtureA, fixtureB, this->context, "PlayState");
								};
								bullet->LeftCollision = [this, bullet](Object*object, b2Fixture *fixtureA, b2Fixture *fixtureB)
								{
									bullet->projectileOnLeftCollision(object, this->context, "PlayState");
								};
								bullet->owner = player;
								bullet->Launch(static_cast<float>((atan2(diff.y, diff.x) + i/**(180 / M_PI)*/)), sf::Vector2f(player->body->GetPosition().x, player->body->GetPosition().y), this->world, filter);
								player->Projectiles->push_back(bullet);


								int channel_id = -1;
								PlaySound(player->currentWeapon->shoot_sound_name, channel_id);

								if (channel_id != -1)
								{
									if (channel_id != -1)
									{
										player->shooting_sound_channel_ids->push_back(channel_id);

										bool isPlaying = false;

										if (context->game->Channels->at(channel_id) != NULL)
										{
											context->game->Channels->at(channel_id)->isPlaying(&isPlaying);
											if (isPlaying)
											{
												FMOD_VECTOR pos;
												pos.z = 0;
												pos.x = player->body->GetPosition().x;
												pos.y = player->body->GetPosition().y;
												context->game->Channels->at(channel_id)->set3DAttributes(&pos, 0);
											}
										}
									}
								}

							}
							player->currentWeapon->ammoInTheClip -= 1;
						}
					}
					else
					{
						for (int i = 0; i < player->currentWeapon->bullets_per_shot; i++)
						{
							if (player->currentWeapon->ammoInTheClip <= 0)
							{
								this->PlaySound(player->currentWeapon->empty_clip_sound);
								break;
							}


							sf::Transform tr;
							tr.rotate(player->RotationAngle, 0, 0);

							sf::Vector2f pointPos = player->GetChildByName("rifle_shoot_point")->GetObjectPosition();
							pointPos = tr.transformPoint(pointPos);

							pointPos += sf::Vector2f(player->body->GetPosition().x, player->body->GetPosition().y);
							/*diff.x = mousePos.x - pointPos.x;
							diff.y = mousePos.y - pointPos.y;*/

							projectile* bullet = new projectile(sf::Vector2f(0, 0), 5.f, 5.f, 500.0f, player->currentWeapon->projectileSpeed * 10, sf::Sprite(context->game->Resources->getTextureResourceDataByName("proj")->texture));
							bullet->owner = player;
							bullet->OnCollision = [this, blood_a, bullet](Object*object, b2Fixture *fixtureA, b2Fixture *fixtureB)
							{

								bullet->projectileOnCollision(object, fixtureA, fixtureB, this->context, "PlayState");
							};
							bullet->LeftCollision = [this, bullet](Object*object, b2Fixture *fixtureA, b2Fixture *fixtureB)
							{
								bullet->projectileOnLeftCollision(object, this->context, "PlayState");
							};

							bullet->Launch(static_cast<float>((atan2(diff.y, diff.x)/**(180 / M_PI)*/)), pointPos/*sf::Vector2f(player->body->GetPosition().x, player->body->GetPosition().y)*/, this->world, filter);
							player->Projectiles->push_back(bullet);
							player->currentWeapon->ammoInTheClip -= 1;

							int channel_id = -1;
							PlaySound(player->currentWeapon->shoot_sound_name, channel_id);

							if (channel_id != -1)
							{
								if (channel_id != -1)
								{
									player->shooting_sound_channel_ids->push_back(channel_id);

									bool isPlaying = false;

									if (context->game->Channels->at(channel_id) != NULL)
									{
										context->game->Channels->at(channel_id)->isPlaying(&isPlaying);
										if (isPlaying)
										{
											FMOD_VECTOR pos;
											pos.z = 0;
											pos.x = player->body->GetPosition().x;
											pos.y = player->body->GetPosition().y;
											context->game->Channels->at(channel_id)->set3DAttributes(&pos, 0);
										}
									}
								}
							}
						}
					}

				}
			}
		}
		else if (event.mouseButton.button == sf::Mouse::Right&&event.type == sf::Event::EventType::MouseButtonPressed)
		{

			for (size_t i = 0; i < StateObjects->size(); i++)
			{
				if (dynamic_cast<PropPhysics*>(StateObjects->at(i)))
				{
					StateObjects->at(i)->applyImpulse(b2Vec2(100, 100));
				}
			}
		}
		else if (event.type == sf::Event::EventType::MouseWheelScrolled)
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
		else if (event.type == sf::Event::EventType::MouseMoved)
		{
			sf::Vector2f mousePos = sf::Vector2f(event.mouseMove.x + player->GetObjectPosition().x - (SCREENWIDTH / 2), event.mouseMove.y + player->GetObjectPosition().y - (SCREENHEIGHT / 2));
			sf::Vector2f diff;
			diff.x = mousePos.x - player->GetObjectPosition().x;
			diff.y = mousePos.y - player->GetObjectPosition().y;


			/*std::cout << atan2(diff.y, diff.x)*(180 / M_PI) << std::endl;*/
		/*	player->Anim->sprite.setRotation((atan2(diff.y, diff.x)*(180 / M_PI)));*/
			player->RotationAngle = (atan2(diff.y, diff.x)*(180 / M_PI));
		}
		else if (event.key.code == sf::Keyboard::Left&&event.type == sf::Event::EventType::KeyPressed)
		{
			WalkLeft = true;
		}
		else if (event.key.code == sf::Keyboard::Right&&event.type == sf::Event::EventType::KeyPressed)
		{
			WalkRight = true;
		}
		else if (event.key.code == sf::Keyboard::Up&&event.type == sf::Event::EventType::KeyPressed)
		{
			WalkUp = true;
		}
		else if (event.key.code == sf::Keyboard::Down&&event.type == sf::Event::EventType::KeyPressed)
		{
			WalkDown = true;
		}


		else if (event.key.code == sf::Keyboard::Left&&event.type == sf::Event::EventType::KeyReleased)
		{
			WalkLeft = false;
		}
		else if (event.key.code == sf::Keyboard::Right&&event.type == sf::Event::EventType::KeyReleased)
		{
			WalkRight = false;
		}
		else if (event.key.code == sf::Keyboard::Up&&event.type == sf::Event::EventType::KeyReleased)
		{
			WalkUp = false;
		}
		else if (event.key.code == sf::Keyboard::Down&&event.type == sf::Event::EventType::KeyReleased)
		{
			WalkDown = false;
		}



		if (WalkLeft)
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
				player->AddVelocity(sf::Vector2f((player->accel), 0.f));
			}

		}
		if (!WalkRight && !WalkLeft)
		{
			if (player->Velocity.x > 0.f) {
				player->AddVelocity(sf::Vector2f(-(player->accel), 0.f));
			}
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
				player->AddVelocity(sf::Vector2f(0.f, -(player->accel)));
			}
			if (player->Velocity.y < 0.f) { player->AddVelocity(sf::Vector2f(0.f, (player->accel))); }
			/*player->SetVelocityY(0.f);*/

		}



	}

	virtual void Update(sf::Time dt) override
	{

		
		if (_map_is_loaded != true)
		{
			if (current_map != "")
				LoadMap(current_map);
		}
		context->game->lowSoundSystem->update();

		if (!player->items->isEmpty() && player->items->isActive)
		{
			if (player->items->currentItemId != -1)
			{
				sf::Vector2i  mousePos = sf::Mouse::getPosition() - context->window->getPosition();
				player->items->_items->at(player->items->currentItemId)->sprite.setPosition(sf::Vector2f(mousePos.x, mousePos.y));
			}

		}


		player->body->SetLinearVelocity(b2Vec2(player->Velocity.x * 5, player->Velocity.y * 5));
		if (!worldIsPaused)
		{
			world.Step(1.f / dt.asSeconds(), 5, 5);
		}

		player->Update(dt);
		
		if (player->body->GetLinearVelocity().x != 0 || player->body->GetLinearVelocity().y != 0)
		{

			if (player->footsteps_sound_channel_id >= 0)
			{
				bool isPlaying = false;
				context->game->Channels->at(player->footsteps_sound_channel_id)->isPlaying(&isPlaying);
				if (!isPlaying)
				{
					if (player->time_footstep_elapsed >= player->time_per_footstep)
					{
						int channel_id = 0;

						this->PlaySound(player->GetFootstepSoundName(), player->footsteps_sound_channel_id);
						player->time_footstep_elapsed = 0.f;

						FMOD_VECTOR pos;
						pos.z = 0;
						pos.x = player->body->GetPosition().x;
						pos.y = player->body->GetPosition().y;
						FMOD_RESULT r = context->game->Channels->at(player->footsteps_sound_channel_id)->set3DAttributes(&pos, 0, 0);
						if (r != FMOD_OK)
						{
							std::cout << FMOD_ErrorString(r) << "aaa" << std::endl;
						}
					}
					else
					{
						player->time_footstep_elapsed += dt.asSeconds();				
					}
					
				}
				else
				{
					if (player->time_footstep_elapsed >= player->time_per_footstep)
					{
						int channel_id = 0;

						this->PlaySound(player->GetFootstepSoundName(), player->footsteps_sound_channel_id);
						player->time_footstep_elapsed = 0.f;
					}
					else
					{
						player->time_footstep_elapsed += dt.asSeconds();				
					}

					FMOD_VECTOR pos;
					pos.z = 0;
					pos.x = player->body->GetPosition().x;
					pos.y = player->body->GetPosition().y;
					FMOD_RESULT r = context->game->Channels->at(player->footsteps_sound_channel_id)->set3DAttributes(&pos, 0, 0);
					if (r != FMOD_OK)
					{
						std::cout << FMOD_ErrorString(r) <<"0"<< std::endl;
					}
				}
			}
			else
			{
				int channel_id = 0;
				this->PlaySound(player->GetFootstepSoundName(), player->footsteps_sound_channel_id);
				player->time_footstep_elapsed = 0.f;			

				FMOD_VECTOR pos;
				pos.z = 0;
				pos.x = player->body->GetPosition().x;
				pos.y = player->body->GetPosition().y;
				FMOD_RESULT r = context->game->Channels->at(player->footsteps_sound_channel_id)->set3DAttributes(&pos, 0, 0);
				if (r != FMOD_OK)
				{
					std::cout << FMOD_ErrorString(r) <<"1"<< std::endl;
				}
			}
			if (context->game->Channels->at(player->footsteps_sound_channel_id) != NULL)
			{
				
			}
		}
	
		if (player->is_reloading)
		{
			
			if (player->_time_in_reload >= player->currentWeapon->reload_time)
			{
				if (player->currentWeapon->clips > 0)
				{
					if (!player->ammoData->empty())
					{
						for (size_t i = 0; i < player->ammoData->size(); i++)
						{
							if (player->ammoData->at(i).ammo_type == player->currentWeapon->weapon_ammo_type)
							{
								player->ammoData->at(i).clip_amount--;
							}
						}
					}
					player->currentWeapon->clips -= 1;
					if (player->currentWeapon->weaponType == WEAPON_TYPE_TAD_SHOTGUN)
					{
						player->currentWeapon->ammoInTheClip += 1;
						if (player->currentWeapon->ammoInTheClip >= player->currentWeapon->ammoPerClip)
						{
							player->is_reloading = false;
						}
						else
						{
							if (player->currentWeapon->clips <= 0)
							{
								player->is_reloading = false;
							}
							else
							{
								this->PlaySound(player->currentWeapon->reload_sound_name, player->reload_sound_channel_id);

								bool isPlaying = false;
								context->game->Channels->at(player->reload_sound_channel_id)->isPlaying(&isPlaying);
								if (isPlaying)
								{
									FMOD_VECTOR pos;
									pos.z = 0;
									pos.x = player->body->GetPosition().x;
									pos.y = player->body->GetPosition().y;
									FMOD_RESULT r = context->game->Channels->at(player->reload_sound_channel_id)->set3DAttributes(&pos, 0, 0);
									if (r != FMOD_OK)
									{
										std::cout << FMOD_ErrorString(r) << " -\"Reload\" Sound 3D positioning on Update" << std::endl;
									}
								}
							}
							
							
						}
					}
					else
					{
						player->currentWeapon->ammoInTheClip = player->currentWeapon->ammoPerClip;
						player->is_reloading = false;
					}

					
				}			
				
				player->_time_in_reload = 0.f;
			}
			else
			{
				player->_time_in_reload += dt.asSeconds();

				bool isPlaying = false;
				context->game->Channels->at(player->reload_sound_channel_id)->isPlaying(&isPlaying);
				if(isPlaying)
				{
					FMOD_VECTOR pos;
					pos.z = 0;
					pos.x = player->body->GetPosition().x;
					pos.y = player->body->GetPosition().y;
					FMOD_RESULT r = context->game->Channels->at(player->reload_sound_channel_id)->set3DAttributes(&pos, 0, 0);
					if (r != FMOD_OK)
					{
						std::cout << FMOD_ErrorString(r) << " -\"Reload\" Sound 3D positioning on Update" << std::endl;
					}
				}	
			}
		}
		else
		{
			if (player->reload_sound_channel_id != -1)
			{
				context->game->Channels->at(player->reload_sound_channel_id)->stop();
				player->reload_sound_channel_id = -1;
			}
		}

		FMOD_VECTOR playerPos;
		playerPos.x = player->body->GetPosition().x;
		playerPos.y = player->body->GetPosition().y;
		playerPos.z = 0;
		
		int num = 0;
		context->game->lowSoundSystem->get3DNumListeners(&num);
		for (int i = 0; i <= num; i++)
		{
			context->game->lowSoundSystem->set3DListenerAttributes(i, &playerPos, 0, 0, 0);
		}
		

		if (!player->shooting_sound_channel_ids->empty())
		{
			for (size_t i = 0; i < player->shooting_sound_channel_ids->size(); i++)
			{
				if (player->shooting_sound_channel_ids->at(i) != -1)
				{
					bool isPlaying = false;

					if (context->game->Channels->at(player->shooting_sound_channel_ids->at(i)) != NULL)
					{
						context->game->Channels->at(player->shooting_sound_channel_ids->at(i))->isPlaying(&isPlaying);
						if (isPlaying)
						{
							FMOD_VECTOR pos;
							pos.z = 0;
							pos.x = player->body->GetPosition().x;
							pos.y = player->body->GetPosition().y;
							context->game->Channels->at(player->shooting_sound_channel_ids->at(i))->set3DAttributes(&pos, 0);
						}
						else
						{
							auto it = std::find(player->shooting_sound_channel_ids->begin(), player->shooting_sound_channel_ids->end(), player->shooting_sound_channel_ids->at(i));
							player->shooting_sound_channel_ids->erase(it);
						}
					}
				}
			}
		}
		projObj->Update(dt);

		dynamic_cast<GUI::Label*>(PlayerUI->GetComponentByName("weapon_name"))->text.setString(player->currentWeapon->name);
		dynamic_cast<GUI::Label*>(PlayerUI->GetComponentByName("health"))->text.setString(std::to_string(static_cast<int>(player->health)));

		if (player->currentWeapon != NULL)
		{
			dynamic_cast<GUI::Label*>(PlayerUI->GetComponentByName("ammo_in_clip"))->text.setString(std::to_string(player->currentWeapon->ammoInTheClip)+"/");
			dynamic_cast<GUI::Label*>(PlayerUI->GetComponentByName("clips"))->text.setString(std::to_string(player->currentWeapon->clips));
		}
		
		if (!PlayerUI->Components->empty())
		{
			for (size_t i = 0; i < PlayerUI->Components->size(); i++)
			{
				PlayerUI->Components->at(i)->SetPosition(sf::Vector2f(50 + PlayerUI->Components->at(i)->GetOriginalPosition().x + player->GetObjectPosition().x - (SCREENWIDTH / 2), PlayerUI->Components->at(i)->GetOriginalPosition().y + player->GetObjectPosition().y - (SCREENHEIGHT / 2)));
			}
		}

		sf::View view;
		view.setSize(SCREENWIDTH, SCREENHEIGHT);
		view.setCenter(player->GetObjectPosition());;
		context->window->setView(view);
	
		this->Current_area_id = player->area_id;

		//do before updating to avoid pointless updates
		this->finishDestoy();

		if (!StateObjects->empty())
		{
			size_t objAmount = StateObjects->size();
			for (size_t i = 0; i < objAmount; i++)
			{
				if (this->StateObjects->at(i)->physBodyInitialized)
				{
					if (this->StateObjects->at(i)->area_id != this->Current_area_id)
					{
						this->StateObjects->at(i)->body->SetActive(false);
					}
					else
					{
						this->StateObjects->at(i)->body->SetActive(true);
					}
				}
				StateObjects->at(i)->Update(dt);
				if (PropPhysics*pp = dynamic_cast<PropPhysics*>(StateObjects->at(i)))
				{
					if (StateObjects->at(i)->body->GetLinearVelocity().x != 0 || StateObjects->at(i)->body->GetLinearVelocity().y != 0)
					{
						if (pp->StateSoundChannelId == -1)
						{
							if (pp->GetMaterialTypeMovementSoundName() != "")
							{
								PlaySound(pp->GetMaterialTypeMovementSoundName(), pp->StateSoundChannelId);
							}

							FMOD_VECTOR pos;
							pos.z = 0;
							pos.x = pp->body->GetPosition().x;
							pos.y = pp->body->GetPosition().y;
							FMOD_RESULT r = context->game->Channels->at(pp->StateSoundChannelId)->set3DAttributes(&pos, 0, 0);
							if (r != FMOD_OK)
							{
								std::cout << FMOD_ErrorString(r) << std::endl;
							}
						}
						else
						{
							FMOD_VECTOR pos;
							pos.z = 0;
							pos.x = pp->body->GetPosition().x;
							pos.y = pp->body->GetPosition().y;
							FMOD_RESULT r = context->game->Channels->at(pp->StateSoundChannelId)->set3DAttributes(&pos, 0, 0);
							if (r != FMOD_OK)
							{
								std::cout << FMOD_ErrorString(r) << std::endl;
							}

							
							/*Channels->at(pp->StateSoundChannelId)->setVolume(50);*/
						}

					}

					else
					{
						StateObjects->at(i)->body->SetAwake(false);
						if (pp->StateSoundChannelId != -1)
						{
							FMOD_RESULT r = context->game->Channels->at(pp->StateSoundChannelId)->stop();
							pp->StateSoundChannelId = -1;
						}
					}

				}

				else if (Decal*d = dynamic_cast<Decal*>(StateObjects->at(i)))
				{
					if (d->IsDone())
					{
						if (std::find(StateObjects->begin(), StateObjects->end(), StateObjects->at(i)) != StateObjects->end())
						{
							StateObjects->erase(std::find(StateObjects->begin(), StateObjects->end(), StateObjects->at(i)));
							if (objAmount > 0)
							{
								objAmount--;
							}
							else
							{
								break;
							}
						}
					}
				}

				else if (SoundSourceObject*sso = dynamic_cast<SoundSourceObject*>(StateObjects->at(i)))
				{
					if (sso->sound_is_active)
					{
						if (sso->sound_channel_id == -1)
						{
							int ch_id = 0;
							this->PlaySound(sso->sound_name, ch_id);
							sso->sound_channel_id = ch_id;
						}
						else
						{
							bool isPlaying = false;
							context->game->Channels->at(sso->sound_channel_id)->isPlaying(&isPlaying);
							if (!isPlaying)
							{
								if (sso->sound_is_looped)
								{
									int ch_id = 0;
									this->PlaySound(sso->sound_name, ch_id);
									sso->sound_channel_id = ch_id;
								}
							}
							else
							{
								FMOD_VECTOR pos;
								pos.x = sso->GetObjectPosition().x;
								pos.y = sso->GetObjectPosition().y;
								pos.z = 0;
								context->game->Channels->at(sso->sound_channel_id)->set3DAttributes(&pos, 0);
							}
						}
					}
					else
					{
						if (sso->sound_channel_id != -1)
						{
							bool isPlaying = false;
							context->game->Channels->at(sso->sound_channel_id)->isPlaying(&isPlaying);
							if (isPlaying)
							{
								context->game->Channels->at(sso->sound_channel_id)->stop();
								sso->sound_channel_id = -1;
							}
						}

					}
				}

				else if (npc_zombie_base*nz = dynamic_cast<npc_zombie_base*>(StateObjects->at(i)))
				{
					if (nz->isDead())
					{
						if (nz->physBodyInitialized)
						{
							nz->body->GetWorld()->DestroyBody(nz->body);
							nz->physBodyInitialized = false;
						}
						
					}
					else if (nz->body->GetLinearVelocity().x != 0 || nz->body->GetLinearVelocity().y != 0)
					{

						if (nz->footsteps_sound_channel_id >= 0)
						{
							bool isPlaying = false;
							context->game->Channels->at(nz->footsteps_sound_channel_id)->isPlaying(&isPlaying);
							if (!isPlaying)
							{
								if (nz->time_footstep_elapsed >= nz->time_per_footstep)
								{
									int channel_id = 0;
									std::string filename = "";
									if (nz->footstep_sound_type == MAT_SOUND_TYPE_CONCRETE)
									{
										std::string name = MAT_SOUND_TYPE_CONCRETE_NAME;
										filename = name + std::to_string(m_get_random_number(1, 4));
									}
									if (nz->footstep_sound_type == MAT_SOUND_TYPE_CHAINLINK)
									{
										std::string name = MAT_SOUND_TYPE_CHAINLINK_NAME;
										filename = name + std::to_string(m_get_random_number(1, 4));
									}
									if (nz->footstep_sound_type == MAT_SOUND_TYPE_DIRT)
									{
										std::string name = MAT_SOUND_TYPE_DIRT_NAME;
										filename = name + std::to_string(m_get_random_number(1, 4));
									}
									if (nz->footstep_sound_type == MAT_SOUND_TYPE_DUCT)
									{
										std::string name = MAT_SOUND_TYPE_DUCT_NAME;
										filename = name + std::to_string(m_get_random_number(1, 4));
									}
									if (nz->footstep_sound_type == MAT_SOUND_TYPE_GRASS)
									{
										std::string name = MAT_SOUND_TYPE_GRASS_NAME;
										filename = name + std::to_string(m_get_random_number(1, 4));
									}
									if (nz->footstep_sound_type == MAT_SOUND_TYPE_GRAVEL)
									{
										std::string name = MAT_SOUND_TYPE_GRAVEL_NAME;
										filename = name + std::to_string(m_get_random_number(1, 4));
									}
									if (nz->footstep_sound_type == MAT_SOUND_TYPE_LADDER)
									{
										std::string name = MAT_SOUND_TYPE_LADDER_NAME;
										filename = name + std::to_string(m_get_random_number(1, 4));
									}
									if (nz->footstep_sound_type == MAT_SOUND_TYPE_METAL)
									{
										std::string name = MAT_SOUND_TYPE_METAL_NAME;
										filename = name + std::to_string(m_get_random_number(1, 4));
									}
									if (nz->footstep_sound_type == MAT_SOUND_TYPE_METALGRATE)
									{
										std::string name = MAT_SOUND_TYPE_METALGRATE_NAME;
										filename = name + std::to_string(m_get_random_number(1, 4));
									}
									if (nz->footstep_sound_type == MAT_SOUND_TYPE_MUD)
									{
										std::string name = MAT_SOUND_TYPE_MUD_NAME;
										filename = name + std::to_string(m_get_random_number(1, 4));
									}
									if (nz->footstep_sound_type == MAT_SOUND_TYPE_SAND)
									{
										std::string name = MAT_SOUND_TYPE_SAND_NAME;
										filename = name + std::to_string(m_get_random_number(1, 4));
									}
									if (nz->footstep_sound_type == MAT_SOUND_TYPE_SLOSH)
									{
										std::string name = MAT_SOUND_TYPE_SLOSH_NAME;
										filename = name + std::to_string(m_get_random_number(1, 4));
									}
									if (nz->footstep_sound_type == MAT_SOUND_TYPE_TILE)
									{
										std::string name = MAT_SOUND_TYPE_TILE_NAME;
										filename = name + std::to_string(m_get_random_number(1, 4));
									}
									if (nz->footstep_sound_type == MAT_SOUND_TYPE_WADE)
									{
										std::string name = MAT_SOUND_TYPE_WADE_NAME;
										filename = name + std::to_string(m_get_random_number(1, 4));
									}
									if (nz->footstep_sound_type == MAT_SOUND_TYPE_WOOD)
									{
										std::string name = MAT_SOUND_TYPE_WOOD_NAME;
										filename = name + std::to_string(m_get_random_number(1, 4));
									}
									if (nz->footstep_sound_type == MAT_SOUND_TYPE_WOODPANEL)
									{
										std::string name = MAT_SOUND_TYPE_WOODPANEL_NAME;
										filename = name + std::to_string(m_get_random_number(1, 4));
									}




									this->PlaySound(filename, nz->footsteps_sound_channel_id);
									nz->time_footstep_elapsed = 0.f;
								}
								else
								{
									nz->time_footstep_elapsed += dt.asSeconds();
								}
							}
							else
							{
								if (nz->time_footstep_elapsed >= nz->time_per_footstep)
								{
									int channel_id = 0;
									std::string filename = "";
									if (nz->footstep_sound_type == MAT_SOUND_TYPE_CONCRETE)
									{
										std::string name = MAT_SOUND_TYPE_CONCRETE_NAME;
										filename = name + std::to_string(m_get_random_number(1, 4));
									}
									if (nz->footstep_sound_type == MAT_SOUND_TYPE_CHAINLINK)
									{
										std::string name = MAT_SOUND_TYPE_CHAINLINK_NAME;
										filename = name + std::to_string(m_get_random_number(1, 4));
									}
									if (nz->footstep_sound_type == MAT_SOUND_TYPE_DIRT)
									{
										std::string name = MAT_SOUND_TYPE_DIRT_NAME;
										filename = name + std::to_string(m_get_random_number(1, 4));
									}
									if (nz->footstep_sound_type == MAT_SOUND_TYPE_DUCT)
									{
										std::string name = MAT_SOUND_TYPE_DUCT_NAME;
										filename = name + std::to_string(m_get_random_number(1, 4));
									}
									if (nz->footstep_sound_type == MAT_SOUND_TYPE_GRASS)
									{
										std::string name = MAT_SOUND_TYPE_GRASS_NAME;
										filename = name + std::to_string(m_get_random_number(1, 4));
									}
									if (nz->footstep_sound_type == MAT_SOUND_TYPE_GRAVEL)
									{
										std::string name = MAT_SOUND_TYPE_GRAVEL_NAME;
										filename = name + std::to_string(m_get_random_number(1, 4));
									}
									if (nz->footstep_sound_type == MAT_SOUND_TYPE_LADDER)
									{
										std::string name = MAT_SOUND_TYPE_LADDER_NAME;
										filename = name + std::to_string(m_get_random_number(1, 4));
									}
									if (nz->footstep_sound_type == MAT_SOUND_TYPE_METAL)
									{
										std::string name = MAT_SOUND_TYPE_METAL_NAME;
										filename = name + std::to_string(m_get_random_number(1, 4));
									}
									if (nz->footstep_sound_type == MAT_SOUND_TYPE_METALGRATE)
									{
										std::string name = MAT_SOUND_TYPE_METALGRATE_NAME;
										filename = name + std::to_string(m_get_random_number(1, 4));
									}
									if (nz->footstep_sound_type == MAT_SOUND_TYPE_MUD)
									{
										std::string name = MAT_SOUND_TYPE_MUD_NAME;
										filename = name + std::to_string(m_get_random_number(1, 4));
									}
									if (nz->footstep_sound_type == MAT_SOUND_TYPE_SAND)
									{
										std::string name = MAT_SOUND_TYPE_SAND_NAME;
										filename = name + std::to_string(m_get_random_number(1, 4));
									}
									if (nz->footstep_sound_type == MAT_SOUND_TYPE_SLOSH)
									{
										std::string name = MAT_SOUND_TYPE_SLOSH_NAME;
										filename = name + std::to_string(m_get_random_number(1, 4));
									}
									if (nz->footstep_sound_type == MAT_SOUND_TYPE_TILE)
									{
										std::string name = MAT_SOUND_TYPE_TILE_NAME;
										filename = name + std::to_string(m_get_random_number(1, 4));
									}
									if (nz->footstep_sound_type == MAT_SOUND_TYPE_WADE)
									{
										std::string name = MAT_SOUND_TYPE_WADE_NAME;
										filename = name + std::to_string(m_get_random_number(1, 4));
									}
									if (nz->footstep_sound_type == MAT_SOUND_TYPE_WOOD)
									{
										std::string name = MAT_SOUND_TYPE_WOOD_NAME;
										filename = name + std::to_string(m_get_random_number(1, 4));
									}
									if (nz->footstep_sound_type == MAT_SOUND_TYPE_WOODPANEL)
									{
										std::string name = MAT_SOUND_TYPE_WOODPANEL_NAME;
										filename = name + std::to_string(m_get_random_number(1, 4));
									}
									this->PlaySound(filename, nz->footsteps_sound_channel_id);
									nz->time_footstep_elapsed = 0.f;
								}
								else
								{
									
								}
							}
							
						}
						else
						{
							int channel_id = 0;
							std::string filename = "";
							if (player->footstep_sound_type == MAT_SOUND_TYPE_CONCRETE)
							{
								std::string name = MAT_SOUND_TYPE_CONCRETE_NAME;
								filename = name + std::to_string(m_get_random_number(1, 4));
							}
							if (player->footstep_sound_type == MAT_SOUND_TYPE_CHAINLINK)
							{
								std::string name = MAT_SOUND_TYPE_CHAINLINK_NAME;
								filename = name + std::to_string(m_get_random_number(1, 4));
							}
							if (player->footstep_sound_type == MAT_SOUND_TYPE_DIRT)
							{
								std::string name = MAT_SOUND_TYPE_DIRT_NAME;
								filename = name + std::to_string(m_get_random_number(1, 4));
							}
							if (player->footstep_sound_type == MAT_SOUND_TYPE_DUCT)
							{
								std::string name = MAT_SOUND_TYPE_DUCT_NAME;
								filename = name + std::to_string(m_get_random_number(1, 4));
							}
							if (player->footstep_sound_type == MAT_SOUND_TYPE_GRASS)
							{
								std::string name = MAT_SOUND_TYPE_GRASS_NAME;
								filename = name + std::to_string(m_get_random_number(1, 4));
							}
							if (player->footstep_sound_type == MAT_SOUND_TYPE_GRAVEL)
							{
								std::string name = MAT_SOUND_TYPE_GRAVEL_NAME;
								filename = name + std::to_string(m_get_random_number(1, 4));
							}
							if (player->footstep_sound_type == MAT_SOUND_TYPE_LADDER)
							{
std::string name = MAT_SOUND_TYPE_LADDER_NAME;
filename = name + std::to_string(m_get_random_number(1, 4));
							}
							if (player->footstep_sound_type == MAT_SOUND_TYPE_METAL)
							{
								std::string name = MAT_SOUND_TYPE_METAL_NAME;
								filename = name + std::to_string(m_get_random_number(1, 4));
							}
							if (player->footstep_sound_type == MAT_SOUND_TYPE_METALGRATE)
							{
								std::string name = MAT_SOUND_TYPE_METALGRATE_NAME;
								filename = name + std::to_string(m_get_random_number(1, 4));
							}
							if (player->footstep_sound_type == MAT_SOUND_TYPE_MUD)
							{
								std::string name = MAT_SOUND_TYPE_MUD_NAME;
								filename = name + std::to_string(m_get_random_number(1, 4));
							}
							if (player->footstep_sound_type == MAT_SOUND_TYPE_SAND)
							{
								std::string name = MAT_SOUND_TYPE_SAND_NAME;
								filename = name + std::to_string(m_get_random_number(1, 4));
							}
							if (player->footstep_sound_type == MAT_SOUND_TYPE_SLOSH)
							{
								std::string name = MAT_SOUND_TYPE_SLOSH_NAME;
								filename = name + std::to_string(m_get_random_number(1, 4));
							}
							if (player->footstep_sound_type == MAT_SOUND_TYPE_TILE)
							{
								std::string name = MAT_SOUND_TYPE_TILE_NAME;
								filename = name + std::to_string(m_get_random_number(1, 4));
							}
							if (player->footstep_sound_type == MAT_SOUND_TYPE_WADE)
							{
								std::string name = MAT_SOUND_TYPE_WADE_NAME;
								filename = name + std::to_string(m_get_random_number(1, 4));
							}
							if (player->footstep_sound_type == MAT_SOUND_TYPE_WOOD)
							{
								std::string name = MAT_SOUND_TYPE_WOOD_NAME;
								filename = name + std::to_string(m_get_random_number(1, 4));
							}
							if (player->footstep_sound_type == MAT_SOUND_TYPE_WOODPANEL)
							{
								std::string name = MAT_SOUND_TYPE_WOODPANEL_NAME;
								filename = name + std::to_string(m_get_random_number(1, 4));
							}




							this->PlaySound(filename, nz->footsteps_sound_channel_id);
							nz->time_footstep_elapsed = 0.f;
						}

						nz->time_footstep_elapsed += dt.asSeconds();
						FMOD_VECTOR pos;
						pos.x = nz->body->GetPosition().x;
						pos.y = nz->body->GetPosition().y;
						pos.z = 0;
						context->game->Channels->at(nz->footsteps_sound_channel_id)->set3DAttributes(&pos, 0);

						if (nz->zombie_footstep_sound_channel_id >= 0)
						{
							bool isPlaying = false;
							context->game->Channels->at(nz->zombie_footstep_sound_channel_id)->isPlaying(&isPlaying);
							if (!isPlaying)
							{
								if (nz->time_footstep_elapsed >= nz->time_per_footstep)
								{
									context->game->PlaySound("zombie_foot" + std::to_string(m_get_random_number(1, 3)), nz->zombie_footstep_sound_channel_id);
								}
							}
							else
							{

								if (nz->time_footstep_elapsed >= nz->time_per_footstep)
								{
									context->game->PlaySound("zombie_foot" + std::to_string(m_get_random_number(1, 3)), nz->zombie_footstep_sound_channel_id);
								}

							}
						}
						else
						{
							context->game->PlaySound("zombie_foot" + std::to_string(m_get_random_number(1, 3)), nz->zombie_footstep_sound_channel_id);
						}

						nz->time_footstep_elapsed += dt.asSeconds();
						FMOD_VECTOR pos1;
						pos1.x = nz->body->GetPosition().x;
						pos1.y = nz->body->GetPosition().y;
						pos1.z = 0;
						context->game->Channels->at(nz->zombie_footstep_sound_channel_id)->set3DAttributes(&pos1, 0);
					}

					if (nz->voice_sound_channel_id != -1)
					{
						if (context->game->Channels->at(nz->voice_sound_channel_id) != NULL)
						{
							bool isPlaying = false;
							context->game->Channels->at(nz->voice_sound_channel_id)->isPlaying(&isPlaying);
							if (isPlaying)
							{
								FMOD_VECTOR pos;
								pos.z = 0;
								pos.x = nz->body->GetPosition().x;
								pos.y = nz->body->GetPosition().y;


								FMOD_RESULT r=  context->game->Channels->at(nz->voice_sound_channel_id)->set3DAttributes(&pos, 0);
								if (r != FMOD_OK)
								{
									std::cout << FMOD_ErrorString(r) << " -\"Zombie Voice\" Channel 3D positioning" << std::endl;
								}
							}
						}
					}
				}

				if (i >= objAmount) { break; }
			}
		}
	
		for (size_t i = 0; i < pixelParticleSystems->size(); i++)
		{
			pixelParticleSystems->at(i).update(dt);

		}
		cursorParticles.update(dt);

		

		context->game->lowSoundSystem->update();

		/*for (size_t i = 0; i < context->game->Channels->size(); i++)
		{
			
			if (context->game->Channels->at(i) != NULL)
			{
				bool ip = false;
				context->game->Channels->at(i)->isPlaying(&ip);
				if (ip)
				{
					float v = 0;
					context->game->Channels->at(i)->getVolume(&v);

					std::cout << v << std::endl;
				}
				
			}
		}*/
	}

	//can be used instead of ~
	virtual void release()
	{
		if (!StateObjects->empty())
		{
			for (size_t i = 0; i < StateObjects->size(); i++)
			{
				delete StateObjects->at(i);
			}
		}
		//delete StateObjects;
		delete player;
		/*world.~b2World();*/
		current_map.~basic_string();
	}
	~PlayState()
	{
		/*if (!StateObjects->empty())
		{
			for (size_t i = 0; i < StateObjects->size(); i++)
			{
				delete StateObjects->at(i);
			}
		}
		StateObjects->~vector();
		delete player;*/
	}
};