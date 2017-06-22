#include "Game/DebugSceneTypes/SceneTypeOIT.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Fonts/Font.hpp"
#include "Engine/Renderer/Renderer/BeirusRenderer.hpp"
#include "Engine/Renderer/Effects/SSAO.hpp"
#include "Engine/Particles/ParticleSystemManager.hpp"
#include "Engine/Particles/ParticleSystem.hpp"
#include "Engine/Renderer/Effects/OIT/OIT.hpp"
#include "Game/LightManager.hpp"
#include "Game/TheGame.hpp"
#include "Engine/Scene/Scene.hpp"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//STRUCTORS
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------------
SceneTypeOIT::SceneTypeOIT() {

	m_debugOITType = DEBUG_OIT_LIGHT_BEHIND_SMOKE_OIT;
	LightManager::SetForOIT();
	m_smokeParticleSystem1 = ParticleSystem::Create("LitSmoke", PARTICLE_SYSTEM_POS1);
	m_smokeParticleSystem2 = ParticleSystem::Create("LitSmoke", PARTICLE_SYSTEM_POS2);
}


//---------------------------------------------------------------------------------------------------------------------------
SceneTypeOIT::~SceneTypeOIT() {

	if (m_smokeParticleSystem1 != nullptr) {
		ParticleSystem::Destroy(m_smokeParticleSystem1);
		m_smokeParticleSystem1 = nullptr;
	}
	if (m_smokeParticleSystem2 != nullptr) {
		ParticleSystem::Destroy(m_smokeParticleSystem2);
		m_smokeParticleSystem2 = nullptr;
	}
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//UPDATE
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------------
void SceneTypeOIT::SetToSorted() {

	m_debugOITType = DEBUG_OIT_LIGHT_BEHIND_SMOKE_SORTED;
	TheGame::GetScene()->EnableDebugSorting();

	if (m_smokeParticleSystem1 != nullptr) {
		ParticleSystem::Destroy(m_smokeParticleSystem1);
		m_smokeParticleSystem1 = nullptr;
	}
	if (m_smokeParticleSystem2 != nullptr) {
		ParticleSystem::Destroy(m_smokeParticleSystem2);
		m_smokeParticleSystem2 = nullptr;
	}

	//No OIT
	OIT::Get()->EnableDebug();
	m_smokeParticleSystem1 = ParticleSystem::Create("LitSmoke_NoOIT", PARTICLE_SYSTEM_POS1);
	m_smokeParticleSystem2 = ParticleSystem::Create("LitSmoke_NoOIT", PARTICLE_SYSTEM_POS2);
}


//---------------------------------------------------------------------------------------------------------------------------
void SceneTypeOIT::SetToUnsorted() {

	m_debugOITType = DEBUG_OIT_LIGHT_BEHIND_SMOKE_UNSORTED;
	TheGame::GetScene()->DisableDebugSorting();

	if (m_smokeParticleSystem1 != nullptr) {
		ParticleSystem::Destroy(m_smokeParticleSystem1);
		m_smokeParticleSystem1 = nullptr;
	}
	if (m_smokeParticleSystem2 != nullptr) {
		ParticleSystem::Destroy(m_smokeParticleSystem2);
		m_smokeParticleSystem2 = nullptr;
	}

	//No OIT
	OIT::Get()->EnableDebug();
	m_smokeParticleSystem1 = ParticleSystem::Create("LitSmoke_NoOIT", PARTICLE_SYSTEM_POS1);
	m_smokeParticleSystem2 = ParticleSystem::Create("LitSmoke_NoOIT", PARTICLE_SYSTEM_POS2);
}


//---------------------------------------------------------------------------------------------------------------------------
void SceneTypeOIT::SetToOIT() {

	TheGame::GetScene()->DisableDebugSorting();
	m_debugOITType = DEBUG_OIT_LIGHT_BEHIND_SMOKE_OIT;
	if (m_smokeParticleSystem1 != nullptr) {
		ParticleSystem::Destroy(m_smokeParticleSystem1);
		m_smokeParticleSystem1 = nullptr;
	}
	if (m_smokeParticleSystem2 != nullptr) {
		ParticleSystem::Destroy(m_smokeParticleSystem2);
		m_smokeParticleSystem2 = nullptr;
	}

	//No OIT
	OIT::Get()->DisableDebug();
	m_smokeParticleSystem1 = ParticleSystem::Create("LitSmoke", PARTICLE_SYSTEM_POS1);
	m_smokeParticleSystem2 = ParticleSystem::Create("LitSmoke", PARTICLE_SYSTEM_POS2);
}


//---------------------------------------------------------------------------------------------------------------------------
void SceneTypeOIT::HandleLeftRight() {
	//Change debug type
	if (InputSystem::GetKeyDown(VK_LEFT)) {
		switch (m_debugOITType) {
		case DEBUG_OIT_LIGHT_BEHIND_SMOKE_OIT: {
			SetToUnsorted();
			break;
		}
		case DEBUG_OIT_LIGHT_BEHIND_SMOKE_UNSORTED: {
			SetToSorted();
			break;
		}
		case DEBUG_OIT_LIGHT_BEHIND_SMOKE_SORTED: {
			SetToOIT();
			break;
		}
		}
	}
	else if (InputSystem::GetKeyDown(VK_RIGHT)) {
		switch (m_debugOITType) {
		case DEBUG_OIT_LIGHT_BEHIND_SMOKE_OIT: {
			SetToSorted();
			break;
		}
		case DEBUG_OIT_LIGHT_BEHIND_SMOKE_UNSORTED: {
			SetToOIT();
			break;
		}
		case DEBUG_OIT_LIGHT_BEHIND_SMOKE_SORTED: {
			SetToUnsorted();
			break;
		}
		}
	}
}


//---------------------------------------------------------------------------------------------------------------------------
void SceneTypeOIT::HandleUpDown() {

	if (InputSystem::GetKeyDown(VK_UP)) {
		switch (m_bufferType) {
		case BUFFER_TYPE_RTOP: {
			m_bufferType = BUFFER_TYPE_NONE;
			break;
		}
		case BUFFER_TYPE_RBOTTOM: {
			m_bufferType = BUFFER_TYPE_RTOP;
			break;
		}
		case BUFFER_TYPE_RFULL: {
			m_bufferType = BUFFER_TYPE_RBOTTOM;
			break;
		}
		case BUFFER_TYPE_AFULL: {
			m_bufferType = BUFFER_TYPE_RFULL;
			break;
		}
		case BUFFER_TYPE_NONE: {
			m_bufferType = BUFFER_TYPE_AFULL;
			break;
		}
		}
		m_age = 0.f;
	}

	if (InputSystem::GetKeyDown(VK_DOWN)) {
		switch (m_bufferType) {
		case BUFFER_TYPE_RTOP: {
			m_bufferType = BUFFER_TYPE_RBOTTOM;
			break;
		}
		case BUFFER_TYPE_RBOTTOM: {
			m_bufferType = BUFFER_TYPE_RFULL;
			break;
		}
		case BUFFER_TYPE_RFULL: {
			m_bufferType = BUFFER_TYPE_AFULL;
			break;
		}
		case BUFFER_TYPE_AFULL: {
			m_bufferType = BUFFER_TYPE_NONE;
			break;
		}
		case BUFFER_TYPE_NONE: {
			m_bufferType = BUFFER_TYPE_RTOP;
			break;
		}
		}
		m_age = 0.f;
	}
}


//---------------------------------------------------------------------------------------------------------------------------
void SceneTypeOIT::UpdateBufferVisualizer(float deltaSeconds) {

	switch (m_bufferType) {
	case BUFFER_TYPE_RTOP: {
		if (m_age < m_animTime) {
			m_age += deltaSeconds;
			m_aCurr.mins = Lerp(m_full.mins, m_pipBottom.mins, m_age / m_animTime);
			m_aCurr.maxs = Lerp(m_full.maxs, m_pipBottom.maxs, m_age / m_animTime);

			m_rCurr.mins = Lerp(m_full.mins, m_pipTop.mins, m_age / m_animTime);
			m_rCurr.maxs = Lerp(m_full.maxs, m_pipTop.maxs, m_age / m_animTime);
		}
		break;
	}
	case BUFFER_TYPE_RBOTTOM: {
		if (m_age < m_animTime) {
			m_age += deltaSeconds;
			m_aCurr.mins = Lerp(m_full.mins, m_pipTop.mins, m_age / m_animTime);
			m_aCurr.maxs = Lerp(m_full.maxs, m_pipTop.maxs, m_age / m_animTime);

			m_rCurr.mins = Lerp(m_full.mins, m_pipBottom.mins, m_age / m_animTime);
			m_rCurr.maxs = Lerp(m_full.maxs, m_pipBottom.maxs, m_age / m_animTime);
		}
		break;
	}
	case BUFFER_TYPE_RFULL: {
		if (m_age < m_animTime) {
			m_age += deltaSeconds;
			m_aCurr.mins = Lerp(m_full.mins, m_pipTop.mins, m_age / m_animTime);
			m_aCurr.maxs = Lerp(m_full.maxs, m_pipTop.maxs, m_age / m_animTime);

			m_rCurr.mins = Lerp(m_pipBottom.mins, m_full.mins, m_age / m_animTime);
			m_rCurr.maxs = Lerp(m_pipBottom.maxs, m_full.maxs, m_age / m_animTime);
		}
		break;
	}
	case BUFFER_TYPE_AFULL: {
		if (m_age < m_animTime) {
			m_age += deltaSeconds;
			m_aCurr.mins = Lerp(m_pipBottom.mins, m_full.mins, m_age / m_animTime);
			m_aCurr.maxs = Lerp(m_pipBottom.maxs, m_full.maxs, m_age / m_animTime);

			m_rCurr.mins = Lerp(m_full.mins, m_pipBottom.mins, m_age / m_animTime);
			m_rCurr.maxs = Lerp(m_full.maxs, m_pipBottom.maxs, m_age / m_animTime);
		}
		break;
	}
	case BUFFER_TYPE_NONE: {
		break;
	}
	}
}


//---------------------------------------------------------------------------------------------------------------------------
VIRTUAL void SceneTypeOIT::Update(float deltaSeconds) {

	HandleLeftRight();
	HandleUpDown();

	UpdateBufferVisualizer(deltaSeconds);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//RENDER
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------------
static void RenderDebugSortType(eDebugOITType type) {

	//Render debug text
	static float fontScale = 0.4f;
	static float fontTopLoc = 50.f;
	const Vector2* screenSize = BeirusRenderer::GetScreenSize();
	float lineHeight = Font::GetDefaultFontLineHeight(fontScale);
	String debugSceneTypeStr = "debug_scene_type: OIT";
	String debugOITMapTypeStr;

	switch (type) {
	case DEBUG_OIT_LIGHT_BEHIND_SMOKE_OIT: {
		debugOITMapTypeStr = "debug_oit_type: on";
		break;
	}
	case DEBUG_OIT_LIGHT_BEHIND_SMOKE_UNSORTED: {
		debugOITMapTypeStr = "debug_oit_type: off, unsorted";
		break;
	}
	case DEBUG_OIT_LIGHT_BEHIND_SMOKE_SORTED: {
		debugOITMapTypeStr = "debug_oit_type: off, sorted";
		break;
	}
	}

	Font::DrawText2DWithDefaultFont(Vector2(10.f, screenSize->y - fontTopLoc) - Vector2(0.f, 0.f * lineHeight), 
		debugSceneTypeStr, fontScale, RGBA::YELLOW);
	Font::DrawText2DWithDefaultFont(Vector2(10.f, screenSize->y - fontTopLoc) - Vector2(0.f, 1.f * lineHeight), 
		debugOITMapTypeStr, fontScale, RGBA::YELLOW);

	//CONTROLS
	Font::DrawText2DWithDefaultFont(Vector2(10.f, screenSize->y - fontTopLoc) - Vector2(0.f, 2.f * lineHeight),
		"Left, right: Change sorting type", fontScale, RGBA::YELLOW);
	Font::DrawText2DWithDefaultFont(Vector2(10.f, screenSize->y - fontTopLoc) - Vector2(0.f, 3.f * lineHeight),
		"Up, down: Switch buffer view", fontScale, RGBA::YELLOW);
}


//---------------------------------------------------------------------------------------------------------------------------
VIRTUAL void SceneTypeOIT::Render() const {

	switch (m_bufferType) {
	case BUFFER_TYPE_RTOP: {
		if (m_debugOITType != DEBUG_OIT_LIGHT_BEHIND_SMOKE_OIT) {
			break;
		}
		OIT::Get()->DebugDrawRevealageBuffer(m_rCurr);
		OIT::Get()->DebugDrawAccumBuffer(m_aCurr);
		//Font::DrawText2DWithDefaultFont(Vector2(100.f, 100.f), "DEBUG TYPE: REVEALAGE TOP", 0.4f, RGBA::WHITE);
		break;
	}
	case BUFFER_TYPE_RBOTTOM: {
		if (m_debugOITType != DEBUG_OIT_LIGHT_BEHIND_SMOKE_OIT) {
			break;
		}
		OIT::Get()->DebugDrawRevealageBuffer(m_rCurr);
		OIT::Get()->DebugDrawAccumBuffer(m_aCurr);
		//Font::DrawText2DWithDefaultFont(Vector2(100.f, 100.f), "DEBUG TYPE: REVEALAGE BOTTOM", 0.4f, RGBA::WHITE);
		break;
	}
	case BUFFER_TYPE_RFULL: {
		if (m_debugOITType != DEBUG_OIT_LIGHT_BEHIND_SMOKE_OIT) {
			break;
		}
		OIT::Get()->DebugDrawRevealageBuffer(m_rCurr);
		//Font::DrawText2DWithDefaultFont(Vector2(100.f, 100.f), "DEBUG TYPE: REVEALAGE FULL", 0.4f, RGBA::WHITE);
		break;
	}
	case BUFFER_TYPE_AFULL: {
		if (m_debugOITType != DEBUG_OIT_LIGHT_BEHIND_SMOKE_OIT) {
			break;
		}
		OIT::Get()->DebugDrawAccumBuffer(m_aCurr);
		//Font::DrawText2DWithDefaultFont(Vector2(100.f, 100.f), "DEBUG TYPE: ACCUM FULL", 0.4f, RGBA::WHITE);
		break;
	}
	case BUFFER_TYPE_NONE: {
		//Font::DrawText2DWithDefaultFont(Vector2(100.f, 100.f), "DEBUG TYPE: NO BUFFER", 0.4f, RGBA::WHITE);
		break;
	}
	}

	RenderDebugSortType(m_debugOITType);
}