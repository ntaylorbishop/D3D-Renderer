#pragma once

#include "Game/RHI/RHIDeviceWindow.hpp"
#include "Game/RHI/D3D11VertexShader.hpp"
#include "Game/RHI/D3D11PixelShader.hpp"
#include "Game/RHI/D3D11ConstantBuffer.hpp"
#include "Game/RHI/D3D11Resource.hpp"


//---------------------------------------------------------------------------------------------------------------------------
enum eWhichShaderBound {
	WHICH_SHADER_VERTEX = 0,
	WHICH_SHADER_FRAGMENT,
	WHICH_SHADER_BOTH,
	WHICH_SHADER_NUM_VALUES
};



//---------------------------------------------------------------------------------------------------------------------------
struct ConstBufferBindInfo {
	uint m_bindPoint = 0;
	D3D11ConstantBuffer* m_pConstBuffer = nullptr;
	eWhichShaderBound m_whichShaders = WHICH_SHADER_NUM_VALUES;
};



//---------------------------------------------------------------------------------------------------------------------------
struct ResourceBindInfo {
	uint m_bindPoint = 0;
	D3D11Resource* m_pResource = nullptr;
	eWhichShaderBound m_whichShaders = WHICH_SHADER_NUM_VALUES;
};



//---------------------------------------------------------------------------------------------------------------------------
class D3D11ShaderProgram {
public:
	D3D11ShaderProgram();
	~D3D11ShaderProgram();

	void SetVertexShader(D3D11VertexShader* pVertShader)	{ m_pVertexShader = pVertShader; }
	void SetPixelShader(D3D11PixelShader* pPixelShader)		{ m_pPixelShader = pPixelShader; }

	void AddConstantBuffer(uint bindPoint, D3D11ConstantBuffer* pConstBuffer, eWhichShaderBound whichShadersToBindTo);
	void AddResource(uint bindPoint, D3D11Resource* pResource, eWhichShaderBound whichShadersToBindTo);

	void Use();

private:
	void BindConstantBuffers();
	void BindResources();

	D3D11VertexShader*	m_pVertexShader	= nullptr;
	D3D11PixelShader*	m_pPixelShader	= nullptr;

	std::vector<ConstBufferBindInfo> m_constBuffers;
	std::vector<ResourceBindInfo> m_resources;

};


//---------------------------------------------------------------------------------------------------------------------------
inline void D3D11ShaderProgram::AddConstantBuffer(uint bindPoint, D3D11ConstantBuffer* pConstBuffer, eWhichShaderBound whichShadersToBindTo) {

	ConstBufferBindInfo boundBuffer;
	boundBuffer.m_bindPoint = bindPoint;
	boundBuffer.m_pConstBuffer = pConstBuffer;
	boundBuffer.m_whichShaders = whichShadersToBindTo;
	m_constBuffers.push_back(boundBuffer);
}


//---------------------------------------------------------------------------------------------------------------------------
inline void D3D11ShaderProgram::AddResource(uint bindPoint, D3D11Resource* pResource, eWhichShaderBound whichShadersToBindTo) {

	ResourceBindInfo boundResource;
	boundResource.m_bindPoint = bindPoint;
	boundResource.m_pResource = pResource;
	boundResource.m_whichShaders = whichShadersToBindTo;
	m_resources.push_back(boundResource);
}