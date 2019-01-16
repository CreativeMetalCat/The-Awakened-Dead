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

public:
	bool _map_is_loaded = false;

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
	std::vector<FMOD::Channel*>*Channels = new std::vector<FMOD::Channel*>(MAX_SOUND_CHANNELS_COUNT);
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

		if (addToStateObjectContainer)
		{
			StateObjects->push_back(object);
		}

	}
	void PlaySound(std::string name)override
	{
		for (size_t i = 0; i < Channels->size(); i++)
		{
			bool res;
			Channels->at(i)->isPlaying(&res);
			if (Channels->at(i) == NULL)
			{
				context->game->lowSoundSystem->playSound(context->game->Resources->getSoundResourceDataByName(name)->sound, 0, false, &Channels->at(i));

				break;
			}
			else if (res == false)
			{
				context->game->lowSoundSystem->playSound(context->game->Resources->getSoundResourceDataByName(name)->sound, 0, false, &Channels->at(i));

				break;
			}
		}
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

								sf::Vector2f pos = sf::Vector2f(64 * cTileIdx, 64 * cTileIdy);

								SceneTile*ta = new SceneTile
								(
									pos,
									sf::Sprite(context->game->Resources->getTextureResourceDataByName(std::to_string(gid))->texture),
									context->game->Resources->getTextureResourceDataByName(std::to_string(gid))->texture.getSize().x,
									context->game->Resources->getTextureResourceDataByName(std::to_string(gid))->texture.getSize().y,
									layer_area_id
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

								sf::Vector2f pos = sf::Vector2f(64 * cTileIdx, 64 * cTileIdy);
								SceneTile*ta = new SceneTile
								(
									pos,
									sf::Sprite(context->game->Resources->getTextureResourceDataByName(std::to_string(gid))->texture),
									context->game->Resources->getTextureResourceDataByName(std::to_string(gid))->texture.getSize().x,
									context->game->Resources->getTextureResourceDataByName(std::to_string(gid))->texture.getSize().y,
									layer_area_id
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
								if (type == "info_player_start")
								{
									float posX = obj->FindAttribute("x")->FloatValue();
									float posY = obj->FindAttribute("y")->FloatValue();
									this->player->SetObjectPosition(sf::Vector2f(posX, posY));
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
									this->StateObjects->push_back(new TextObject(sf::Vector2f(posX, posY), text, color, sf::Text(text, context->game->Resources->getFontResourceDataByName(fontName)->font, pixelSize),true,layer_area_id));
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

				}
				else if (dynamic_cast<TextObject*>(StateObjects->at(i)))
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
					senseShape.SetAsBox(StateObjects->at(i)->GetObjectRectangle().width, StateObjects->at(i)->GetObjectRectangle().height);

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

					StateObjects->at(i)->body->SetMassData(&propMass);
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
		Animation::SpritesAnimation*pistol_move = new  Animation::SpritesAnimation(true, 0.2f, "solder_pistol_move");
		for (int i = 0; i < 20; i++)
		{
			pistol_move->AddFrame(sf::Sprite(context->game->Resources->getTextureResourceDataByName("solder_pistol_move_" + std::to_string(i))->texture));
		}
		player->spritesAnimations->addAnimation(pistol_move);
		player->Init();

		npc_zombie*z = new npc_zombie(sf::Vector2f(500, 500), 1.f, 100, 100);
		z->Init();
		z->OnCollision = [this, z](Object*object)
		{
			z->onCollision(object, this->context, "PlayState");
		};




		projObj = new projectile(sf::Vector2f(0, 0), 100.f, 100.f, 50.0f, 2.0f, sf::Sprite(context->game->Resources->getTextureResourceDataByName("proj")->texture));


		Animation::SpritesAnimation*zombie_idle = new  Animation::SpritesAnimation(true, 0.2f, "skeleton_idle");
		for (int i = 0; i < 17; i++)
		{
			zombie_idle->AddFrame(sf::Sprite(context->game->Resources->getTextureResourceDataByName("skeleton-idle_" + std::to_string(i))->texture));
		}
		z->spritesAnimations->addAnimation(zombie_idle);
		z->Init();
		z->SetAnimation("skeleton_idle");




		this->StateObjects->push_back(z);




		//GUI

		GUI::Label*l1 = new GUI::Label("weapon_name", "0", sf::Color::Red, context->game->Resources->getFontResourceDataByName("calibri")->font, 80, context->game->Resources->getTextureResourceDataByName("textBoxTexture1")->texture);
		l1->Init();
		this->PlayerUI->Components->push_back(l1);

		GUI::Label*l2 = new GUI::Label("health", "0", sf::Color::Red, context->game->Resources->getFontResourceDataByName("calibri")->font, 80, context->game->Resources->getTextureResourceDataByName("textBoxTexture1")->texture);
		l2->SetPosition(sf::Vector2f(10, SCREENHEIGHT - 200));
		l2->Init();
		this->PlayerUI->Components->push_back(l2);


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

	bool CheckObjectCollision(Object*&object)
	{
		if (StateObjects->empty()) { return false; }
		bool res = false;
		for (b2ContactEdge* ce = object->body->GetContactList(); ce; ce->next)
		{
			b2Contact* c = ce->contact;
			for (size_t i = 0; i < StateObjects->size(); i++)
			{
				if (object != StateObjects->at(i) && c->GetFixtureA() != c->GetFixtureB())
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
		if (event.key.code == sf::Keyboard::Num1&&event.type == sf::Event::EventType::KeyPressed)
		{
			LoadMap("td_free_tv.tmx");
		}
		if (event.key.code == sf::Keyboard::Num2&&event.type == sf::Event::EventType::KeyPressed)
		{
			LoadMap("t_t1.tmx");
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
							FMOD_RESULT res;
							res = context->game->lowSoundSystem->playSound(context->game->Resources->getSoundResourceDataByName("pistol_fire2")->sound, 0, false, &Channels->at(i));
							if (res != FMOD_OK)
							{
								std::cout << "Error playing sound. Name: " << context->game->Resources->getSoundResourceDataByName("pistol_fire2")->name.c_str() << "Filename: " << context->game->Resources->getSoundResourceDataByName("pistol_fire2")->filename.c_str() << "Error: " << FMOD_ErrorString(res) << std::endl;
							}

						}
						if (player->currentWeapon->weaponType == WEAPON_TYPE_TAD_RIFLE)
						{
							FMOD_RESULT res;
							res = context->game->lowSoundSystem->playSound(context->game->Resources->getSoundResourceDataByName("fire1")->sound, 0, false, &Channels->at(i));
							if (res != FMOD_OK)
							{
								std::cout << "Error playing sound. Name: " << context->game->Resources->getSoundResourceDataByName("fire1")->name.c_str() << "Filename: " << context->game->Resources->getSoundResourceDataByName("fire1")->filename.c_str() << "Error: " << FMOD_ErrorString(res) << std::endl;
							}


							break;
						}
						else if (res == false)
						{
							if (player->currentWeapon->weaponType == WEAPON_TYPE_TAD_PISTOL)
							{
								FMOD_RESULT res;
								res = context->game->lowSoundSystem->playSound(context->game->Resources->getSoundResourceDataByName("pistol_fire2")->sound, 0, false, &Channels->at(i));
								if (res != FMOD_OK)
								{
									std::cout << "Error playing sound. Name: " << context->game->Resources->getSoundResourceDataByName("pistol_fire2")->name.c_str() << "Filename: " << context->game->Resources->getSoundResourceDataByName("pistol_fire2")->filename.c_str() << "Error: " << FMOD_ErrorString(res) << std::endl;
								}

							}
							if (player->currentWeapon->weaponType == WEAPON_TYPE_TAD_RIFLE)
							{
								FMOD_RESULT res;
								res = context->game->lowSoundSystem->playSound(context->game->Resources->getSoundResourceDataByName("fire1")->sound, 0, false, &Channels->at(i));
								if (res != FMOD_OK)
								{
									std::cout << "Error playing sound. Name: " << context->game->Resources->getSoundResourceDataByName("fire1")->name.c_str() << "Filename: " << context->game->Resources->getSoundResourceDataByName("fire1")->filename.c_str() << "Error: " << FMOD_ErrorString(res) << std::endl;
								}

							}

						}
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
				proje* bullet = new proje(sf::Vector2f(0, 0), 10.f, 10.f, 50.0f, player->currentWeapon->projectileSpeed * 10, sf::Sprite(context->game->Resources->getTextureResourceDataByName("proj")->texture));

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
			else if (player->currentWeapon->weaponType == WEAPON_TYPE_TAD_PISTOL)
			{

				projectile* bullet = new projectile(sf::Vector2f(0, 0), 10.f, 10.f, 500.0f, player->currentWeapon->projectileSpeed, sf::Sprite(context->game->Resources->getTextureResourceDataByName("proj")->texture));

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
			/*	if (!Channels->empty())
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

				}*/

			for (size_t i = 0; i < StateObjects->size(); i++)
			{
				if (dynamic_cast<PropPhysics*>(StateObjects->at(i)))
				{
					StateObjects->at(i)->applyImpulse(b2Vec2(100, 100));
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
		else if (!WalkRight && !WalkLeft)
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
			world.Step(1.f / dt.asSeconds(), 35, 20);
		}

		player->Update(dt);
		projObj->Update(dt);

		dynamic_cast<GUI::Label*>(PlayerUI->GetComponentByName("weapon_name"))->text.setString(player->currentWeapon->name);
		dynamic_cast<GUI::Label*>(PlayerUI->GetComponentByName("health"))->text.setString(std::to_string(static_cast<int>(player->health)));

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
				if (this->StateObjects->at(i)->physBodyInitialized)
				{
					if (this->StateObjects->at(i)->area_id != this->Current_area_id)
					{
						this->StateObjects->at(i)->body->SetAwake(false);
					}
				}
				StateObjects->at(i)->Update(dt);
				if (PropPhysics*pp = dynamic_cast<PropPhysics*>(StateObjects->at(i)))
				{

					if (StateObjects->at(i)->body->GetLinearVelocity().x != 0 || StateObjects->at(i)->body->GetLinearVelocity().y != 0)
					{
						if (pp->StateSoundChannelId == -1)
						{
							for (size_t ind = 0; ind < Channels->size(); ind++)
							{
								bool res;
								Channels->at(ind)->isPlaying(&res);
								if (Channels->at(ind) == NULL)
								{
									if (pp->MaterialType == MAT_TYPE_PLASTIC_BARREL)
									{
										if (pp->mass >= 100.f)
										{
											context->game->lowSoundSystem->playSound(context->game->Resources->getSoundResourceDataByName(MAT_TYPE_PLASTIC_BARREL_HARD_NAME)->sound, 0, false, &Channels->at(ind));
										}
										else
										{
											context->game->lowSoundSystem->playSound(context->game->Resources->getSoundResourceDataByName(MAT_TYPE_PLASTIC_BARREL_LIGHT_NAME)->sound, 0, false, &Channels->at(ind));
										}
									}

									if (pp->MaterialType == MAT_TYPE_PLASTIC_BOX)
									{
										if (pp->mass >= 100.f)
										{

											context->game->lowSoundSystem->playSound(context->game->Resources->getSoundResourceDataByName(MAT_TYPE_PLASTIC_BOX_HARD_NAME)->sound, 0, false, &Channels->at(ind));
										}
										else
										{
											context->game->lowSoundSystem->playSound(context->game->Resources->getSoundResourceDataByName(MAT_TYPE_PLASTIC_BOX_LIGHT_NAME)->sound, 0, false, &Channels->at(ind));
										}
									}

									if (pp->MaterialType == MAT_TYPE_WOOD)
									{
										if (pp->mass >= 100.f)
										{

											context->game->lowSoundSystem->playSound(context->game->Resources->getSoundResourceDataByName(MAT_TYPE_WOOD_BOX_HARD_NAME)->sound, 0, false, &Channels->at(ind));
										}
										else
										{
											context->game->lowSoundSystem->playSound(context->game->Resources->getSoundResourceDataByName(MAT_TYPE_WOOD_BOX_LIGHT_NAME)->sound, 0, false, &Channels->at(ind));
										}

									}

									pp->StateSoundChannelId = ind;
									break;
								}
								else if (res == false)
								{
									if (pp->MaterialType == MAT_TYPE_PLASTIC_BARREL)
									{
										if (pp->mass >= 100.f)
										{

											context->game->lowSoundSystem->playSound(context->game->Resources->getSoundResourceDataByName(MAT_TYPE_PLASTIC_BARREL_HARD_NAME)->sound, 0, false, &Channels->at(ind));
										}
										else
										{
											context->game->lowSoundSystem->playSound(context->game->Resources->getSoundResourceDataByName(MAT_TYPE_PLASTIC_BARREL_LIGHT_NAME)->sound, 0, false, &Channels->at(ind));
										}
									}

									if (pp->MaterialType == MAT_TYPE_PLASTIC_BOX)
									{
										if (pp->mass >= 100.f)
										{

											context->game->lowSoundSystem->playSound(context->game->Resources->getSoundResourceDataByName(MAT_TYPE_PLASTIC_BOX_HARD_NAME)->sound, 0, false, &Channels->at(ind));
										}
										else
										{
											context->game->lowSoundSystem->playSound(context->game->Resources->getSoundResourceDataByName(MAT_TYPE_PLASTIC_BOX_LIGHT_NAME)->sound, 0, false, &Channels->at(ind));
										}
									}

									if (pp->MaterialType == MAT_TYPE_WOOD_CRATE)
									{
										if (pp->mass >= 100.f)
										{
											context->game->lowSoundSystem->playSound(context->game->Resources->getSoundResourceDataByName(MAT_TYPE_WOOD_CRATE_HARD_NAME)->sound, 0, false, &Channels->at(ind));
										}
										else
										{
											context->game->lowSoundSystem->playSound(context->game->Resources->getSoundResourceDataByName(MAT_TYPE_WOOD_CRATE_LIGHT_NAME)->sound, 0, false, &Channels->at(ind));
										}

									}
									if (pp->MaterialType == MAT_TYPE_WOOD_PLANK)
									{
										if (pp->mass >= 100.f)
										{
											context->game->lowSoundSystem->playSound(context->game->Resources->getSoundResourceDataByName(MAT_TYPE_WOOD_PLANK_HARD_NAME)->sound, 0, false, &Channels->at(ind));
										}
										else
										{
											context->game->lowSoundSystem->playSound(context->game->Resources->getSoundResourceDataByName(MAT_TYPE_WOOD_PLANK_LIGHT_NAME)->sound, 0, false, &Channels->at(ind));
										}

									}
									if (pp->MaterialType == MAT_TYPE_WOOD_BOX)
									{
										if (pp->mass >= 100.f)
										{
											context->game->lowSoundSystem->playSound(context->game->Resources->getSoundResourceDataByName(MAT_TYPE_WOOD_BOX_HARD_NAME)->sound, 0, false, &Channels->at(ind));
										}
										else
										{
											context->game->lowSoundSystem->playSound(context->game->Resources->getSoundResourceDataByName(MAT_TYPE_WOOD_BOX_LIGHT_NAME)->sound, 0, false, &Channels->at(ind));
										}

									}

									pp->StateSoundChannelId = ind;


									break;
								}

							}
						}
						else
						{
							FMOD_VECTOR pos;
							pos.z = 0;
							pos.x = pp->body->GetPosition().x;
							pos.y = -pp->body->GetPosition().y;
							Channels->at(pp->StateSoundChannelId)->set3DAttributes(&pos, 0, 0);
							/*Channels->at(pp->StateSoundChannelId)->setVolume(50);*/
						}
					}
					else
					{
						StateObjects->at(i)->body->SetAwake(false);
						if (pp->StateSoundChannelId != -1)
						{
							FMOD_RESULT r = Channels->at(pp->StateSoundChannelId)->stop();
							pp->StateSoundChannelId = -1;
						}
					}

				}


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

		FMOD_VECTOR playerPos;
		playerPos.x = player->body->GetPosition().x;
		playerPos.y = -player->body->GetPosition().y;
		playerPos.z = 0;
		context->game->lowSoundSystem->set3DListenerAttributes(0, &playerPos, 0, 0, 0);

		context->game->lowSoundSystem->update();
	}

};