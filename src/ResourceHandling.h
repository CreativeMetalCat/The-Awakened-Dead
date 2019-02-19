#pragma once

#include "SFML/Graphics.hpp"
#include <iostream>
#include <vector>
#include "MaterialTypes.h"

#include <fmod.hpp>
#include <fmod_errors.h>

#pragma region Resource Types

#define RESOURCE_NULL 0x0

#define RESOURCE_FONT 0x65

#define RESOURCE_TEXTURE 0x66

#define RESOURCE_SHADER 0x67

#define RESOURCE_SOUND 0x68

#pragma endregion

class Resource
{
protected:
	std::string fileExtension;
public:
	int Type = RESOURCE_NULL;
	std::string name;
	std::string filename;

	Resource(std::string name) { this->name = name; }

	Resource(std::string name, std::string filename, int Type = RESOURCE_NULL) { this->name = name; this->filename = filename; this->fileExtension = fileExtension; this->Type = Type; }
	//fileExt - file extension
	virtual void CreateResourceFromFile() {};
};


class FontResource :public Resource
{
public:
	sf::Font font;
	FontResource(std::string name, std::string filename, int Type = RESOURCE_FONT) :Resource(name, filename, Type)
	{

	}
	FontResource(std::string name, std::string filename, sf::Font& font, int Type = RESOURCE_FONT) :Resource(name, filename, Type), font(font)
	{

	}

	void CreateResourceFromFile()override
	{
		font.loadFromFile(filename);
	}

	~FontResource()
	{
		this->font.~Font();
	}
};
class TextureResource :public Resource
{
private:
	bool textureIsRepeated;
	bool textureIsSmooth;
public:
	sf::Texture texture;
	TextureResource(std::string name, std::string filename, bool textureIsRepeated, bool textureIsSmooth, int Type = RESOURCE_TEXTURE) :Resource(name, filename, Type)
	{
		this->textureIsRepeated = textureIsRepeated;
		this->textureIsSmooth = textureIsSmooth;
	}
	TextureResource(std::string name, std::string filename, sf::Texture& texture, bool textureIsRepeated, bool textureIsSmooth, int Type = RESOURCE_TEXTURE) :Resource(name, filename, Type), texture(texture)
	{
		this->textureIsRepeated = textureIsRepeated;
		this->textureIsSmooth = textureIsSmooth;
	}
	void CreateResourceFromFile()override
	{
		texture.loadFromFile(filename);
		texture.setRepeated(textureIsRepeated);
		texture.setSmooth(textureIsSmooth);
	}
	~TextureResource()
	{
		texture.~Texture();
		this->name.~basic_string();
	}
};
class ShaderResource :public Resource
{
public:
	sf::Shader shader;
	ShaderResource(std::string name, std::string filename, int Type = RESOURCE_SHADER) :Resource(name, filename, Type)
	{

	}
	ShaderResource(std::string name, std::string filename, sf::Shader&shader, int Type = RESOURCE_SHADER) :Resource(name, filename, Type)
	{

	}
	void CreateResourceFromFile()override
	{

	}
};
class SoundResource :public Resource
{
private:
	//this will not work because it requires fmod::SoundSystem 
	//sound is created by it
	void CreateResourceFromFile()override {}
public:

	FMOD_MODE soundMode = FMOD_2D;
	FMOD::Sound * sound;

	SoundResource(std::string name, std::string filename, FMOD_MODE soundMode=FMOD_3D_LINEARROLLOFF, int Type = RESOURCE_SOUND) :Resource(name, filename, Type),soundMode(soundMode)
	{

	}
};

class TileData
{
public:
	int MatSoundtype = MAT_SOUND_TYPE_CONCRETE;
	int ResourceId = 0;
	TileData(int ResourceId = 0, int MatSoundtype = MAT_SOUND_TYPE_CONCRETE)
	{
		this->MatSoundtype = MatSoundtype;
		this->ResourceId = ResourceId;
	}
};

