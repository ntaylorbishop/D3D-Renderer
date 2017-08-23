#include "Game/RHI/Texture.hpp"

STATIC TextureDatabase* TextureDatabase::s_texDB = nullptr;


//---------------------------------------------------------------------------------------------------------------------------
STATIC Texture* TextureDatabase::CreateOrGetTexture(const String& filePath) {

	TextureMapIterator texIt = m_textures.find(filePath);

	if (texIt != m_textures.end()) {
		return texIt->second;
	}
	else {
		Texture* newTex = new Texture2D(filePath);
		m_textures.insert(TextureMapPair(filePath, newTex));
		return newTex;
	}
}


//---------------------------------------------------------------------------------------------------------------------------
TextureDatabase::TextureDatabase() {

}


//---------------------------------------------------------------------------------------------------------------------------
TextureDatabase::~TextureDatabase() {

	TextureMapIterator texIt = m_textures.begin();

	for (texIt; texIt != m_textures.end(); ++texIt) {

		Texture* texToDelete = texIt->second;
		delete texToDelete;
	}
}