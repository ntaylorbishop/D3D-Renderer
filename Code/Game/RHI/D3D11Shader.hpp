#pragma once

#include "Engine/General/Core/EngineCommon.hpp"
#include <d3d11_1.h>

enum eD3D11ShaderType {
	D3D11SHADERTYPE_VERTEX = 0,
	D3D11SHADERTYPE_FRAGMENT
};

class D3D11Shader {
public:
	//STRUCTORS
	D3D11Shader(const char* shaderName, eD3D11ShaderType shaderType);
	~D3D11Shader();

private:
	ID3D11VertexShader* m_shaderHandle = nullptr;
	String				m_shaderName;
	eD3D11ShaderType	m_shaderType;
};