class ResourceContainer
{
private:
	//gets firts resource with this name of that type
	Resource* getResourceDataByName(std::string name, int Type)
	{
		//name MUST be longer then 0
		if (name == "") { throw(std::runtime_error("Texture name is null (\"\")")); }

		if (ResourceData->empty())
		{
			throw(std::runtime_error("No resources avalable"));
		}
		else
		{
			for (size_t i = 0; i < ResourceData->size(); i++)
			{
				if (ResourceData->at(i)->name == name && ResourceData->at(i)->Type == Type) { return ResourceData->at(i); }
			}
			throw(std::runtime_error("Unable to find resorce"));
		}
	}
protected:
	//all data for vector specific operations
	std::vector<Resource*>*ResourceData = new std::vector<Resource*>();
public:
	std::vector<TextureResource*>*TextureData = new std::vector<TextureResource*>();
	std::vector<FontResource*>*FontData = new std::vector<FontResource*>();
	std::vector<SoundResource*>*SoundData = new std::vector<SoundResource*>();

	std::vector<TileData>*TileDataContainer = new std::vector<TileData>();

	void addAnimationData(Resource* textureResource) { ResourceData->push_back(textureResource); }

	TileData getTileDataById(int id)
	{
		if (!TileDataContainer->empty())
		{
			for (size_t i = 0;i < TileDataContainer->size(); i++)
			{
				if (TileDataContainer->at(i).ResourceId == id)
				{
					return TileDataContainer->at(i);

				}
			}
			throw(std::runtime_error("Unable to find resorce.ID: " + id));
			return NULL;
		}
		else
		{
			throw(std::runtime_error("Unable to find resorce.ID: " + id));
			return NULL;
		}
	}
	void addTileData(TileData tileData)
	{
		this->TileDataContainer->push_back(tileData);
	}

	//gets firts resource with this name
	TextureResource* getTextureResourceDataByName(std::string name)
	{
		//name MUST be longer then 0
		if (name == "") { throw(std::runtime_error("name is null (\"\")")); }

		if (TextureData->empty())
		{
			throw(std::runtime_error("No resources avalable"));
		}
		else
		{
			for (size_t i = 0; i < TextureData->size(); i++)
			{
				if (TextureData->at(i)->name == name) { return TextureData->at(i); }
			}
			throw(std::runtime_error("Unable to find resorce. Name: " + name));
		}
	}


	//gets firts resource with this name of that type
	FontResource* getFontResourceDataByName(std::string name)
	{
		//name MUST be longer then 0
		if (name == "") { throw(std::runtime_error("name is null (\"\")")); }

		if (FontData->empty())
		{
			throw(std::runtime_error("No resources avalable"));
		}
		else
		{
			for (size_t i = 0; i < FontData->size(); i++)
			{
				if (FontData->at(i)->name == name) { return FontData->at(i); }
			}
			throw(std::runtime_error("Unable to find resorce.Name: " + name));
		}
	}

	//gets firts resource with this name
	SoundResource* getSoundResourceDataByName(std::string name)
	{
		//name MUST be longer then 0
		if (name == "") { throw(std::runtime_error("name is null (\"\")")); }

		if (SoundData->empty())
		{
			throw(std::runtime_error("No resources avalable"));
		}
		else
		{
			for (size_t i = 0; i < SoundData->size(); i++)
			{
				if (SoundData->at(i)->name == name) { return SoundData->at(i); }
			}
			throw(std::runtime_error("Unable to find resorce. Name: " + name));
		}
	}



	void AddTextureResource(TextureResource*r)
	{
		TextureData->push_back(r);
	}
	void AddFontResource(FontResource*r)
	{
		FontData->push_back(r);
	}
	void AddSoundResource(SoundResource*r)
	{
		SoundData->push_back(r);
	}
	void InitResources()
	{
		if (!TextureData->empty())
		{
			for (size_t i = 0; i < TextureData->size(); i++)
			{
				TextureData->at(i)->CreateResourceFromFile();
				std::cout << "Texture resource created. name: " + TextureData->at(i)->name + "       filename: " + TextureData->at(i)->filename << std::endl;
			}
		}
		if (!FontData->empty())
		{
			for (size_t i = 0; i < FontData->size(); i++)
			{
				FontData->at(i)->CreateResourceFromFile();
				std::cout << "Font resource created. name: " + FontData->at(i)->name + "       filename: " + FontData->at(i)->filename << std::endl;
			}
		}

	}


};