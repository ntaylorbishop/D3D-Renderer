#pragma once

#include "Engine/General/Core/EngineCommon.hpp"
#include "Game/MovingLight.hpp"

class ShadowCastingLight3D;


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//CONSTANTS
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const uint STARTING_NUM_LIGHTS = 0;

const Vector3 AREA_MINS = Vector3(-50.f, 0.f, 9.f);
const Vector3 AREA_MAXS = Vector3(42.f, 60.f, -13.f);

const float LIGHT_SPEED = 10.f;

const float LIGHT_MIN_SIZE = 1.f;
const float LIGHT_MAX_SIZE = 2.f;

const float LIGHT_VELOCITY_MIN = 2.f;
const float LIGHT_VELOCITY_MAX = 10.f;

const float LIGHT_INTENSITY = 5.f;
constexpr Vector3	PARTICLE_SYSTEM_LIGHT_POS	= Vector3(45.f, 10.f, 0.f);
constexpr Vector3	PARTICLE_SYSTEM_POS			= Vector3(10.f, 10.f, 0.f);



//2 to 10


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//LIGHTMANAGER CLASS
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class LightManager {
public:
	//INIT DESTROY
	static void Initialize();
	static void Shutdown();

	//UPDATE
	static void Update(float deltaSeconds);
	
	static void SetForOIT();
	static void SetForOIT_ShowOff();
	static void ResetLights(uint numLightsToSetTo);

	static size_t GetNumLightsInScene() { return s_theLightManager->m_lights.size(); }

	static ShadowCastingLight3D* GetShadowLight() { return s_theLightManager->m_stationaryLight; }
	static size_t GetShadowLightIdx() { return s_theLightManager->m_shadowLightIdx; }

private:
	//STRUCTORS
	LightManager();
	~LightManager();

	float m_age = 0.f;
	std::vector<MovingLight*> m_lights;
	ShadowCastingLight3D* m_stationaryLight	= nullptr;
	uint m_shadowLightIdx = 0;

	static LightManager* s_theLightManager;
};