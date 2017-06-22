#include "Game/ShadowSphere.hpp"
#include "Game/TheGame.hpp"
#include "Engine/Scene/Scene.hpp"
#include "Engine/Renderer/Mesh/Model.hpp"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//STRUCTORS
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------------
ShadowSphere::ShadowSphere(const Vector3& pos) 
	: m_startPos(pos)
{
	m_model = new Model(pos, PRIMITIVE_MESH_SPHERE_100);
	m_model->SetShadowCaster(false);

	m_model->m_material = new Material("Unlit");

	Texture* diffuseTex = Texture::CreateOrGetTexture("Data/Textures/Black.png");
	Texture* normTex	= Texture::CreateOrGetTexture("Data/Textures/Black_Normal.png");
	m_model->m_material->CreateUniform("gTexDiffuse",		UNIFORM_TEXTURE2D,	1, 0, diffuseTex);
	m_model->m_material->CreateUniform("gTexNormal",		UNIFORM_TEXTURE2D,	1, 1, normTex);
	m_model->m_material->CreateUniform("gDiffuseColor",		UNIFORM_RGBA,		1, &m_color);
	m_model->m_material->CreateUniform("gUsesDiffuseTex",	UNIFORM_UINT,		1, &UINT_FALSE);

	m_model->m_model.Scale(Vector3(SPHERE_SCALE, SPHERE_SCALE, SPHERE_SCALE));

	TheGame::GetScene()->AddStaticMesh(m_model);
}


//---------------------------------------------------------------------------------------------------------------------------
ShadowSphere::~ShadowSphere() {

	TheGame::GetScene()->RemoveStaticMesh(m_model);
	delete m_model;
	m_model = nullptr;
}