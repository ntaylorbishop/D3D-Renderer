#pragma once

#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Game/DebugSceneTypes/DebugSceneType.hpp"

class ParticleSystem;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//ENUMS CONSTS
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------------
enum eDebugOITType {
	DEBUG_OIT_LIGHT_BEHIND_SMOKE_OIT = 0,
	DEBUG_OIT_LIGHT_BEHIND_SMOKE_UNSORTED,
	DEBUG_OIT_LIGHT_BEHIND_SMOKE_SORTED
};


//---------------------------------------------------------------------------------------------------------------------------
enum eBufferTypeOIT {
	BUFFER_TYPE_RTOP = 0,
	BUFFER_TYPE_RBOTTOM,
	BUFFER_TYPE_RFULL,
	BUFFER_TYPE_AFULL,
	BUFFER_TYPE_NONE
};


constexpr Vector3	PARTICLE_SYSTEM_POS1 = Vector3(10.f, 10.f, 0.f);
constexpr Vector3	PARTICLE_SYSTEM_POS2 = Vector3(40.f, 10.f, 0.f);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//SCENETYPE OIT CLASS
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------------
class SceneTypeOIT : public DebugSceneType {
public:
	SceneTypeOIT();
	~SceneTypeOIT();

	void SetToSorted();
	void SetToUnsorted();
	void SetToOIT();

	void HandleLeftRight();
	void HandleUpDown();

	void UpdateBufferVisualizer(float deltaSeconds);

	VIRTUAL void Update(float deltaSeconds);
	VIRTUAL void Render() const;


private:
	eDebugOITType	m_debugOITType = DEBUG_OIT_LIGHT_BEHIND_SMOKE_OIT;
	ParticleSystem* m_smokeParticleSystem1 = nullptr;
	ParticleSystem* m_smokeParticleSystem2 = nullptr;

	AABB2			m_pipTop		= AABB2(Vector2(10.f, 455.f), Vector2(0.f, 455.f) + Vector2(50.f, 50.f) * Vector2(16.f, 9.f));
	AABB2			m_pipBottom		= AABB2(Vector2(10.f, 10.f), Vector2(50.f, 50.f) * Vector2(16.f, 9.f));
	AABB2			m_full			= AABB2(Vector2(0.f), Vector2(1920.f, 1080.f));
	AABB2			m_aCurr			= AABB2(Vector2::ZERO, Vector2::ZERO);
	AABB2			m_rCurr			= AABB2(Vector2::ZERO, Vector2::ZERO);
	float			m_age			= 0.f;
	float			m_animTime		= 0.2f;
	eBufferTypeOIT	m_bufferType	= BUFFER_TYPE_NONE;
};