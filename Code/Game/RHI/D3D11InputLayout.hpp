#pragma once

#include "Game/RHI/D3D11VertexShader.hpp"

class D3D11InputLayout {
public:

private:

	ID3D11InputLayout*	m_vertLayout = nullptr;
};

void BindVertexLayoutToDeviceWindow(eVertexType vertType, D3D11VertexShader* pVertShader);

// Define the input layout
D3D11_INPUT_ELEMENT_DESC layout[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};
UINT numElements = ARRAYSIZE(layout);

// Create the input layout
hr = RHIDeviceWindow::Get()->m_pd3dDevice->CreateInputLayout(layout, numElements,
	m_pVertexShader->GetCompiledBlob()->GetBufferPointer(),
	m_pVertexShader->GetCompiledBlob()->GetBufferSize(), &m_pVertexLayout);

m_pVertexShader->GetCompiledBlob()->Release();
if (FAILED(hr))
ERROR_AND_DIE("HR FAILED");

// Set the input layout
RHIDeviceWindow::Get()->m_pImmediateContext->IASetInputLayout(m_pVertexLayout);