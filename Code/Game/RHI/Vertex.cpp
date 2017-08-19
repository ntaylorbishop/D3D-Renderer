#include "Game/RHI/Vertex.hpp"

//---------------------------------------------------------------------------------------------------------------------------
size_t CreateVertex(byte* buffer, size_t bufferSize, const Vector3& pos, const RGBA& col) {
	memcpy_s(buffer, bufferSize, (void*)&pos, sizeof(Vector3));
	buffer += sizeof(Vector3);
	memcpy_s(buffer, bufferSize, (void*)&col, sizeof(RGBA));
	return sizeof(Vector3) + sizeof(RGBA);
}

/*
//---------------------------------------------------------------------------------------------------------------------------
size_t CreateVertex(byte* buffer, size_t bufferSize, const Vector3& pos, const RGBA& col, const Vector2& tex) {
	return 0;
}


//---------------------------------------------------------------------------------------------------------------------------
size_t CreateVertex(byte* buffer, size_t bufferSize, const Vector3& pos, const RGBA& col, const Vector2& tex, const Vector3& normal) {
	return 0;
}


//---------------------------------------------------------------------------------------------------------------------------
size_t CreateVertex(byte* buffer, size_t bufferSize, const Vector3& pos, const RGBA& col, const Vector2& tex, const Vector3& tangent, const Vector3& bitangent) {
	return 0;
}


//---------------------------------------------------------------------------------------------------------------------------
size_t CreateVertex(byte* buffer, size_t bufferSize, const Vector3& pos, const RGBA& col, const Vector2& tex, const Vector3& tangent, const Vector3& bitangent, const Vector3& normal, const Vector4& boneWeights, const IntVector4& boneInds) {
	return 0;
}


//---------------------------------------------------------------------------------------------------------------------------
size_t CreateVertex(byte* buffer, size_t bufferSize, const Vector2& pos, const RGBA& col, const Vector2& tex) {
	return 0;
}
*/


//---------------------------------------------------------------------------------------------------------------------------
uint16_t GetVertexByteSize(eVertexType vertType) {
	switch (vertType) {
	case VERTEX_TYPE_PC: {
		return sizeof(Vector3) + sizeof(RGBA);
	}
	}
}