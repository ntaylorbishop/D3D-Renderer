#pragma once

#include "Engine/Renderer/General/RGBA.hpp"
#include "Engine/Math/Vector3.hpp"

const size_t WATER_FIDELITY = 50;
const RGBA WATER_AMBIENT_LIGHT = RGBA(1.f, 1.f, 1.f, 0.3f);

class Model;
class Material;
class TextureCubemap;

class WaterPlane {
public:
	//STRUCTORS
	WaterPlane(const Vector3& centerPosition, const Vector3& scale);
	~WaterPlane();

	//UPDATE
	void Update(float deltaSeconds);

	//GETTERS SETTERS
	Model* GetModel() { return m_model; }


private:
	static void CreateSurfaceMesh();


	Vector3			m_position		= Vector3::ZERO;
	Model*			m_model			= nullptr;
	Material*		m_material		= nullptr;
	uint8_t			m_waterPlaneID	= 0;
	float			m_age			= 0.f;

	static uint8_t	s_numWaterPlanes;
	static size_t	s_surfaceMeshID;
};