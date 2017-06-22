#include "Game/DebugSceneTypes/SceneTypeDRP.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Fonts/Font.hpp"
#include "Engine/Renderer/Renderer/BeirusRenderer.hpp"
#include "Engine/Renderer/Effects/SSAO.hpp"
#include "Game/WaterPlane.hpp"
#include "Game/TheGame.hpp"
#include "Engine/Scene/Scene.hpp"
#include "Engine/Renderer/ReflectionProbes/DynamicReflectionProbe.hpp"
#include "Engine/Renderer/Mesh/Model.hpp"


STATIC WaterPlane* SceneTypeDRP::s_waterPlane = nullptr;


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//STRUCTORS
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------------
SceneTypeDRP::SceneTypeDRP() {


	if (s_waterPlane) {
		s_waterPlane->GetModel()->m_model.ChangePosition(Vector3(-90.f, -4.f, -90.f));
	}
	else {
		s_waterPlane = new WaterPlane(Vector3(-90.f, -4.f, -90.f), Vector3(6.f));
		TheGame::GetScene()->AddStaticMesh(s_waterPlane->GetModel());
	}

}


//---------------------------------------------------------------------------------------------------------------------------
SceneTypeDRP::~SceneTypeDRP() {

	s_waterPlane->GetModel()->m_model.ChangePosition(Vector3(-90.f, -1.f, -90.f));
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//UPDATE
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------------
VIRTUAL void SceneTypeDRP::Update(float deltaSeconds) {

	if (m_height < 1.f) {
		Vector3 pos = s_waterPlane->GetModel()->m_model.GetPosition();
		const float speed = 4.f;
		m_height = pos.y + deltaSeconds * speed;
		s_waterPlane->GetModel()->m_model.ChangePosition(pos + Vector3(0.f, deltaSeconds * speed, 0.f));
	}
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//RENDER
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------------
VIRTUAL void SceneTypeDRP::Render() const {

	//Render debug text
	static float fontScale = 0.4f;
	static float fontTopLoc = 50.f;
	const Vector2* screenSize = BeirusRenderer::GetScreenSize();
	float lineHeight = Font::GetDefaultFontLineHeight(fontScale);
	String debugSceneTypeStr = "debug_scene_type: Dynamic Environment Mapping";
	String heatmapStr = "showing: heatmap view";

	Font::DrawText2DWithDefaultFont(Vector2(10.f, screenSize->y - fontTopLoc) - Vector2(0.f, 0.f * lineHeight),
		debugSceneTypeStr, fontScale, RGBA::YELLOW);
}