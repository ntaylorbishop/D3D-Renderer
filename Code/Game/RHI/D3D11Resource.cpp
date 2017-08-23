#include "Game/RHI/D3D11Resource.hpp"


//---------------------------------------------------------------------------------------------------------------------------
D3D11Resource(eResourceType resourceType) 
	: m_resourceType(resourceType)
{

	we are here

	switch (m_resourceType) {
	case RESOURCE_TYPE_DEPTH_STENCIL_VIEW: {
		D3D11_DEPTH_STENCIL_VIEW_DESC dsv_desc;
		MemZero(&dsv_desc);

		ASSERT(format == IMAGE_D24S8);
		dsv_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

		dsv_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

		dd->CreateDepthStencilView(dx_resource, &dsv_desc, &dx_dsv);
		break;
	}
	case RESOURCE_TYPE_RENDER_TARGET_VIEW: {
		dd->CreateRenderTargetView(dx_resource, nullptr, &dx_rtv);
		break;
	}
	case RESOURCE_TYPE_SHADER_RESOURCE_VIEW: {
		D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc;
		MemZero(&srv_desc);

		srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srv_desc.Texture2D.MostDetailedMip = 0;
		srv_desc.Texture2D.MipLevels = (UINT)-1;

		// NOTE:  Depth Stencil Buffers need to have a different view
		// then their normal 
		srv_desc.Format = DXFormatFromImageFormat(format);
		if (format == IMAGE_D24S8) {
			srv_desc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		}

		HRESULT result = dd->CreateShaderResourceView(dx_resource, &srv_desc, &dx_srv);
		ASSERT(SUCCEEDED(result));
		break;
	}
	case RESOURCE_TYPE_UNORDERED_ACCESS_VIEW: {
		D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
		MemZero(&desc);

		desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;;
		desc.Texture2D.MipSlice = 0;

		// NOTE:  Depth Stencil Buffers need to have a different view
		// then their normal 
		desc.Format = DXFormatFromImageFormat(format);
		if (format == IMAGE_D24S8) {
			desc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		}

		HRESULT result = dd->CreateUnorderedAccessView(dx_resource, &desc, &dx_uav);
		ASSERT(SUCCEEDED(result));
		break;
	}
	}

	}

}


//---------------------------------------------------------------------------------------------------------------------------
ID3D11DepthStencilView* D3D11Resource::AsDepthStencilView() {

	ASSERT_OR_DIE(m_resourceType == RESOURCE_TYPE_DEPTH_STENCIL_VIEW, "ERROR: Resource is not depth stencil view.");
	return (ID3D11DepthStencilView*)m_view;
}


//---------------------------------------------------------------------------------------------------------------------------
ID3D11RenderTargetView* D3D11Resource::AsRenderTargetView() {

	ASSERT_OR_DIE(m_resourceType == RESOURCE_TYPE_RENDER_TARGET_VIEW, "ERROR: Resource is not depth stencil view.");
	return (ID3D11RenderTargetView*)m_view;
}


//---------------------------------------------------------------------------------------------------------------------------
ID3D11ShaderResourceView* D3D11Resource::AsShaderResourceView() {

	ASSERT_OR_DIE(m_resourceType == RESOURCE_TYPE_SHADER_RESOURCE_VIEW, "ERROR: Resource is not depth stencil view.");
	return (ID3D11ShaderResourceView*)m_view;
}


//---------------------------------------------------------------------------------------------------------------------------
ID3D11UnorderedAccessView* D3D11Resource::AsUnorderedAccessView() {

	ASSERT_OR_DIE(m_resourceType == RESOURCE_TYPE_UNORDERED_ACCESS_VIEW, "ERROR: Resource is not depth stencil view.");
	return (ID3D11UnorderedAccessView*)m_view;
}