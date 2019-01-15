#pragma once
#include "State.h"
#include "GUI.h"
#include "PixelParticleSystem.h"
#include "SceneActor.h"

#ifndef MAX_SOUND_CHANNELS_COUNT
#define MAX_SOUND_CHANNELS_COUNT 1024
#endif // !MAX_SOUND_CHANNELS_COUNT

//Test class for menu system
class MenuState :public State
{
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
