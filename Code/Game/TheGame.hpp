#pragma once

#include "Engine/General/Core/BeirusEngine.hpp"
#include "Engine/Renderer/Renderer/BeirusRenderer.hpp"
#include "Engine/Tools/FBX/SceneImport.hpp"
#include "Engine/Renderer/Lights/LightSystem.hpp"
#include "Engine/Renderer/Material/MaterialBank.hpp"
#include "Engine/EventSystem/NamedProperties.hpp"

enum eDebugSceneType {
	DEBUG_SCENE_TYPE_NONE = 0,
	DEBUG_SCENE_TYPE_SSAO,
	DEBUG_SCENE_TYPE_OIT,
	DEBUG_SCENE_TYPE_HDR,
	DEBUG_SCENE_TYPE_FPLUS
};





class TextureCubemap;
class Scene;
class Light3D;
class Model;
class DebugSceneType;

class TheGame;
extern TheGame* g_theGame;

const float MOUSE_SENSITIVITY				= 0.08f;
const float PLAYER_MOVE_SPEED				= 20.f;
const int	MULTILIGHT_VIEW_MODE_REGULAR	= 2;
const int	FBO_VIEW_MODE_NORMAL			= 0;
const int	MULTILIGHT_VIEW_MODE_HEATMAP	= 1;
const int	FBO_VIEW_MODE_DEPTH				= 1;
const float BRDF_LIGHT_RADIUS				= 50.f;

constexpr Vector3	SHADOWCASTING_LIGHT_POS	= Vector3(-20.f,  10.f, 0.f);
constexpr Vector3	IMESH_POS				= Vector3(90.f, 10.f, 0.f);

class ShadowCastingLight3D;
class ParticleSystem;
class DynamicReflectionProbe;

#define USING_SPONZA_SCENE


class TheGame {
public:
	//INIT DESTROY
	static void Initialize();
	static void Shutdown();

	//DEBUG FOR NOW
	static void Render();

	//SWITCHING VIEW MODES
	static void SwitchToHeatmapView(NamedProperties& eventData);
	static void SwitchToRegularView(NamedProperties& eventData);
	static void SwitchToDebugDepthView(NamedProperties& eventData);

	static bool HasMouseInput() { return s_theGame->m_hasMouseInput; }

	static Scene* GetScene() { return s_theGame->m_scene; }
	//static DynamicReflectionProbe* GetReflectionProbe() { return s_theGame->m_reflectionProbe; }


private:
	//STRUCTORS INIT
	TheGame();
	~TheGame();

	//SCENE INIT
	void InitializeSkybox();
	void InitializeSponzaScene();
	void InitializeParticleDefinitionsAndSystem();

	//CLICKABLES
	void SelectClickables();
	

	//UPDATE
	void UpdateDebugSceneType(float deltaSeconds);
	void Update(float deltaSeconds);


	//INPUT
	void HandleMouseInputCheck();
	void HandlePlayerLook();
	void HandlePlayerMovement(float deltaSeconds);


	//RENDER
	void InternalRender() const;


	//INTERSECTING MESHES
	Model*	m_plane1 = nullptr;
	RGBA	m_color1 = RGBA(1.f, 0.f, 0.f, 0.5f);
	Model*	m_plane2 = nullptr;
	RGBA	m_color2 = RGBA(0.f, 0.f, 1.f, 0.5f);



	TextureCubemap*			m_skyboxTex			= nullptr;
	bool					m_hasMouseInput		= false;
	Model*					m_skybox			= nullptr;
	Scene*					m_scene				= nullptr;
	//DynamicReflectionProbe* m_reflectionProbe	= nullptr;
	//DynamicReflectionProbe* m_selectedProbe = nullptr;

	//GENERAL DEBUG
	bool					m_isCameraLocked	= false;

	//DEBUG SSAO
	DebugSceneType* m_debugSceneType = nullptr;

	Vector3 m_start = Vector3::ZERO;
	Vector3 m_end	= Vector3::ZERO;

	static TheGame* s_theGame;

	
	//COMMANDS
	static void LoadFBXFromFile(Command& args);
	static void LoadMesh(Command& args);
	static void SaveMesh(Command& args);

	static void ExposureVolumeToggle(Command& args);
	static void SetDebugSceneType(Command& args);
	static void SetAmbientLight(Command& args);
	static void SetExposure(Command& args);

	static void CreateReflectionProbe(Command& args);
};