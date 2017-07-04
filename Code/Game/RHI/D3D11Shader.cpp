#include "Game/RHI/D3D11Shader.hpp"
#include "Game/RHI/RHIShaderUtils.hpp"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//STRUCTORS
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------------
D3D11Shader::D3D11Shader(const char* shaderName, eD3D11ShaderType shaderType) {

	// Compile the vertex shader
	ID3DBlob* pVSBlob = nullptr;
	CompileShaderFromFile("Tutorial02.fx", "VS", "vs_4_0", &pVSBlob);

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