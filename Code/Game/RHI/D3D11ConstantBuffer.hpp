#pragma once

#include "Engine/General/Core/EngineCommon.hpp"
#include "Game/RHI/RHIDeviceWindow.hpp"
#include "Engine/EventSystem/NamedProperties.hpp"
#include "Game/RHI/Uniform.hpp"

class D3D11ConstantBuffer {
public:
	D3D11ConstantBuffer(size_t byteSizeOfBuffer);
	~D3D11ConstantBuffer();

	void CreateBufferOnDevice();
	void ReleaseLocalBuffer();
	void UpdateBufferOnDevice();

	void AddUniform(Uniform* uniToAdd) { m_uniforms.push_back(uniToAdd); }

	//template<typename T>
	//void AddElement(const T elem);

	byte* GetBuffer() { return m_pByteBuffer; }

	NamedProperties	m_bufferData;
private:
	std::vector<Uniform*>	m_uniforms;
	byte*					m_pByteBuffer		= nullptr;
	size_t					m_currSizeOfBuffer	= 0;
	size_t					m_bufferSize		= 0;
	ID3D11Buffer*			m_pDeviceBuffer		= nullptr;
};



template<typename T>
void D3D11ConstantBuffer::AddElement(T elem) {

	memcpy_s(m_pByteBuffer + m_bufferSize, m_bufferSize, (void*)&elem, sizeof(T));
	m_bufferSize += sizeof(T);
}

/*
template<typename T>
void D3D11ConstantBuffer::AddElement(const T elem) {

	memcpy_s(m_pByteBuffer + m_bufferSize, m_bufferSize, (void*)&elem, sizeof(T));
	m_bufferSize += sizeof(T);
}*/