#pragma once

#include "State.h"
#include "AnimationDataContainer.h"
#include <iostream>

#ifndef MAX_SOUND_CHANNELS_COUNT
#define MAX_SOUND_CHANNELS_COUNT 1024
#endif // !MAX_SOUND_CHANNELS_COUNT

//class that manages work of the game's bones and musles
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

	//channels used to play sounds
	std::vector<FMOD::Channel*>*Channels = new std::vector<FMOD::Channel*>(MAX_SOUND_CHANNELS_COUNT);

	FMOD::System*lowSoundSystem = NULL;
	sf::RenderWindow window;
	State*CurrentState;
	AnimationDataContainer*animationsData = new AnimationDataContainer();
	ResourceContainer*Resources = new ResourceContainer();


	//Plays sound using one of the channels
	void PlaySound(std::string name)
	{
		for (size_t i = 0; i < Channels->size(); i++)
		{
			bool res;
			Channels->at(i)->isPlaying(&res);
			if (Channels->at(i) == NULL)
			{
				lowSoundSystem->playSound(Resources->getSoundResourceDataByName(name)->sound, 0, false, &Channels->at(i));

				break;
			}
			else if (res == false)
			{
				lowSoundSystem->playSound(Resources->getSoundResourceDataByName(name)->sound, 0, false, &Channels->at(i));

				break;
			}
		}
	}

	//Plays sound using one of the channels and sound from game->resourses->SoundData
	//@param
	//channel_id - id of channel that was used for this sound 
	void PlaySound(std::string name, int &channel_id)
	{
		for (size_t i = 0; i < Channels->size(); i++)
		{
			bool res;
			Channels->at(i)->isPlaying(&res);
			if (Channels->at(i) == NULL)
			{
				FMOD_RESULT r;
				r = lowSoundSystem->playSound(Resources->getSoundResourceDataByName(name)->sound, 0, false, &Channels->at(i));
				if (r != FMOD_OK)
				{
					throw(std::runtime_error(FMOD_ErrorString(r)));
				}
				channel_id = i;
				break;
			}
			else if (res == false)
			{
				FMOD_RESULT r;
				r = lowSoundSystem->playSound(Resources->getSoundResourceDataByName(name)->sound, 0, false, &Channels->at(i));
				if (r != FMOD_OK)
				{
					throw(std::runtime_error(FMOD_ErrorString(r)));
				}
				channel_id = i;
				break;
			}
		}
	}



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
		std::cout << "Sound resources..." << std::endl;
		if (!Resources->SoundData->empty())
		{
			for (size_t i = 0; i < Resources->SoundData->size(); i++)
			{
				FMOD_RESULT res;
				res = this->lowSoundSystem->createSound(Resources->SoundData->at(i)->filename.c_str(), FMOD_2D, 0, &Resources->SoundData->at(i)->sound);
				if (res != FMOD_OK)
				{
					std::cout << "Error creating sound. Name: " << Resources->SoundData->at(i)->name.c_str() << "Filename: " << Resources->SoundData->at(i)->filename.c_str() << "Error: " << FMOD_ErrorString(res) << std::endl;
				}
				else
				{

				}
			}
		}
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