#include "Game/DebugSceneTypes/SceneTypeHDR.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Fonts/Font.hpp"
#include "Engine/Renderer/Renderer/BeirusRenderer.hpp"
#include "Engine/Renderer/Effects/SSAO.hpp"
#include "Engine/Renderer/Shaders/ShaderProgram.hpp"
#include "Engine/Renderer/Effects/Tonemapping.hpp"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//STRUCTORS
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------------
SceneTypeHDR::SceneTypeHDR() {

	ShaderProgram* tonemappingProg	= ShaderProgram::CreateOrGetShaderProgram("Tonemapping");
	Uniform* debugUni				= tonemappingProg->GetUniform("gDebugDraw");

	debugUni->SetData(&m_tonemapOperator);
}


//---------------------------------------------------------------------------------------------------------------------------
SceneTypeHDR::~SceneTypeHDR() {

	ShaderProgram* tonemappingProg = ShaderProgram::CreateOrGetShaderProgram("Tonemapping");
	Uniform* debugUni = tonemappingProg->GetUniform("gDebugDraw");

	debugUni->SetData((void*)&FILMIC_TONEMAPPER);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//UPDATE
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------------
void SceneTypeHDR::HandleDrawToggle() {

	if (InputSystem::GetKeyDown('T')) {

		m_tonemapOperator++;
		
		if (m_tonemapOperator == 3) {
			m_tonemapOperator = 0;
		}
	}
}

//---------------------------------------------------------------------------------------------------------------------------
void SceneTypeHDR::HandleExposureVolumeToggle() {

	if (InputSystem::GetKeyDown('Y')) {
		m_debugDrawExpVols = !m_debugDrawExpVols;
	}

	if (InputSystem::GetKeyDown('U')) {
		m_exposureVolsEnabled = !m_exposureVolsEnabled;
		Tonemapping::Get()->ToggleExposureVolumes(m_exposureVolsEnabled);
	}




	if(m_debugDrawExpVols) {
		Tonemapping::Get()->EnableDebugVisualizer();
	}
	else {
		Tonemapping::Get()->DisableDebugVisualizer();
	}
}

//---------------------------------------------------------------------------------------------------------------------------
VIRTUAL void SceneTypeHDR::Update(float deltaSeconds) {

	HandleDrawToggle();
	HandleExposureVolumeToggle();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//RENDER
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------------
void SceneTypeHDR::RenderText() const {
	
	//Render debug text
	static float fontScale = 0.4f;
	static float fontTopLoc = 50.f;
	const Vector2* screenSize = BeirusRenderer::GetScreenSize();
	float lineHeight = Font::GetDefaultFontLineHeight(fontScale);

	String showingStr;

	if (m_tonemapOperator == 0) {
		showingStr = "View: Reinhardt";
	}
	else if (m_tonemapOperator == 1) {
		showingStr = "View: Linear";
	}
	else if (m_tonemapOperator == 2) {
		showingStr = "View: Filmic";
	}


	String expVolDispStr = (m_debugDrawExpVols) 
		? ("Y: Toggle exposure volume display OFF") 
		: ("Y: Toggle exposure volume display ON");
	String expVolStr = (m_exposureVolsEnabled) 
		? ("U: Toggle exposure volumes OFF")
		: ("U: Toggle exposure volumes ON");

	Font::DrawText2DWithDefaultFont(Vector2(10.f, screenSize->y - fontTopLoc) - Vector2(0.f, 0.f * lineHeight),
		showingStr, fontScale, RGBA::YELLOW);

	//CONTROLS
	Font::DrawText2DWithDefaultFont(Vector2(10.f, screenSize->y - fontTopLoc) - Vector2(0.f, 1.f * lineHeight),
		"T: Toggle HDR view", fontScale, RGBA::YELLOW);
	Font::DrawText2DWithDefaultFont(Vector2(10.f, screenSize->y - fontTopLoc) - Vector2(0.f, 2.f * lineHeight),
		expVolDispStr, fontScale, RGBA::YELLOW);
	Font::DrawText2DWithDefaultFont(Vector2(10.f, screenSize->y - fontTopLoc) - Vector2(0.f, 3.f * lineHeight),
		expVolStr, fontScale, RGBA::YELLOW);
	Font::DrawText2DWithDefaultFont(Vector2(10.f, screenSize->y - fontTopLoc) - Vector2(0.f, 4.f * lineHeight),
		"CONSOLE set_ambient_light", fontScale, RGBA::YELLOW);
	Font::DrawText2DWithDefaultFont(Vector2(10.f, screenSize->y - fontTopLoc) - Vector2(0.f, 5.f * lineHeight),
		"CONSOLE exp <num>", fontScale, RGBA::YELLOW);
}


//---------------------------------------------------------------------------------------------------------------------------
VIRTUAL void SceneTypeHDR::Render() const {

	RenderText();
}