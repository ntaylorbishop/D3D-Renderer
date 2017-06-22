#pragma once

#include "Engine/Math/Matrix4.hpp"
#include "Engine/Renderer/Lights/Light3D.hpp"
#include "Engine/Renderer/Lights/ShadowCastingLight3D.hpp"

class Model;
class Material;
class Mesh;

const char* const BILLBOARD_SPRITE_PATH = "Data/Textures/Brick.png";

class MovingLight {
public:
	//STRUCTORS
	MovingLight(const Vector3& startingPos, const Vector3& startingVel, const RGBA& color, float size, float intensity);
	~MovingLight();

	//UPDATE
	void Update(float deltaSeconds);

	//GETTERS SETTERS
	Vector3 GetPosition() const { return m_position; }
	Vector3 GetVelocity() const { return m_velocity; }
	void SetPosition(const Vector3& newPos) { m_position = newPos; }
	void SetVelocity(const Vector3& newVel) { m_velocity = newVel; }

	Light3D* GetLight() const { }

private:
	Vector3		m_position		= Vector3::ZERO;
	Vector3		m_velocity		= Vector3::ZERO;
	RGBA		m_color			= RGBA::WHITE;
	float		m_size			= 0.f;
	float		m_intensity		= 0.f;
	uint		m_lightIdx		= 0;
	Model*		m_model			= nullptr;
	Matrix4		m_modelMat		= Matrix4::IDENTITY;
};