#include "Game/DebugSceneTypes/SceneTypeShadows.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Fonts/Font.hpp"
#include "Engine/Renderer/Renderer/BeirusRenderer.hpp"
#include "Engine/Renderer/Effects/SSAO.hpp"
#include "Engine/Renderer/Lights/ShadowCastingLight3D.hpp"
#include "Engine/Renderer/Mesh/Model.hpp"
#include "Engine/Scene/Scene.hpp"
#include "Game/LightManager.hpp"
#include "Game/TheGame.hpp"
#include "Game/ShadowSphere.hpp"


STATIC ShadowSphere* SceneTypeShadows::s_lightSphere = nullptr;


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//STRUCTORS
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------------
SceneTypeShadows::SceneTypeShadows() {

	m_shadowLight = LightManager::GetShadowLight();
	m_shadowLight->SetToMoving(true);

	float angle = 0.f;
	float numSpheres = 16.f;
	float circleRadius = 7.f;

	for (size_t i = 0; i < (size_t)numSpheres; i++) {

		Vector3 diff = Vector3(CosDegrees(angle), 0.f, SinDegrees(angle));
		Vector3 pos = SHADOWCASTING_LIGHT_POS + circleRadius * diff;
		ShadowSphere* sphere = new ShadowSphere(pos);

		angle += (360.f / numSpheres) * (float)i;

		m_spheres.push_back(sphere);
	}

	if (s_lightSphere == nullptr) {
		s_lightSphere = new ShadowSphere(m_shadowLight->GetPosition());
	}
	else {
		s_lightSphere->SetPos(m_shadowLight->GetPosition());
	}
}


