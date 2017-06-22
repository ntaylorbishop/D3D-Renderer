#pragma once

#include "Engine/Math/Vector3.hpp"
#include "Engine/Renderer/General/RGBA.hpp"
#include "Engine/Renderer/Mesh/Model.hpp"

class Model;

const float SPHERE_SCALE = 0.9f;

class ShadowSphere {
public:
	ShadowSphere(const Vector3& pos);
	~ShadowSphere();

	void	SetPos(const Vector3& pos)	{ m_model->m_model.ChangePosition(pos);		}
	Vector3 GetPos()					{ return m_model->m_model.GetPosition();	}
	Vector3 GetStartPos()				{ return m_startPos;						}

private:
	Vector3 m_pos		= Vector3::ZERO;
	Model*	m_model		= nullptr;
	RGBA	m_color		= RGBA::WHITE;
	Vector3 m_startPos	= Vector3::ZERO;
};