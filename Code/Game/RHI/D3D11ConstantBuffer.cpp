#include "Game/RHI/D3D11ConstantBuffer.hpp"


//---------------------------------------------------------------------------------------------------------------------------
D3D11ConstantBuffer::D3D11ConstantBuffer(size_t byteSizeOfBuffer) 
	: m_bufferSize(byteSizeOfBuffer)
{
	m_pByteBuffer = new byte[byteSizeOfBuffer];
}


//---------------------------------------------------------------------------------------------------------------------------
D3D11ConstantBuffer::~D3D11ConstantBuffer() {

	delete m_pByteBuffer;
	m_pByteBuffer = nullptr;
}


//---------------------------------------------------------------------------------------------------------------------------
void D3D11ConstantBuffer::CreateBufferOnDevice() {

	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = m_bufferSize;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	HRESULT hr = RHIDeviceWindow::Get()->m_pd3dDevice->CreateBuffer(&bufferDesc, nullptr, &m_pDeviceBuffer);

	if (FAILED(hr))
		ERROR_AND_DIE("HR FAILED");
}


//---------------------------------------------------------------------------------------------------------------------------
void D3D11ConstantBuffer::ReleaseLocalBuffer() {

	delete m_pByteBuffer;
	m_pByteBuffer = nullptr;
}


//---------------------------------------------------------------------------------------------------------------------------
void D3D11ConstantBuffer::UpdateBufferOnDevice() {

	for (size_t i = 0; i < m_uniforms.size(); i++) {

	}


	RHIDeviceWindow::Get()->m_pDeviceContext->UpdateSubresource(m_pConstantBuffer, 0, nullptr, &cb, 0, 0);

}