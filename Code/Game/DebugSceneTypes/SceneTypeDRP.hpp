#pragma once

#include "Engine/Math/AABB2.hpp"
#include "Game/DebugSceneTypes/DebugSceneType.hpp"

class DynamicReflectionProbe;
class WaterPlane;

class SceneTypeDRP : public DebugSceneType {
public:
	SceneTypeDRP();
	~SceneTypeDRP();

	VIRTUAL void Update(float deltaSeconds);
	VIRTUAL void Render() const;


private:
	DynamicReflectionProbe* m_probe			= nullptr;

	float					m_age			= 0.f;
	float					m_animTime		= 2.f;
	float					m_height		= 0.f;
	
	static WaterPlane*		s_waterPlane;
};