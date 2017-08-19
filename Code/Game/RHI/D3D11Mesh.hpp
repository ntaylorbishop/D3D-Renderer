#pragma once

#include "Game/RHI/Vertex.hpp"

class D3D11Mesh {
public:
	D3D11Mesh(eVertexType vertType, size_t numVerts);

	void AddVertex(const Vector3& pos, const RGBA& col);

	/*
	void AddVertex(const Vector3& pos, const RGBA& col, const Vector2& tex);
	void AddVertex(const Vector3& pos, const RGBA& col, const Vector2& tex, const Vector3& normal);
	void AddVertex(const Vector3& pos, const RGBA& col, const Vector2& tex, const Vector3& tangent, const Vector3& bitangent);
	void AddVertex(const Vector3& pos, const RGBA& col, const Vector2& tex, const Vector3& tangent, const Vector3& bitangent, const Vector3& normal, const Vector4& boneWeights, const IntVector4& boneInds);
	void AddVertex(const Vector2& pos, const RGBA& col, const Vector2& tex);
	*/

private:
	byte*		m_verts		= nullptr;
	eVertexType m_vertType	= VERTEX_TYPE_NUM_TYPES;
	size_t		m_numVerts	= 0;
	uint16_t	m_vertSize	= 0;
};