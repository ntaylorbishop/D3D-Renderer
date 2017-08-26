#include "Game/RHI/Texture2D.hpp"

#define STBI_HEADER_FILE_ONLY

#pragma warning(push)
#include "ThirdParty/stb_image.c"
#pragma warning(pop)

//---------------------------------------------------------------------------------------------------------------------------
Texture2D::Texture2D(const char* imageFilePath, bool generateMips, eTextureBindFlags bindFlags, eTextureCPUAccessFlags accessFlags) {

	m_resourceViews.resize(RESOURCE_TYPE_NUM_VIEWS);

	int numComponents = 0;
	int numComponentsRequested = 0;

	unsigned char* imageData = stbi_load(imageFilePath, &m_textureSize.x, &m_textureSize.y, &numComponents, numComponentsRequested);

	UINT miscFlags;
	if (generateMips) {
		miscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
	}
	else {
		miscFlags = 0;
	}

	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
	desc.Width = static_cast<UINT>(m_textureSize.x);
	desc.Height = static_cast<UINT>(m_textureSize.y);
	desc.MipLevels = static_cast<UINT>(0);
	desc.ArraySize = static_cast<UINT>(1);
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT; //Default for now, may optimize later
	desc.BindFlags = bindFlags;
	desc.CPUAccessFlags = accessFlags;
	desc.MiscFlags = miscFlags;
	ID3D11Texture2D* tex = nullptr;


	D3D11_SUBRESOURCE_DATA data;
	D3D11_SUBRESOURCE_DATA* pTexData = nullptr;
	ZeroMemory(&data, sizeof(data));

	data.pSysMem = imageData;
	data.SysMemPitch = 4; //WARNING: Assuming RGBA8
	pTexData = &data;

	HRESULT hr = GetDevice()->CreateTexture2D(&desc, pTexData, &tex );

	if (generateMips) {
		UseAsShaderResourceView();
		GetDeviceContext()->GenerateMips(GetSRV());
	}
}


//---------------------------------------------------------------------------------------------------------------------------
Texture2D::~Texture2D() {

}


//---------------------------------------------------------------------------------------------------------------------------
void Texture2D::UseAsDepthStencilView() {

	m_resourceViews[RESOURCE_TYPE_DEPTH_STENCIL_VIEW] = new D3D11Resource(m_textureHandle, RESOURCE_TYPE_DEPTH_STENCIL_VIEW);
}


//---------------------------------------------------------------------------------------------------------------------------
void Texture2D::UseAsRenderTargetView() {

	m_resourceViews[RESOURCE_TYPE_RENDER_TARGET_VIEW] = new D3D11Resource(m_textureHandle, RESOURCE_TYPE_RENDER_TARGET_VIEW);
}


//---------------------------------------------------------------------------------------------------------------------------
void Texture2D::UseAsShaderResourceView() {

	m_resourceViews[RESOURCE_TYPE_SHADER_RESOURCE_VIEW] = new D3D11Resource(m_textureHandle, RESOURCE_TYPE_SHADER_RESOURCE_VIEW);
}


//---------------------------------------------------------------------------------------------------------------------------
void Texture2D::UseAsUnorderedAccessView() {

	m_resourceViews[RESOURCE_TYPE_UNORDERED_ACCESS_VIEW] = new D3D11Resource(m_textureHandle, RESOURCE_TYPE_UNORDERED_ACCESS_VIEW);
}