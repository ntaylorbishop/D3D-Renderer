#include "Game/DebugSceneTypes/SceneTypeForwardPlus.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Fonts/Font.hpp"
#include "Engine/Renderer/Renderer/BeirusRenderer.hpp"
#include "Engine/Renderer/Effects/SSAO.hpp"
#include "Engine/Renderer/General/Texture.hpp"
#include "Engine/Renderer/General/TextureBuffer.hpp"
#include "Engine/Renderer/Framebuffer.hpp"
#include "Engine/Renderer/Structures/BeirusMeshCollection.hpp"
#include "Engine/Renderer/Material/Material.hpp"
#include "Engine/Renderer/Mesh/MeshRenderer.hpp"
#include "Engine/Renderer/Fonts/Font.hpp"
#include "Game/LightManager.hpp"
#include "Engine/General/Core/EngineCommon.hpp"

STATIC Texture*			s_lightHeatmap			= nullptr;
STATIC Framebuffer*		s_heatmapFBO			= nullptr;
STATIC TextureBuffer*	s_lightHeatmapBuffer	= nullptr;
STATIC Material*		s_heatmapGenMat			= nullptr;
STATIC Material*		s_heatmapDrawMat		= nullptr;


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//STRUCTORS
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------------
SceneTypeForwardPlus::SceneTypeForwardPlus() {

	if (s_lightHeatmap == nullptr) {
		Vector2 screenSize		= *BeirusRenderer::GetScreenSize();
		s_lightHeatmapBuffer	= new TextureBuffer(screenSize.x, screenSize.y, TEXTURE_FORMAT_RGBA8);
		s_lightHeatmap			= s_lightHeatmapBuffer->AsTexture();
		s_heatmapGenMat			= new Material("LightHeatmap");
		s_heatmapDrawMat		= BeirusRenderer::GetOrGenerateMaterial(s_lightHeatmap);
		s_heatmapGenMat->m_renderState.m_backfaceCulling = false;

		std::vector<TextureBuffer*> heatmapTex;
		heatmapTex.push_back(s_lightHeatmapBuffer);

		s_heatmapFBO = new Framebuffer(heatmapTex, nullptr);
	}
}


//---------------------------------------------------------------------------------------------------------------------------
SceneTypeForwardPlus::~SceneTypeForwardPlus() {

	LightManager::ResetLights(0);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//UPDATE
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------------
void SceneTypeForwardPlus::HandleLeftRight() {

	if (InputSystem::GetKeyDown(VK_LEFT) || InputSystem::GetKeyDown(VK_RIGHT)) {
		m_isHeatmapEnabled = !m_isHeatmapEnabled;
	}
}


//---------------------------------------------------------------------------------------------------------------------------
void SceneTypeForwardPlus::HandleUpDown() {

	if (InputSystem::GetKeyDown(VK_UP)) {

		if (m_currNumLightsInScene < 4100) {
			m_currNumLightsInScene += 100;
		}
		LightManager::ResetLights(m_currNumLightsInScene);
	}
	else if (InputSystem::GetKeyDown(VK_DOWN)) {
		
		if (m_currNumLightsInScene >= 100) {
			m_currNumLightsInScene -= 100;
		}
		LightManager::ResetLights(m_currNumLightsInScene);
	}

}

//---------------------------------------------------------------------------------------------------------------------------
VIRTUAL void SceneTypeForwardPlus::Update(float deltaSeconds) {

	HandleLeftRight();
	HandleUpDown();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//RENDER
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------------
void SceneTypeForwardPlus::RenderToLightHeatmap() const {

	if (!m_isHeatmapEnabled) {
		return;
	}

	s_heatmapFBO->Bind();
	BeirusRenderer::ClearScreen();
	MeshID mID			= BeirusRenderer::GetGlobalFullscreenMesh();
	MeshRenderer* mr	= BeirusRenderer::GetGlobalMeshRenderer();

	BeirusRenderer::DisableDepthTesting();
	mr->RenderMeshWithMaterial(mID, s_heatmapGenMat);
}


//---------------------------------------------------------------------------------------------------------------------------
void SceneTypeForwardPlus::RenderLightHeatmap() const {

	if (!m_isHeatmapEnabled) {
		return;
	}

	BeirusRenderer::GetGlobalFBO()->Bind();

	MeshID mID			= BeirusRenderer::GetGlobalFullscreenMesh();
	MeshRenderer* mr	= BeirusRenderer::GetGlobalMeshRenderer();
	s_heatmapDrawMat->m_renderState.m_backfaceCulling = false;

	BeirusRenderer::DisableDepthTesting();
	AABB2 quadToDrawOn = AABB2(Vector2(0.f, 0.f), Vector2(1920.f, 1080.f));
	BeirusRenderer::DrawTexturedAABB2(s_lightHeatmap, AABB2(Vector2(0.f, 1.f), Vector2(1.f, 0.f)), RGBA::WHITE, quadToDrawOn);
}


//---------------------------------------------------------------------------------------------------------------------------
void SceneTypeForwardPlus::RenderText() const {

	//Render debug text
	static float fontScale = 0.4f;
	static float fontTopLoc = 50.f;
	const Vector2* screenSize = BeirusRenderer::GetScreenSize();
	float lineHeight = Font::GetDefaultFontLineHeight(fontScale);
	String debugSceneTypeStr = "debug_scene_type: Forward Plus";
	String heatmapStr = "showing: heatmap view";

	String numLightsStr = StringUtils::Stringf("Num Lights: %u", m_currNumLightsInScene);

	Font::DrawText2DWithDefaultFont(Vector2(10.f, screenSize->y - fontTopLoc) - Vector2(0.f, 0.f * lineHeight), 
		debugSceneTypeStr, fontScale, RGBA::YELLOW);
	Font::DrawText2DWithDefaultFont(Vector2(10.f, screenSize->y - fontTopLoc) - Vector2(0.f, 1.f * lineHeight), 
		numLightsStr, fontScale, RGBA::YELLOW);

	if (m_isHeatmapEnabled) {
		Font::DrawText2DWithDefaultFont(Vector2(10.f, screenSize->y - fontTopLoc) - Vector2(0.f, 2.f * lineHeight),
			heatmapStr, fontScale, RGBA::YELLOW);
	}

	//CONTROLS
	Font::DrawText2DWithDefaultFont(Vector2(10.f, screenSize->y - fontTopLoc) - Vector2(0.f, 3.f * lineHeight),
		"Left, right: Switch heatmap view", fontScale, RGBA::YELLOW);
	Font::DrawText2DWithDefaultFont(Vector2(10.f, screenSize->y - fontTopLoc) - Vector2(0.f, 4.f * lineHeight),
		"Up, down: Increase, decrease num lights", fontScale, RGBA::YELLOW);
}


//---------------------------------------------------------------------------------------------------------------------------
VIRTUAL void SceneTypeForwardPlus::Render() const {

	RenderToLightHeatmap();
	RenderLightHeatmap();

	RenderText();
}