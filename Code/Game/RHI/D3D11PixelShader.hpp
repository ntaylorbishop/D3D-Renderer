#pragma once

#include "Game/RHI/D3D11Shader.hpp"

class D3D11PixelShader : public D3D11Shader {
public:
	D3D11PixelShader(const char* shaderName, eD3D11ShaderType shaderType) 
		: D3D11Shader(shaderName, shaderType) 
		, m_shaderHandle(CreatePixelShader())
	{ }
	~D3D11PixelShader();


	ID3D11PixelShader* GetShaderHandle() { return m_shaderHandle; }


private:
	ID3D11PixelShader*		m_shaderHandle = nullptr;
};