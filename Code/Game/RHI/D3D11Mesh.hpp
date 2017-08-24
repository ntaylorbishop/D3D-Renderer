#pragma once

#include "Game/RHI/Vertex.hpp"

class D3D11Mesh {
public:
	D3D11Mesh(eVertexType vertType, size_t numVerts);

	void AddVertex(const Vector3& pos, const RGBA& col);
	void AddVertex(const Vector3& pos, const RGBA& col, const Vector2& tex);
	void AddVertex(const Vector3& pos, const RGBA& col, const Vector2& tex, const Vector3& normal);
	void AddVertex(const Vector3& pos, const RGBA& col, const Vector2& tex, const Vector3& tangent, const Vector3& bitangent);
	void AddVertex(const Vector3& pos, const RGBA& col, const Vector2& tex, const Vector3& tangent, const Vector3& bitangent, const Vector3& normal, const Vector4& boneWeights, const IntVector4& boneInds);
	void CreateVertexBufferOnDevice();
	void CreateIndexBufferOnDevice();
	void BindVertBufferToDeviceWindow();
	void BindIndBufferToDeviceWindow();
	void SetIndexBuffer(uint32_t* indBuffer, size_t bufferSize, size_t numInds);

	D3D11_BUFFER_DESC		m_bufferDesc;
	D3D11_SUBRESOURCE_DATA	m_initData;

private:

	byte*			m_pVertData		= nullptr;
	uint32_t*		m_pIndData		= nullptr;
	eVertexType		m_vertType		= VERTEX_TYPE_NUM_TYPES;
	size_t			m_numVerts		= 0;
	uint16_t		m_vertByteSize	= 0;
	size_t			m_bufferSize	= 0;
	ID3D11Buffer*   m_pVertBuffer	= nullptr;
	ID3D11Buffer*   m_pIndBuffer	= nullptr;
	size_t			m_numInds		= 0;

};