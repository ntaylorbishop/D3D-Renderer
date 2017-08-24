#pragma once

#include "Game/RHI/Texture.hpp"
#include "Game/RHI/RHIDeviceWindow.hpp"
#include "Engine/General/Core/EngineCommon.hpp"
#include "Engine/Math/IntVector2.hpp"
#include "Game/RHI/D3D11Resource.hpp"

//---------------------------------------------------------------------------------------------------------------------------
enum eTextureBindFlags {
	TEXTURE_BIND_VERTEX_BUFFER		= 0x1L,
	TEXTURE_BIND_INDEX_BUFFER		= 0x2L,
	TEXTURE_BIND_CONSTANT_BUFFER	= 0x4L,
	TEXTURE_BIND_SHADER_RESOURCE	= 0x8L,
	TEXTURE_BIND_STREAM_OUTPUT		= 0x10L,
	TEXTURE_BIND_RENDER_TARGET		= 0x20L,
	TEXTURE_BIND_DEPTH_STENCIL		= 0x40L,
	TEXTURE_BIND_UNORDERED_ACCESS	= 0x80L,
	TEXTURE_BIND_DECODER			= 0x200L,
	TEXTURE_BIND_VIDEO_ENCODER		= 0x400L
};


//---------------------------------------------------------------------------------------------------------------------------
enum eTextureCPUAccessFlags {
	TEXTURE_CPU_ACCESS_WRITE = 0x10000L,
	TEXTURE_CPU_ACCESS_READ = 0x20000L
};


//---------------------------------------------------------------------------------------------------------------------------
class Texture2D : public Texture {
public:
	Texture2D(const char* imageFilePath, bool generateMips, eTextureBindFlags bindFlags, eTextureCPUAccessFlags accessFlags);
	~Texture2D();

	void UseAsDepthStencilView();
	void UseAsRenderTargetView();
	void UseAsShaderResourceView();
	void UseAsUnorderedAccessView();

	ID3D11DepthStencilView*		GetDSV() { return (ID3D11DepthStencilView*)m_resourceViews[RESOURCE_TYPE_DEPTH_STENCIL_VIEW];	}
	ID3D11RenderTargetView*		GetRTV() { return (ID3D11RenderTargetView*)m_resourceViews[RESOURCE_TYPE_RENDER_TARGET_VIEW];	}
	ID3D11ShaderResourceView*	GetSRV() { return (ID3D11ShaderResourceView*)m_resourceViews[RESOURCE_TYPE_SHADER_RESOURCE_VIEW];	}
	ID3D11UnorderedAccessView*	GetUAV() { return (ID3D11UnorderedAccessView*)m_resourceViews[RESOURCE_TYPE_UNORDERED_ACCESS_VIEW]; }

private:
	
	IntVector2					m_textureSize	= IntVector2::ZERO;
	ID3D11Texture2D*			m_textureHandle	= nullptr;
	std::vector<D3D11Resource*> m_resourceViews;
	uint						m_mipLevel		= 0; //Mip level 0 is the full chain
};