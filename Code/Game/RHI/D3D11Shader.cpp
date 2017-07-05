#include "Game/RHI/D3D11Shader.hpp"
#include "Game/RHI/RHIShaderUtils.hpp"
#include "Game/RHI/RHIDeviceWindow.hpp"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//STRUCTORS
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------------
D3D11Shader::D3D11Shader(const char* shaderName, eD3D11ShaderType shaderType) 
	: m_shaderName(shaderName)
	, m_shaderType(shaderType)
{

	String shaderPath = "Shaders/" + String(shaderName) + ".hlsl";
	String entryPoint;

	if (shaderType == D3D11SHADERTYPE_VERTEX) {
		entryPoint = "VS";
	}
	else if (shaderType == D3D11SHADERTYPE_FRAGMENT) {
		entryPoint = "PS";
	}

	// Compile the vertex shader
	ID3DBlob* pVSBlob = nullptr;
	CompileShaderFromFile(shaderPath.c_str(), "VS", "vs_4_0", &pVSBlob);

	ID3D11Device* device = RHIDeviceWindow::Get()->GetD3DDevice();
	HRESULT hr = device->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &m_shaderHandle);

	if (FAILED(hr)) {
		pVSBlob->Release();
		ERROR_AND_DIE("ERROR: Could not create D3D11 shader.");
	}
}


//---------------------------------------------------------------------------------------------------------------------------
D3D11Shader::~D3D11Shader() {

}