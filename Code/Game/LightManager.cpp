#include "Game/LightManager.hpp"
#include "Game/TheGame.hpp"
#include "Engine/Renderer/Shaders/ShaderProgram.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Lights/ShadowCastingLight3D.hpp"

STATIC LightManager* LightManager::s_theLightManager = nullptr;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//STATIC INIT DESTROY
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------------
STATIC void LightManager::Initialize() {

	ASSERT_OR_DIE(s_theLightManager == nullptr, "ERROR: LightManager already initialized.");
	s_theLightManager = new LightManager();
}


//---------------------------------------------------------------------------------------------------------------------------
STATIC void LightManager::Shutdown() {

	ASSERT_OR_DIE(s_theLightManager != nullptr, "ERROR: LightManager already destroyed.");
	delete s_theLightManager;
	s_theLightManager = nullptr;
}


//---------------------------------------------------------------------------------------------------------------------------
STATIC void LightManager::Update(float deltaSeconds) {

	s_theLightManager->m_age += deltaSeconds / 5.f;


	for (uint i = 0; i < s_theLightManager->m_lights.size(); i++) {

		MovingLight* currLight = s_theLightManager->m_lights[i];
		currLight->Update(deltaSeconds);

		Vector3 lightPos = currLight->GetPosition();

		if (lightPos.y >= AREA_MAXS.y) {
			s_theLightManager->m_lights[i]->SetPosition(Vector3(lightPos.x, AREA_MINS.y, lightPos.z));
		}
	}
}


//---------------------------------------------------------------------------------------------------------------------------
STATIC void LightManager::SetForOIT() {

	LightSystem::ClearLights();
	s_theLightManager->m_lights.clear();

	s_theLightManager->m_lights.push_back(new MovingLight(PARTICLE_SYSTEM_LIGHT_POS, Vector3::ZERO, RGBA::RED, 5.f, LIGHT_INTENSITY));
	return;
}


//---------------------------------------------------------------------------------------------------------------------------
STATIC void LightManager::SetForOIT_ShowOff() {

	LightSystem::ClearLights();
	s_theLightManager->m_lights.clear();

	for (uint i = 0; i < 20; i++) {

		Vector3 startingPos = PARTICLE_SYSTEM_POS + Vector3(Randf(-5.f, 5.f), Randf(AREA_MINS.y, AREA_MAXS.y), Randf(-5.f, 5.f));
		Vector3 vel = Vector3(0.f, Randf(LIGHT_VELOCITY_MIN, LIGHT_VELOCITY_MAX), 0.f);
		RGBA color = RGBA(Randf(0.f, 1.f), Randf(0.f, 1.f), Randf(0.f, 1.f), 1.f);
		float size = Randf(LIGHT_MIN_SIZE, LIGHT_MAX_SIZE);

		s_theLightManager->m_lights.push_back(new MovingLight(startingPos, vel, color, size, LIGHT_INTENSITY));
	}


	s_theLightManager->m_stationaryLight = new ShadowCastingLight3D(SHADOWCASTING_LIGHT_POS, RGBA::WHITE, 20.f);
	s_theLightManager->m_shadowLightIdx = LightSystem::AddLight(s_theLightManager->m_stationaryLight);
}


//---------------------------------------------------------------------------------------------------------------------------
STATIC void LightManager::ResetLights(uint numLightsToSetTo) {

	LightSystem::ClearLights();
	s_theLightManager->m_lights.clear();

	for (uint i = 0; i < numLightsToSetTo; i++) {

		int which = 0;
		which = RandInt(0, 1);
		float z;
		if (which == 0) {
			z = Randf(AREA_MINS.z - 1.f, AREA_MINS.z + 1.f);
		}
		else if (which == 1) {
			z = Randf(AREA_MAXS.z - 1.f, AREA_MAXS.z + 1.f);
		}

		Vector3 startingPos = Vector3(Randf(AREA_MINS.x, AREA_MAXS.x), Randf(AREA_MINS.y, AREA_MAXS.y), z);
		Vector3 vel = Vector3(0.f, Randf(LIGHT_VELOCITY_MIN, LIGHT_VELOCITY_MAX), 0.f);
		RGBA color = RGBA(Randf(0.f, 1.f), Randf(0.f, 1.f), Randf(0.f, 1.f), 1.f);
		float size = Randf(LIGHT_MIN_SIZE, LIGHT_MAX_SIZE);

		s_theLightManager->m_lights.push_back(new MovingLight(startingPos, vel, color, size, LIGHT_INTENSITY));
	}

	s_theLightManager->m_shadowLightIdx = LightSystem::AddLight(s_theLightManager->m_stationaryLight);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//STRUCTORS
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------------
LightManager::LightManager() {

	for (uint i = 0; i < STARTING_NUM_LIGHTS; i++) {

		int which = 0;
		which = RandInt(0, 1);
		float z;
		if (which == 0) {
			z = Randf(AREA_MINS.z - 1.f, AREA_MINS.z + 1.f);
		}
		else if (which == 1) {
			z = Randf(AREA_MAXS.z - 1.f, AREA_MAXS.z + 1.f);
		}



		Vector3 startingPos = Vector3(Randf(AREA_MINS.x, AREA_MAXS.x), AREA_MINS.y, z);
		Vector3 vel = Vector3(0.f, Randf(LIGHT_VELOCITY_MIN, LIGHT_VELOCITY_MAX), 0.f);
		RGBA color = RGBA(Randf(0.f, 1.f), Randf(0.f, 1.f), Randf(0.f, 1.f), 1.f);
		float size = Randf(LIGHT_MIN_SIZE, LIGHT_MAX_SIZE);


		m_lights.push_back(new MovingLight(startingPos, vel, color, size, LIGHT_INTENSITY));
	}

	//DEFAULT SIZE = 20.f;
	m_stationaryLight = new ShadowCastingLight3D(SHADOWCASTING_LIGHT_POS, RGBA::WHITE,	0.f);
	LightSystem::AddLight(m_stationaryLight);
}


//---------------------------------------------------------------------------------------------------------------------------
LightManager::~LightManager() {

	for (uint i = 0; i < m_lights.size(); i++) {
		delete m_lights[i];
	}
}