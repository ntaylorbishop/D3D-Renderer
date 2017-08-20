#pragma once

class D3D11ConstantBuffer {
public:
	D3D11ConstantBuffer(size_t byteSizeOfBuffer);
	~D3D11ConstantBuffer();

	template<typename T>
	void AddElement(T elem);

	template<typename T>
	void AddElement(const T elem);

	byte* GetBuffer() { return m_pBuffer; }

private:
	byte*	m_pBuffer			= nullptr;
	size_t	m_currSizeOfBuffer	= 0;
};



template<typename T>
void D3D11ConstantBuffer::AddElement(T elem) {

}

template<typename T>
void D3D11ConstantBuffer::AddElement(const T elem) {

}