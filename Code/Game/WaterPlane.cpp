#include "Game/WaterPlane.hpp"
#include "Engine/Renderer/Mesh/Model.hpp"
#include "Engine/Renderer/Material/Material.hpp"
#include "Engine/Renderer/Structures/BeirusMeshCollection.hpp"
#include "Engine/Renderer/Effects/SSAO.hpp"
#include "Engine/General/Core/BeirusEngine.hpp"
#include "Engine/Renderer/General/TextureCubemap.hpp"

STATIC uint8_t	WaterPlane::s_numWaterPlanes	= 0;
STATIC size_t	WaterPlane::s_surfaceMeshID		= 0;



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//STRUCTORS
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------------
WaterPlane::WaterPlane(const Vector3& centerPosition, const Vector3& scale) 
	: m_position(centerPosition)
	, m_waterPlaneID(s_numWaterPlanes++)
{

	if (m_waterPlaneID == 0) {
		CreateSurfaceMesh();
	}

	m_model = new Model(centerPosition);
	m_model->m_meshID = s_surfaceMeshID;
	m_material = new Material("Water");
	m_model->m_material = m_material;

	m_model->m_model.ChangePosition(centerPosition);
	m_model->m_model.Scale(scale);

	Texture* diffuseTex = Texture::CreateOrGetTexture("Data/Textures/Blue.png");
	Texture* normTex	= Texture::CreateOrGetTexture("Data/Textures/WaterNormal2.jpg");
	Texture* dispTex	= Texture::CreateOrGetTexture("Data/Textures/WaterNormal.jpg");

	m_material->CreateUniform("gTexDiffuse",		UNIFORM_TEXTURE2D,			1, 0, diffuseTex);
	m_material->CreateUniform("gTexNormal",			UNIFORM_TEXTURE2D,			1, 1, normTex);
	m_material->CreateUniform("gAmbientLight",		UNIFORM_RGBA,				1, &WATER_AMBIENT_LIGHT);
	m_material->CreateUniform("gTime",				UNIFORM_FLOAT,				1, &m_age);
	m_material->CreateUniform("gTexDisplacement",	UNIFORM_TEXTURE2D,			1, 4, dispTex);
	//m_model->m_renderState.m_blendMode = BLEND_MODE_TRANSPARENT_DEFAULT;

	BeirusEngine::RegisterUpdateCallback(MakeDelegate(this, &WaterPlane::Update));
}


//---------------------------------------------------------------------------------------------------------------------------
WaterPlane::~WaterPlane() {

}


void WaterPlane::Update(float deltaSeconds) {

	m_age += deltaSeconds / 100.f;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//INTERNAL
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------------
STATIC void WaterPlane::CreateSurfaceMesh() {

	MeshID newMeshID	= BeirusMeshCollection::Get()->Allocate(&VertexDefinition::TEXTUREDVERTEX_TBN);
	Mesh* surfaceMesh	= BeirusMeshCollection::Get()->GetMesh(newMeshID);

	std::vector<TexturedVertex_TBN> verts;
	std::vector<uint16_t>			inds;

	for (size_t z = 0; z < WATER_FIDELITY; z++) {
		for (size_t x = 0; x < WATER_FIDELITY; x++) {
			verts.push_back(TexturedVertex_TBN(Vector3((float)x, 0.f, (float)z), RGBA::WHITE, 
				Vector2(1.f / (float)WATER_FIDELITY, 1.f / (float)WATER_FIDELITY) * Vector2(x, z),
				Vector3(1.f, 0.f, 0.f), Vector3(0.f, 0.f, 1.f)
			));

			if (x < WATER_FIDELITY - 1 && z < WATER_FIDELITY - 1) {
				inds.push_back((uint16_t)(WATER_FIDELITY *  z + x));
				inds.push_back((uint16_t)(WATER_FIDELITY * (z + 1) + x));
				inds.push_back((uint16_t)(WATER_FIDELITY * (z + 1) + x + 1));
				inds.push_back((uint16_t)(WATER_FIDELITY *  z + x));
				inds.push_back((uint16_t)(WATER_FIDELITY * (z + 1) + x + 1));
				inds.push_back((uint16_t)(WATER_FIDELITY *  z + x + 1));
			}
		}
	}

	surfaceMesh->UpdateMesh(verts, inds);
	s_surfaceMeshID = newMeshID;
}