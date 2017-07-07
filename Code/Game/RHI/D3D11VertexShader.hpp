#pragma once

#include "Game/RHI/D3D11Shader.hpp"

class D3D11VertexShader : public D3D11Shader {
public:
	D3D11VertexShader(const char* shaderName, eD3D11ShaderType shaderType) 
		: D3D11Shader(shaderName, shaderType) 
		, m_shaderHandle(CreateVertexShader())
	{ }
	~D3D11VertexShader();


	ID3D11VertexShader* GetShaderHandle() { return m_shaderHandle; }


private:
	ID3D11VertexShader*		m_shaderHandle	= nullptr;
};