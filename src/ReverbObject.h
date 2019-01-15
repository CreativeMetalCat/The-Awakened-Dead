#pragma once
#include "fmod.hpp"
#include "Object.h"

//Mostly data for FMOD::Reverb3D
//reverb has to be created by game class itself
class ReverbObject
{
private:
	bool m_beenInit = false;
protected:
	//position that used for fmod properties
	//if you need get position use getBasicPosition
	sf::Vector3f _postion;
public:

	float minDist = 0.f;

	float maxDist = 10.f;

	//reverb itself
	FMOD::Reverb3D *reverb;

	//Properties of reverb
	FMOD_REVERB_PROPERTIES m_properties = FMOD_PRESET_GENERIC;

	ReverbObject(sf::Vector3f pos, float minDist = 0.f, float maxDist = 10.f, FMOD_REVERB_PROPERTIES prop = FMOD_PRESET_GENERIC) :
		_postion(pos),
		minDist(minDist),
		maxDist(maxDist),
		m_properties(prop)
	{

	}

	//leaves previous z value
	void set3DPosition(sf::Vector2f pos)
	{
		_postion.x = pos.x;
		_postion.y = pos.y;
	}

	void set3DPosition(sf::Vector3f pos) { _postion = pos; }

	//position that used for fmod properties
	//if you need get position use getBasicPosition
	sf::Vector3f get3DPosition()const { return _postion; }

	//initializes reverb
	void createReverb(FMOD::System*&sys)
	{
		sys->createReverb3D(&reverb);

		reverb->setProperties(&m_properties);

		FMOD_VECTOR pos;
		pos.x = _postion.x;
		pos.y = _postion.y;
		pos.z = _postion.z;
		reverb->set3DAttributes(&pos, minDist, maxDist);
	}

	sf::Vector2f getBasicPosition()const { return sf::Vector2f(_postion.x, _postion.y); }

	void setReverbProperties(FMOD_REVERB_PROPERTIES prop)
	{
		if (!m_beenInit) { throw(std::runtime_error("Reverb is not initialized")); return; }
		m_properties = prop;
		reverb->setProperties(&prop);
	}
};