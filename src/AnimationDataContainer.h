#pragma once
#include <vector>
#include "Animation.h"
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
				if (animationsData->at(i).name == name) { return animationsData->at(i); }
			}
		}
	}
};