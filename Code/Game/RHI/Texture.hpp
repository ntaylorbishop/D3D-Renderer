#pragma once

#include "Engine/General/Core/EngineCommon.hpp"

class Texture {
public:
	Texture();
	~Texture();

private:


	friend class TextureDatabase;
};


//---------------------------------------------------------------------------------------------------------------------------
class TextureDatabase {
public:
	static Texture* CreateOrGetTexture(const String& filePathOrName);


private:
	TextureDatabase();
	~TextureDatabase();

	std::map<String, Texture*> m_textures;

	static TextureDatabase* s_texDB;
};

typedef std::map<String, Texture*>::iterator		TextureMapIterator;
typedef std::map<String, Texture*>::const_iterator	TextureMapConstIterator;
typedef std::pair<String, Texture*>					TextureMapPair;