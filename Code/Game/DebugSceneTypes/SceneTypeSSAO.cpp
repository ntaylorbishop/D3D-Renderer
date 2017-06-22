#include "Game/DebugSceneTypes/SceneTypeSSAO.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Fonts/Font.hpp"
#include "Engine/Renderer/Renderer/BeirusRenderer.hpp"
#include "Engine/Renderer/Effects/SSAO.hpp"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//STRUCTORS
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------------
SceneTypeSSAO::SceneTypeSSAO() {

}


//---------------------------------------------------------------------------------------------------------------------------
SceneTypeSSAO::~SceneTypeSSAO() {
	//Toggle SSAO enable
	if (InputSystem::GetKeyDown('T')) {
		SSAO::DisableDebugEnable();
	}
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//UPDATE
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------------
VIRTUAL void SceneTypeSSAO::Update(float deltaSeconds) {

	//Input to switch ssao scene types
	if (InputSystem::GetKeyDown(VK_UP)) {
		switch (m_bufferType) {
		case DEBUG_SSAO_NONE: {
			m_bufferType = DEBUG_SSAO_PIP;
			break;
		}
		case DEBUG_SSAO_PIP: {
			m_bufferType = DEBUG_SSAO_FULL;
			break;
		}
		case DEBUG_SSAO_FULL: {
			m_bufferType = DEBUG_SSAO_NONE;
			break;
		}
		}
		m_ssaoAge = 0.f;
	}
	else if (InputSystem::GetKeyDown(VK_DOWN)) {
		switch (m_bufferType) {
		case DEBUG_SSAO_NONE: {
			m_bufferType = DEBUG_SSAO_FULL;
			break;
		}
		case DEBUG_SSAO_PIP: {
			m_bufferType = DEBUG_SSAO_NONE;
			break;
		}
		case DEBUG_SSAO_FULL: {
			m_bufferType = DEBUG_SSAO_PIP;
			break;
		}
		}
		m_ssaoAge = 0.f;
	}

	if (InputSystem::GetKeyDown(VK_LEFT)) {
		switch (m_ssaoMap) {
		case DEBUG_SSAO_MAP_NONBLUR: {
			m_ssaoMap = DEBUG_SSAO_MAP_BLUR;
			break;
		}
		case DEBUG_SSAO_MAP_BLUR: {
			m_ssaoMap = DEBUG_SSAO_MAP_NORMALS;
			break;
		}
		case DEBUG_SSAO_MAP_NORMALS: {
			m_ssaoMap = DEBUG_SSAO_MAP_NONBLUR;
			break;
		}
		}
	}
	else if (InputSystem::GetKeyDown(VK_RIGHT)) {
		switch (m_ssaoMap) {
		case DEBUG_SSAO_MAP_NONBLUR: {
			m_ssaoMap = DEBUG_SSAO_MAP_NORMALS;
			break;
		}
		case DEBUG_SSAO_MAP_BLUR: {
			m_ssaoMap = DEBUG_SSAO_MAP_NONBLUR;
			break;
		}
		case DEBUG_SSAO_MAP_NORMALS: {
			m_ssaoMap = DEBUG_SSAO_MAP_BLUR;
			break;
		}
		}
	}

	//Update anims for buffer drawing
	switch (m_bufferType) {
	case DEBUG_SSAO_NONE: {
		m_SSAOcurr = m_SSAOfull;
		break;
	}
	case DEBUG_SSAO_PIP: {
		if (m_ssaoAge < m_ssaoAnimTime) {
			m_ssaoAge += deltaSeconds;
			m_SSAOcurr.mins = Lerp(m_SSAOfull.mins, m_SSAOpip.mins, m_ssaoAge / m_ssaoAnimTime);
			m_SSAOcurr.maxs = Lerp(m_SSAOfull.maxs, m_SSAOpip.maxs, m_ssaoAge / m_ssaoAnimTime);
		}
		break;
	}
	case DEBUG_SSAO_FULL: {
		if (m_ssaoAge < m_ssaoAnimTime) {
			m_ssaoAge += deltaSeconds;
			m_SSAOcurr.mins = Lerp(m_SSAOpip.mins, m_SSAOfull.mins, m_ssaoAge / m_ssaoAnimTime);
			m_SSAOcurr.maxs = Lerp(m_SSAOpip.maxs, m_SSAOfull.maxs, m_ssaoAge / m_ssaoAnimTime);
			break;
		}
	}
	}

	//Toggle SSAO enable
	if (InputSystem::GetKeyDown('T')) {
		SSAO::ToggleDebugEnable();
	}
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//RENDER
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------------
VIRTUAL void SceneTypeSSAO::Render() const {

	//Render debug text
	static float fontScale = 0.4f;
	static float fontTopLoc = 50.f;
	const Vector2* screenSize = BeirusRenderer::GetScreenSize();
	float lineHeight = Font::GetDefaultFontLineHeight(fontScale);
	String debugSceneTypeStr = "debug_scene_type: SSAO";
	String debugSSAOMapTypeStr;

	switch (m_ssaoMap) {
	case DEBUG_SSAO_MAP_BLUR: {
		debugSSAOMapTypeStr = "Left, Right: blurred";
		break;
	}
	case DEBUG_SSAO_MAP_NONBLUR: {
		debugSSAOMapTypeStr = "Left, Right: non-blurred";
		break;
	}
	case DEBUG_SSAO_MAP_NORMALS: {
		debugSSAOMapTypeStr = "Left, Right: view-space normals";
		break;
	}
	}

	if (m_bufferType != DEBUG_SSAO_NONE) {
		switch (m_ssaoMap) {
		case DEBUG_SSAO_MAP_BLUR: {
			SSAO::DebugDrawSSAOMapBlur(m_SSAOcurr);
			break;
		}
		case DEBUG_SSAO_MAP_NONBLUR: {
			SSAO::DebugDrawSSAOMapNonBlur(m_SSAOcurr);
			break;
		}
		case DEBUG_SSAO_MAP_NORMALS: {
			SSAO::DebugDrawNormalsMap(m_SSAOcurr);
			break;
		}
		}
	}

	String debugDrawStr = (SSAO::GetDebugEnable()) ? ("T: Disable SSAO") : ("T: Enable SSAO");

	Font::DrawText2DWithDefaultFont(Vector2(10.f, screenSize->y - fontTopLoc) - Vector2(0.f, 0.f * lineHeight), 
		debugSceneTypeStr, fontScale, RGBA::YELLOW);
	Font::DrawText2DWithDefaultFont(Vector2(10.f, screenSize->y - fontTopLoc) - Vector2(0.f, 1.f * lineHeight),
		debugSSAOMapTypeStr, fontScale, RGBA::YELLOW);

	//CONTROLS
	Font::DrawText2DWithDefaultFont(Vector2(10.f, screenSize->y - fontTopLoc) - Vector2(0.f, 2.f * lineHeight),
		"Up, Down: Cycle buffer visualization", fontScale, RGBA::YELLOW);
	Font::DrawText2DWithDefaultFont(Vector2(10.f, screenSize->y - fontTopLoc) - Vector2(0.f, 3.f * lineHeight),
		debugDrawStr, fontScale, RGBA::YELLOW);
}