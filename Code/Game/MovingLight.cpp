#include "Engine/Renderer/Lights/LightSystem.hpp"
#include "Engine/Renderer/Mesh/Model.hpp"
#include "Engine/Renderer/Mesh/Mesh.hpp"
#include "Engine/Renderer/Material/Material.hpp"
#include "Engine/Renderer/Material/MaterialBank.hpp"
#include "Engine/Scene/Scene.hpp"
#include "Engine/General/Core/BeirusEngine.hpp"
#include "Engine/General/Time/Clock.hpp"
#include "Game/MovingLight.hpp"
#include "Game/TheGame.hpp"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//STRUCTORS
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------------
MovingLight::MovingLight(const Vector3& startingPos, const Vector3& startingVel, const RGBA& color, float size, float intensity) 
	: m_position(startingPos)
	, m_velocity(startingVel)
	, m_color(color)
	, m_size(size)
	, m_lightIdx(0)
	, m_model(nullptr)
	, m_modelMat(Matrix4::IDENTITY)
	, m_intensity(intensity)
{
	Light3D* newLight = new Light3D(m_position, m_color, m_size, m_intensity);
	m_lightIdx = LightSystem::AddLight(newLight);

	m_model = new Model(startingPos, PRIMITIVE_MESH_SPHERE_10);
	m_model->SetShadowCaster(false);

	m_model->m_material = new Material("Unlit");
	
	Texture* diffuseTex = Texture::CreateOrGetTexture("Data/Textures/Black.png");
	Texture* normTex	= Texture::CreateOrGetTexture("Data/Textures/Black_Normal.png");
	m_model->m_material->CreateUniform("gTexDiffuse",		UNIFORM_TEXTURE2D,	1, 0, diffuseTex);
	m_model->m_material->CreateUniform("gTexNormal",		UNIFORM_TEXTURE2D,	1, 1, normTex);
	m_model->m_material->CreateUniform("gDiffuseColor",		UNIFORM_RGBA,		1, &m_color);
	m_model->m_material->CreateUniform("gUsesDiffuseTex",	UNIFORM_UINT,		1, &UINT_FALSE);

	m_model->m_model.Scale(Vector3(0.1f, 0.1f, 0.1f));

	//TheGame::GetScene()->AddStaticMesh(m_model);
}


//---------------------------------------------------------------------------------------------------------------------------
MovingLight::~MovingLight() {
	delete m_model;
	m_model = nullptr;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//UPDATE
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------------
void MovingLight::Update(float deltaSeconds) {

	m_position += m_velocity * deltaSeconds;

	LightSystem::GetLights()->at(m_lightIdx)->SetPosition(m_position);
	m_model->m_model.ChangePosition(m_position);
}