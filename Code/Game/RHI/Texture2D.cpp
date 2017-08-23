#include "Game/RHI/Texture2D.hpp"

#define STBI_HEADER_FILE_ONLY

#pragma warning(push)
#include "ThirdParty/stb_image.c"
#pragma warning(pop)

//---------------------------------------------------------------------------------------------------------------------------
Texture2D::Texture2D(const char* imageFilePath) {

	int numComponents = 0;
	int numComponentsRequested = 0;

	unsigned char* imageData = stbi_load(imageFilePath.c_str(), &m_textureSize.x, &m_textureSize.y, &numComponents, numComponentsRequested);




	D3D11_TEXTURE2D_DESC desc;
	desc.Width = static_cast<UINT>(m_textureSize.x);
	desc.Height = static_cast<UINT>(m_textureSize.y);
	desc.MipLevels = static_cast<UINT>(mipCount);
	desc.ArraySize = static_cast<UINT>(1);
	desc.Format = format;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = usage;
	desc.BindFlags = bindFlags;
	desc.CPUAccessFlags = cpuAccessFlags;
	desc.MiscFlags = miscFlags & ~D3D11_RESOURCE_MISC_TEXTURECUBE;

	ID3D11Texture2D* tex = nullptr;
	HRESULT hr = RHIDeviceWindow::Get()->m_pImmediateContext->CreateTexture2D(&desc, imageData, &tex );

	if (SUCCEEDED(hr) && tex != 0)
	{
		if (textureView != 0)
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
			memset(&SRVDesc, 0, sizeof(SRVDesc));
			SRVDesc.Format = format;

			if (isCubeMap)
			{
				if (arraySize > 6)
				{
					SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBEARRAY;
					SRVDesc.TextureCubeArray.MipLevels = (!mipCount) ? -1 : desc.MipLevels;

					// Earlier we set arraySize to (NumCubes * 6)
					SRVDesc.TextureCubeArray.NumCubes = static_cast<UINT>(arraySize / 6);
				}
				else
				{
					SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
					SRVDesc.TextureCube.MipLevels = (!mipCount) ? -1 : desc.MipLevels;
				}
			}
			else if (arraySize > 1)
			{
				SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
				SRVDesc.Texture2DArray.MipLevels = (!mipCount) ? -1 : desc.MipLevels;
				SRVDesc.Texture2DArray.ArraySize = static_cast<UINT>(arraySize);
			}
			else
			{
				SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
				SRVDesc.Texture2D.MipLevels = (!mipCount) ? -1 : desc.MipLevels;
			}

			hr = d3dDevice->CreateShaderResourceView(tex,
				&SRVDesc,
				textureView
			);
			if (FAILED(hr))
			{
				tex->Release();
				return hr;
			}
		}

		if (texture != 0)
		{
			*texture = tex;
		}
		else
		{
			SetDebugObjectName(tex, "DDSTextureLoader");
			tex->Release();
		}
	}
}
break;
}


//---------------------------------------------------------------------------------------------------------------------------
Texture2D::~Texture2D() {

}