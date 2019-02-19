#pragma once
#include "State.h"
#include "GUI.h"
#include "PixelParticleSystem.h"
#include "SceneActor.h"
#include "GUI_Button_Load_Map.h"
#include "PlayState.h"

#ifndef MAX_SOUND_CHANNELS_COUNT
#define MAX_SOUND_CHANNELS_COUNT 1024
#endif // !MAX_SOUND_CHANNELS_COUNT

//Test class for menu system
class MenuState :public State
{
private:
	int mouse_move_sound_channel_id = -1;
	bool can_mouse_move_sound_be_played = true;
protected:
	size_t seletion_index = 0;
public:
	/*std::vector<TextObject*>*Options = new std::vector<TextObject*>();*/
	PixelParticleSystem cursorParticles = PixelParticleSystem(3000, sf::Color::Red);

	GUI::Container*container = new GUI::Container();

	GUI::Container*map_loading_menu = new GUI::Container();

	FMOD::Sound *click;
	FMOD::Sound *hover;
	FMOD::Sound*release;
	/*FMOD::Channel *channel;*/
	std::vector<FMOD::Channel*>*Channels = new std::vector<FMOD::Channel*>(MAX_SOUND_CHANNELS_COUNT);
	void Init()override
	{
		map_loading_menu->IsVisible = false;
		map_loading_menu->IsActive = false;

		context->game->lowSoundSystem->createSound("./../sounds/ui/buttonclick.wav", FMOD_3D, 0, &click);
		context->game->lowSoundSystem->createSound("./../sounds/ui/buttonrollover.wav", FMOD_3D, 0, &hover);
		context->game->lowSoundSystem->createSound("./../sounds/ui/buttonclickrelease.wav", FMOD_3D, 0, &release);

		GUI::Button*Button = new GUI::Button("Play", sf::Color::White, context->game->Resources->getFontResourceDataByName("calibri")->font, 64, sf::Sprite(context->game->Resources->getTextureResourceDataByName("textBoxTexture1")->texture));
		sf::IntRect rect = sf::IntRect(Button->GetPosition().x, Button->GetPosition().y, 0, 0);


		cursorParticles.texture = new sf::Texture(context->game->Resources->getTextureResourceDataByName("proj")->texture);
		/*Button->Sprite.setTextureRect(rect);*/

		Button->SetPosition(sf::Vector2<float>(0, 120));
		Button->Action = [this]()
		{
			/*context->game->ActivateState(context->game->GetStateByName("PlayState"));
			context->game->DisableState(context->game->GetStateByName("MenuState"));*/
			map_loading_menu->IsVisible = true;
			map_loading_menu->IsActive = true;
			container->IsActive = false;
			container->IsVisible = false;
		};
		this->container->Components->push_back(Button);

		Button->Init();

		GUI::Button_Load_Map*blm = new GUI::Button_Load_Map("Physics Test", sf::Color::White, context->game->Resources->getFontResourceDataByName("Calibri")->font, 64, sf::Sprite());

		blm->SetPosition(sf::Vector2<float>(300, 120));
		blm->map_path = "td_free_tv.tmx";
		blm->Action = [this,blm]()
		{
			context->game->DisableState(context->game->GetStateByName("MenuState"));
			context->game->ActivateState(context->game->GetStateByName("PlayState"));

			
			dynamic_cast<PlayState*>(context->game->GetStateByName("PlayState"))->current_map = blm->map_path;
		};
		blm->Init();
		this->map_loading_menu->Components->push_back(blm);

		GUI::Button_Load_Map*blm1 = new GUI::Button_Load_Map("AI Test", sf::Color::White, context->game->Resources->getFontResourceDataByName("Calibri")->font, 64, sf::Sprite());

		blm1->SetPosition(sf::Vector2<float>(300, 220));
		blm1->map_path = "ai_zombie_test.tmx";
		blm1->Action = [this, blm1]()
		{
			context->game->DisableState(context->game->GetStateByName("MenuState"));
			context->game->ActivateState(context->game->GetStateByName("PlayState"));


			dynamic_cast<PlayState*>(context->game->GetStateByName("PlayState"))->current_map = blm1->map_path;
		};
		blm1->Init();
		this->map_loading_menu->Components->push_back(blm1);

		GUI::Button_Load_Map*blm2 = new GUI::Button_Load_Map("3D Sound Test", sf::Color::White, context->game->Resources->getFontResourceDataByName("Calibri")->font, 64, sf::Sprite());

		blm2->SetPosition(sf::Vector2<float>(300, 320));
		blm2->map_path = "sound_test.tmx";
		blm2->Action = [this, blm2]()
		{
			context->game->DisableState(context->game->GetStateByName("MenuState"));
			context->game->ActivateState(context->game->GetStateByName("PlayState"));


			dynamic_cast<PlayState*>(context->game->GetStateByName("PlayState"))->current_map = blm2->map_path;
		};
		blm2->Init();
		this->map_loading_menu->Components->push_back(blm2);
		

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
		context->game->PlaySound(name, channel_id);
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

		if (!container->Components->empty()&&container->IsVisible)
		{
			for (size_t i = 0; i < container->Components->size(); i++)
			{

				container->Components->at(i)->Draw(context->window);

				/*if (DEBUG_DRAWCOLLISION)
				{
					sf::VertexArray va;
					va.append(sf::Vertex(sf::Vector2f(container->Components->at(i)->ComponentRectangle.top, container->Components->at(i)->ComponentRectangle.left), sf::Color::Red));
					va.append(sf::Vertex(sf::Vector2f(container->Components->at(i)->ComponentRectangle.top, container->Components->at(i)->ComponentRectangle.left + container->Components->at(i)->ComponentRectangle.width), sf::Color::Red));
					va.append(sf::Vertex(sf::Vector2f(container->Components->at(i)->ComponentRectangle.top + container->Components->at(i)->ComponentRectangle.height, container->Components->at(i)->ComponentRectangle.left + container->Components->at(i)->ComponentRectangle.width), sf::Color::Red));
					va.append(sf::Vertex(sf::Vector2f(container->Components->at(i)->ComponentRectangle.top + container->Components->at(i)->ComponentRectangle.height, container->Components->at(i)->ComponentRectangle.left), sf::Color::Red));
					va.append(sf::Vertex(sf::Vector2f(container->Components->at(i)->ComponentRectangle.top, container->Components->at(i)->ComponentRectangle.left), sf::Color::Red));
					context->window->draw(va);
				}*/
			}
		}
		
		if (!map_loading_menu->Components->empty()&& map_loading_menu->IsVisible)
		{
			for (size_t i = 0; i < map_loading_menu->Components->size(); i++)
			{
				map_loading_menu->Components->at(i)->Draw(context->window);
			}
		}
		context->window->draw(cursorParticles);
	}

