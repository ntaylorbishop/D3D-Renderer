#pragma once

#include "Engine/Math/AABB2.hpp"
#include "Game/DebugSceneTypes/DebugSceneType.hpp"

const uint FILMIC_TONEMAPPER = 2;

class SceneTypeHDR : public DebugSceneType {
public:
	SceneTypeHDR();
	~SceneTypeHDR();

	//UPDATE
	void HandleDrawToggle();
	void HandleExposureVolumeToggle();

	VIRTUAL void Update(float deltaSeconds);
	
	//RENDER
	void RenderText() const;
	VIRTUAL void Render() const;


private:
	uint	m_tonemapOperator		= 2;
	bool	m_debugDrawExpVols		= false;
	bool	m_exposureVolsEnabled	= false;
};