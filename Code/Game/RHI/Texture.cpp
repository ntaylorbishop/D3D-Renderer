#include "Game/RHI/Texture.hpp"

STATIC TextureDatabase* TextureDatabase::s_texDB = nullptr;


//---------------------------------------------------------------------------------------------------------------------------
STATIC Texture* TextureDatabase::CreateOrGetTexture(const String& filePath) {

	std::map<String, Texture*>::iterator texIt = s_texDB->m_textures.find(filePath);

	if (texIt != s_texDB->m_textures.end()) {
		return texIt->second;
	}
	else {
		return nullptr;
	}
}


//---------------------------------------------------------------------------------------------------------------------------
TextureDatabase::TextureDatabase() {

}


//---------------------------------------------------------------------------------------------------------------------------
TextureDatabase::~TextureDatabase() {

	std::map<String, Texture*>::iterator texIt = s_texDB->m_textures.begin();

	for (texIt; texIt != s_texDB->m_textures.end(); ++texIt) {

		Texture* texToDelete = texIt->second;
		delete texToDelete;
	}
}