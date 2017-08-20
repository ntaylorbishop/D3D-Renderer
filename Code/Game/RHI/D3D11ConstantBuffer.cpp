#include "Game/RHI/D3D11ConstantBuffer.hpp"


//---------------------------------------------------------------------------------------------------------------------------
D3D11ConstantBuffer::D3D11ConstantBuffer(size_t byteSizeOfBuffer) {

	m_pBuffer = new byte[byteSizeOfBuffer];
}


//---------------------------------------------------------------------------------------------------------------------------
D3D11ConstantBuffer::~D3D11ConstantBuffer() {

	delete m_pBuffer;
	m_pBuffer = nullptr;
}