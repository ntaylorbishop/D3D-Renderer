#include "Game/RHI/D3D11Mesh.hpp"


//---------------------------------------------------------------------------------------------------------------------------
D3D11Mesh::D3D11Mesh(eVertexType vertType, size_t numVerts)
	: m_vertType(vertType)
	, m_numVerts(numVerts)
	, m_vertSize(GetVertexByteSize(vertType))
{
	m_verts = (byte*)malloc(m_vertSize * m_numVerts);
}

//---------------------------------------------------------------------------------------------------------------------------
void D3D11Mesh::AddVertex(const Vector3& pos, const RGBA& col) {
	CreateVertex(m_verts + m_numVerts * GetVertexByteSize(m_vertType), m_numVerts * GetVertexByteSize(m_vertType), pos, col);
	m_numVerts++;
}


/*
//---------------------------------------------------------------------------------------------------------------------------
void AddVertex(const Vector3& pos, const RGBA& col, const Vector2& tex) {

}


//---------------------------------------------------------------------------------------------------------------------------
void AddVertex(const Vector3& pos, const RGBA& col, const Vector2& tex, const Vector3& normal) {

}


//---------------------------------------------------------------------------------------------------------------------------
void AddVertex(const Vector3& pos, const RGBA& col, const Vector2& tex, const Vector3& tangent, const Vector3& bitangent) {

}


//---------------------------------------------------------------------------------------------------------------------------
void AddVertex(const Vector3& pos, const RGBA& col, const Vector2& tex, const Vector3& tangent, const Vector3& bitangent, const Vector3& normal, const Vector4& boneWeights, const IntVector4& boneInds) {

}


//---------------------------------------------------------------------------------------------------------------------------
void AddVertex(const Vector2& pos, const RGBA& col, const Vector2& tex) {

}
*/