//---------------------------------------------------------------------------------------------------------------------------
SceneTypeShadows::~SceneTypeShadows() {

	m_shadowLight->SetToMoving(false);
	s_lightSphere->SetPos(SHADOWCASTING_LIGHT_POS + Vector3(2000.f, 2000.f, 2000.f));

	for (size_t i = 0; i < 16; i++) {
		delete m_spheres[i];
		m_spheres[i] = nullptr;
	}
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//UPDATE
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------------
void SceneTypeShadows::HandleToggleMoveLight() {

	if (InputSystem::GetKeyDown('T')) {
		m_isLightMoving = !m_isLightMoving;
	}
}


//---------------------------------------------------------------------------------------------------------------------------
void SceneTypeShadows::MoveLight(float deltaSeconds) {

	Vector3 pos = m_shadowLight->GetPosition();

	float delta = cos(m_age / 100.f * 1.f) * 40.f;
	pos = Vector3(delta, SHADOWCASTING_LIGHT_POS.y, 0.f);
	m_shadowLight->SetPosition(pos);
	size_t shadowLightIdx = LightManager::GetShadowLightIdx();
	
	LightSystem::GetLights()->at(shadowLightIdx)->SetPosition(pos);
	s_lightSphere->SetPos(m_shadowLight->GetPosition());
}


//---------------------------------------------------------------------------------------------------------------------------
void SceneTypeShadows::UpdateSpheres(float deltaSeconds) {

	float angle = 0.f;
	float numSpheres = 16.f;
	float circleRadius = 7.f;

	for (size_t i = 0; i < (size_t)numSpheres; i++) {

		ShadowSphere* sphereModel = m_spheres[i];
		Vector3 startPos = sphereModel->GetStartPos();
		Vector3 pos = SHADOWCASTING_LIGHT_POS
			+ circleRadius * Vector3(CosDegrees(angle + m_age), 0.f, SinDegrees(angle + m_age));

		angle += (360.f / numSpheres) * (float)i;
		sphereModel->SetPos(pos);
	}
}


//---------------------------------------------------------------------------------------------------------------------------
void SceneTypeShadows::HandleAttachLight() {
	
	if (InputSystem::GetKeyDown('O')) {
		m_isAttached = !m_isAttached;
	}

	if (m_isAttached) {
		Vector3 camPos = BeirusRenderer::GetPlayerCamera()->m_position + Vector3(0.f, 5.f, 0.f);
		s_lightSphere->SetPos(SHADOWCASTING_LIGHT_POS + Vector3(0.f, -200.f, 0.f));

		m_shadowLight->SetPosition(camPos);
		size_t shadowLightIdx = LightManager::GetShadowLightIdx();
		LightSystem::GetLights()->at(shadowLightIdx)->SetPosition(camPos);
		s_lightSphere->SetPos(camPos);
	}
}


//---------------------------------------------------------------------------------------------------------------------------
VIRTUAL void SceneTypeShadows::Update(float deltaSeconds) {

	m_age += deltaSeconds * 100.f;

	HandleToggleMoveLight();
	UpdateSpheres(deltaSeconds);
	HandleAttachLight();

	if (m_isLightMoving && !m_isAttached) {
		MoveLight(deltaSeconds);
		return;
	}
	
	if (InputSystem::GetKey(VK_LEFT)) {

		Vector3 pos = m_shadowLight->GetPosition();
		pos += Vector3(0.f, 0.f, -1.f) * deltaSeconds * SHADOW_MOVE_SPEED;
		size_t shadowLightIdx = LightManager::GetShadowLightIdx();

		m_shadowLight->SetPosition(pos);
		LightSystem::GetLights()->at(shadowLightIdx)->SetPosition(pos);
	}
	else if (InputSystem::GetKey(VK_RIGHT)) {

		Vector3 pos = m_shadowLight->GetPosition();
		pos += Vector3(0.f, 0.f, 1.f) * deltaSeconds * SHADOW_MOVE_SPEED;
		size_t shadowLightIdx = LightManager::GetShadowLightIdx();

		m_shadowLight->SetPosition(pos);
		LightSystem::GetLights()->at(shadowLightIdx)->SetPosition(pos);

	}
	else if (InputSystem::GetKey(VK_DOWN)) {

		Vector3 pos = m_shadowLight->GetPosition();
		pos += Vector3(-1.f, 0.f, 0.f) * deltaSeconds * SHADOW_MOVE_SPEED;
		size_t shadowLightIdx = LightManager::GetShadowLightIdx();

		m_shadowLight->SetPosition(pos);
		LightSystem::GetLights()->at(shadowLightIdx)->SetPosition(pos);
	}
	else if (InputSystem::GetKey(VK_UP)) {

		Vector3 pos = m_shadowLight->GetPosition();
		pos += Vector3(1.f, 0.f, 0.f) * deltaSeconds * SHADOW_MOVE_SPEED;
		size_t shadowLightIdx = LightManager::GetShadowLightIdx();

		m_shadowLight->SetPosition(pos);
		LightSystem::GetLights()->at(shadowLightIdx)->SetPosition(pos);
	}
	else if (InputSystem::GetKey('I')) {

		Vector3 pos = m_shadowLight->GetPosition();
		pos += Vector3(0.f, 1.f, 0.f) * deltaSeconds * SHADOW_MOVE_SPEED;
		size_t shadowLightIdx = LightManager::GetShadowLightIdx();

		m_shadowLight->SetPosition(pos);
		LightSystem::GetLights()->at(shadowLightIdx)->SetPosition(pos);
	}
	else if (InputSystem::GetKey('K')) {

		Vector3 pos = m_shadowLight->GetPosition();
		pos += Vector3(0.f, -1.f, 0.f) * deltaSeconds * SHADOW_MOVE_SPEED;
		size_t shadowLightIdx = LightManager::GetShadowLightIdx();

		m_shadowLight->SetPosition(pos);
		LightSystem::GetLights()->at(shadowLightIdx)->SetPosition(pos);
	}

	if (!m_isAttached) {
		s_lightSphere->SetPos(m_shadowLight->GetPosition());
	}
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//RENDER
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------------
VIRTUAL void SceneTypeShadows::Render() const {

	//Render debug text
	static float fontScale = 0.4f;
	static float fontTopLoc = 50.f;
	const Vector2* screenSize = BeirusRenderer::GetScreenSize();
	float lineHeight = Font::GetDefaultFontLineHeight(fontScale);
	String debugSceneTypeStr = "debug_scene_type: Shadowmapping";
	
	String lightMovingStr = (m_isLightMoving) ? ("T: Disable moving light") : ("T: Enable moving light");
	String attachedStr = (m_isAttached) ? ("O: Detach light from camera") : ("A: Attach light to camera");

	Font::DrawText2DWithDefaultFont(Vector2(10.f, screenSize->y - fontTopLoc) - Vector2(0.f, 0.f * lineHeight),
		debugSceneTypeStr, fontScale, RGBA::YELLOW);
	Font::DrawText2DWithDefaultFont(Vector2(10.f, screenSize->y - fontTopLoc) - Vector2(0.f, 1.f * lineHeight),
		lightMovingStr, fontScale, RGBA::YELLOW);
	Font::DrawText2DWithDefaultFont(Vector2(10.f, screenSize->y - fontTopLoc) - Vector2(0.f, 2.f * lineHeight),
		attachedStr, fontScale, RGBA::YELLOW);

	//CONTROLS
	//Font::DrawText2DWithDefaultFont(Vector2(10.f, screenSize->y - fontTopLoc) - Vector2(0.f, 2.f * lineHeight),
	//	"Left, right: Change sorting type", fontScale, RGBA::YELLOW);
	//Font::DrawText2DWithDefaultFont(Vector2(10.f, screenSize->y - fontTopLoc) - Vector2(0.f, 3.f * lineHeight),
	//	"Up, down: Switch buffer view", fontScale, RGBA::YELLOW);
}