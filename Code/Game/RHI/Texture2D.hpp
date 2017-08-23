#pragma once

#include "Game/RHI/Texture.hpp"
#include "Game/RHI/RHIDeviceWindow.hpp"
#include "Engine/General/Core/EngineCommon.hpp"

class Texture2D : public Texture {
public:
	Texture2D(const char* imageFilePath);
	~Texture2D();


private:
	
	IntVector2 m_textureSize = IntVector2::ZERO;
	ID3D11Texture2D* m_textureHandle = nullptr;

};