	virtual void HandleEvent(sf::Event& event)override
	{
		if (event.type == sf::Event::Closed)
		{
			context->window->close();
		}
		

		if (!map_loading_menu->Components->empty() && map_loading_menu->IsActive)
		{
			if (event.type == sf::Event::EventType::MouseMoved)
			{

				event.mouseMove.x;
				event.mouseMove.y;
				cursorParticles.setEmitter(sf::Vector2f(event.mouseMove.x, event.mouseMove.y));
				if (!map_loading_menu->Components->empty())
				{
					for (size_t i = 0; i < map_loading_menu->Components->size(); i++)
					{
						if (map_loading_menu->Components->at(i)->isSelectable)
						{
							if (map_loading_menu->Components->at(i)->ComponentRectangle.contains(sf::Vector2f(event.mouseMove.x, event.mouseMove.y)))
							{
								map_loading_menu->Select(i);
								seletion_index = i;
								
								
							}
							else
							{
								map_loading_menu->Components->at(i)->UnSelect();
								
							}
						}
					}
				}

			}
			if (event.mouseButton.button == sf::Mouse::Left&&event.type == sf::Event::EventType::MouseButtonPressed)
			{

				for (size_t i = 0; i < map_loading_menu->Components->size(); i++)
				{
					if (map_loading_menu->Components->at(i)->ComponentRectangle.contains(sf::Vector2f(event.mouseButton.x, event.mouseButton.y)))
					{
						map_loading_menu->Components->at(seletion_index)->Activate();
						seletion_index = i;
						if (can_mouse_move_sound_be_played)
						{
							if (mouse_move_sound_channel_id != -1)
							{
								bool isPlaying = false;
								this->context->game->Channels->at(i)->isPlaying(&isPlaying);
								if (!isPlaying)
								{
									this->PlaySound("buttonclick");
								}
							}
							else
							{
								this->PlaySound("buttonclick");
							}
						}
					}
					else
					{

					}
				}
			}
			if (event.mouseButton.button == sf::Keyboard::Down&&event.type == sf::Event::EventType::MouseButtonPressed)
			{
				for (size_t i = 0; i < map_loading_menu->Components->size(); i++)
				{
					map_loading_menu->Components->at(i)->UnSelect();
				}

				map_loading_menu->SelectNext();
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
			{
				for (size_t i = 0; i < map_loading_menu->Components->size(); i++)
				{
					map_loading_menu->Components->at(i)->UnSelect();
				}
				map_loading_menu->SelectPrevious();
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter))
			{
				map_loading_menu->Components->at(seletion_index)->Activate();
			}
		}

		else if (!container->Components->empty() && container->IsActive)
		{

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
							if (can_mouse_move_sound_be_played)
							{
								if (mouse_move_sound_channel_id != -1)
								{
									bool isPlaying = false;
									this->context->game->Channels->at(i)->isPlaying(&isPlaying);
									if (!isPlaying)
									{
										this->PlaySound("buttonclick");
									}
								}
								else
								{
									this->PlaySound("buttonclick");
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

								/*if (can_mouse_move_sound_be_played)
								{
									if (mouse_move_sound_channel_id != -1)
									{
										bool isPlaying = false;
										this->context->game->Channels->at(i)->isPlaying(&isPlaying);
										if(!isPlaying)
										{ 
											this->PlaySound("buttonrollover");
										}
									}
									else
									{
										this->PlaySound("buttonrollover");
									}
								}
								can_mouse_move_sound_be_played = false;*/
							}
							else
							{
								container->Components->at(i)->UnSelect();
								/*can_mouse_move_sound_be_played = true;*/
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
		context->window->setFramerateLimit(60);

		context->game->lowSoundSystem->update();
		/*context->window->setMouseCursorVisible(false);*/
		cursorParticles.update(dt);
	}
};
