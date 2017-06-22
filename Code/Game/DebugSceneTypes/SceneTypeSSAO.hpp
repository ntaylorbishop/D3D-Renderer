#pragma once

#include "Engine/Math/AABB2.hpp"
#include "Game/DebugSceneTypes/DebugSceneType.hpp"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//ENUMS
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------------
enum eDebugSSAOType {
	DEBUG_SSAO_PIP = 0,
	DEBUG_SSAO_NONE,
	DEBUG_SSAO_FULL
};

enum eDebugSSAOMap {
	DEBUG_SSAO_MAP_NONBLUR = 0,
	DEBUG_SSAO_MAP_BLUR,
	DEBUG_SSAO_MAP_NORMALS
};


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//SCENETYPESSAO CLASS
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------------
class SceneTypeSSAO : public DebugSceneType {
public:
	SceneTypeSSAO();
	~SceneTypeSSAO();

	VIRTUAL void Update(float deltaSeconds);
	VIRTUAL void Render() const;


private:
	eDebugSSAOType	m_bufferType	= DEBUG_SSAO_PIP;
	eDebugSSAOMap	m_ssaoMap		= DEBUG_SSAO_MAP_BLUR;

	AABB2			m_SSAOpip		= AABB2(Vector2(10.f, 10.f), Vector2(50.f, 50.f) * Vector2(16.f, 9.f));
	AABB2			m_SSAOfull		= AABB2(Vector2(0.f), Vector2(1920.f, 1080.f));
	AABB2			m_SSAOcurr		= AABB2(Vector2::ZERO, Vector2::ZERO);
	float			m_ssaoAge		= 0.f;
	float			m_ssaoAnimTime	= 0.2f;
	bool			m_blurredSSAO	= false;
};