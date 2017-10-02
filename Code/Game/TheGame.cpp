#include "Game/TheGame.hpp"
#include "Game/TheApp.hpp"
#include "Engine/Renderer/D3D11/General/RHIDeviceWindow.hpp"
#include "Engine/Renderer/D3D11/Shaders/D3D11VertexShader.hpp"
#include "Engine/Renderer/D3D11/Shaders/D3D11PixelShader.hpp"
#include "Engine/Renderer/D3D11/Mesh/D3D11Mesh.hpp"
#include "Engine/Renderer/D3D11/Resources/D3D11ConstantBuffer.hpp"
#include "Engine/Renderer/D3D11/Texture/Texture2D.hpp"
#include "Engine/Renderer/D3D11/Resources/D3D11SamplerState.hpp"
#include "Engine/Renderer/D3D11/General/RHI.hpp"
#include "Engine/Renderer/D3D11/Shaders/D3D11ShaderProgram.hpp"
#include "Engine/Renderer/D3D11/Mesh/D3D11Vertex.hpp"
#include "Engine/Renderer/Lights/Light3D.hpp"
#include "Engine/Renderer/D3D11/General/D3D11Renderer.hpp"
#include "Engine/Renderer/D3D11/Material/D3D11Material.hpp"
#include "Engine/Renderer/D3D11/Mesh/D3D11MeshRenderer.hpp"
#include "Engine/UI/UIRenderer.hpp"
#include "Engine/Console/Console.hpp"
#include <d3dcompiler.h>

TheGame* TheGame::s_theGame = nullptr;


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//STATIC INIT DESTROY
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------------
STATIC void TheGame::Initialize(HINSTANCE applicationInstanceHandle, int nCmdShow) {

	s_theGame = new TheGame(applicationInstanceHandle, nCmdShow);
	BeirusEngine::RegisterUpdateCallback(MakeDelegate(s_theGame, &TheGame::Update));
}


//---------------------------------------------------------------------------------------------------------------------------
STATIC void TheGame::Shutdown() {

	BeirusEngine::UnregisterUpdateCallback(MakeDelegate(s_theGame, &TheGame::Update));
	delete s_theGame;
}



STATIC TheGame* TheGame::Get() {

	if (s_theGame == nullptr) {
		ERROR_AND_DIE("nuuu");
	}

	return s_theGame;
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//STRUCTORS INITIALIZATION
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using namespace DirectX;

//--------------------------------------------------------------------------------------
// Structures
//--------------------------------------------------------------------------------------
struct SimpleVertex
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
};


struct ConstantBuffer
{
	XMMATRIX mWorld;
	XMMATRIX mView;
	XMMATRIX mProjection;
};


XMMATRIX				m_World;
XMMATRIX				m_World2;

XMMATRIX				m_localModel2;
XMMATRIX				m_View;
XMMATRIX				m_Projection;
D3D11Mesh*				g_mesh;
D3D11Material*			brickMat;
D3D11Material*			brickMat2;

D3D11MeshRenderer		g_meshRenderer;

XMMATRIX				m_localModel;
XMMATRIX				m_localView;
XMMATRIX				m_localProjection;
ID3D11Debug*			g_debugDevice = nullptr;

Light3D m_light;


Texture2D* m_texDiffuse = nullptr;
Texture2D* m_texNormal = nullptr;

UINT SCREEN_SIZE_X = 3840;
UINT SCREEN_SIZE_Y = 2160;
const float MOUSE_SENSITIVITY = 0.08f;
const float PLAYER_MOVE_SPEED = 20.f;



//---------------------------------------------------------------------------------------------------------------------------
TheGame::TheGame(HINSTANCE applicationInstanceHandle, int nCmdShow) {

	RHIDeviceWindow::Initialize(applicationInstanceHandle, nCmdShow);

	D3D11Renderer::Get();

	InputSystem::HideMouseCursor();
	Console::Initialize(CONSOLE_KEYBIND);

	//HRESULT hr = GetDevice()->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&g_debugDevice));
	//ASSERT_OR_DIE(SUCCEEDED(hr), "ERROR: Could not query d3d11 debug device.");

	UINT width = SCREEN_SIZE_X;
	UINT height = SCREEN_SIZE_Y;

	m_playerCamera = Camera3D(Vector3(0.f, 1.f, -5.f), 0.f, 0.f, 0.f);
	m_playerCamera.SetPerspectiveProjection(90.f, (float)(width / height), 0.01f, 1000.f);

	CreateMesh();
	CreateShaderProgram();

	//Initialize MATs	
	m_localModel = XMMatrixIdentity();
	m_localModel2 = XMMatrixIdentity();
}


//---------------------------------------------------------------------------------------------------------------------------
TheGame::~TheGame() {
	RHIDeviceWindow::Destroy();
}


//---------------------------------------------------------------------------------------------------------------------------
void TheGame::CreateShaderProgram() {
	
	brickMat = new D3D11Material("BlinnPhong");

	brickMat->CreateUniform("uModel", UNIFORM_MAT4, &m_World);
	brickMat->CreateUniform("uView", UNIFORM_MAT4, &m_playerCamera.m_view);
	brickMat->CreateUniform("uProj", UNIFORM_MAT4, &m_playerCamera.m_proj);

	m_light.m_position = Vector3(0.f, 10.f, 0.f);
	m_light.m_color = RGBA::WHITE;
	m_light.m_minLightDistance = 9.f;
	m_light.m_maxLightDistance = 11.f;
	m_light.m_powerAtMin = 1.f;
	m_light.m_powerAtMax = 0.f;

	brickMat->CreateUniform("color",			UNIFORM_RGBA,		&m_light.m_color);
	brickMat->CreateUniform("minLightDistance",	UNIFORM_FLOAT,		&m_light.m_minLightDistance);
	brickMat->CreateUniform("maxLightDistance",	UNIFORM_FLOAT,		&m_light.m_maxLightDistance);
	brickMat->CreateUniform("powerAtMin",		UNIFORM_FLOAT,		&m_light.m_powerAtMin);
	brickMat->CreateUniform("powerAtMax",		UNIFORM_FLOAT,		&m_light.m_powerAtMax);
	brickMat->CreateUniform("position",			UNIFORM_VECTOR3,	&m_light.m_position);
	brickMat->CreateUniform("CameraPos",		UNIFORM_VECTOR3,	&m_playerCamera.m_position);

	Texture2D* defaultDiffuse = Texture2D::GetTexture("Data/Textures/Brick2.png");
	Texture2D* defaultNormal = Texture2D::GetTexture("Data/Textures/Brick_Normal.png");

	D3D11Resource* texID = defaultDiffuse->GetSRVResource();
	D3D11Resource* normID = defaultNormal->GetSRVResource();

	brickMat->AddResource(0, texID, WHICH_SHADER_FRAGMENT);
	brickMat->AddResource(1, normID, WHICH_SHADER_FRAGMENT);

	
	brickMat2 = new D3D11Material("BlinnPhong");

	brickMat2->CreateUniform("uModel",				UNIFORM_MAT4,		&m_World);
	brickMat2->CreateUniform("uView",				UNIFORM_MAT4,		&m_playerCamera.m_view);
	brickMat2->CreateUniform("uProj",				UNIFORM_MAT4,		&m_playerCamera.m_proj);

	brickMat2->CreateUniform("color",				UNIFORM_RGBA,		&m_light.m_color);
	brickMat2->CreateUniform("minLightDistance",	UNIFORM_FLOAT,		&m_light.m_minLightDistance);
	brickMat2->CreateUniform("maxLightDistance",	UNIFORM_FLOAT,		&m_light.m_maxLightDistance);
	brickMat2->CreateUniform("powerAtMin",			UNIFORM_FLOAT,		&m_light.m_powerAtMin);
	brickMat2->CreateUniform("powerAtMax",			UNIFORM_FLOAT,		&m_light.m_powerAtMax);
	brickMat2->CreateUniform("position",			UNIFORM_VECTOR3,	&m_light.m_position);
	brickMat2->CreateUniform("CameraPos",			UNIFORM_VECTOR3,	&m_playerCamera.m_position);

	brickMat2->AddResource(0, texID, WHICH_SHADER_FRAGMENT);
	brickMat2->AddResource(1, normID, WHICH_SHADER_FRAGMENT);
}


RGBA tint = RGBA::WHITE;
Vector4 texCoordsQuad = Vector4(0.f, 0.f, 1.f, 1.f);



//---------------------------------------------------------------------------------------------------------------------------
void TheGame::CreateMesh() {

	// Create vertex buffer
	g_mesh = new D3D11Mesh(VERTEX_TYPE_PCTTBN, 24);

	float scale = 1.f;

	//FRONT FACE
	g_mesh->AddVertex(Vector3(-scale, scale, scale), RGBA::WHITE, Vector2(1.f, 0.f),
		Vector4(-1.f, 0.f, 0.f, 0.f), Vector4(0.f, 0.f, 1.f, 0.f), 
		Vector4(CrossProduct(Vector3(-1.f, 0.f, 0.f), Vector3(0.f, 0.f, 1.f)), 0.f));
	g_mesh->AddVertex(Vector3(-scale, scale, -scale), RGBA::WHITE, Vector2(1.f, 1.f),
		Vector4(-1.f, 0.f, 0.f, 0.f), Vector4(0.f, 0.f, 1.f, 0.f), 
		Vector4(CrossProduct(Vector3(-1.f, 0.f, 0.f), Vector3(0.f, 0.f, 1.f)), 0.f));
	g_mesh->AddVertex(Vector3(scale, scale, -scale), RGBA::WHITE, Vector2(0.f, 1.f),
		Vector4(-1.f, 0.f, 0.f, 0.f), Vector4(0.f, 0.f, 1.f, 0.f), 
		Vector4(CrossProduct(Vector3(-1.f, 0.f, 0.f), Vector3(0.f, 0.f, 1.f)), 0.f));
	g_mesh->AddVertex(Vector3(scale, scale, scale), RGBA::WHITE, Vector2(0.f, 0.f),
		Vector4(-1.f, 0.f, 0.f, 0.f), Vector4(0.f, 0.f, 1.f, 0.f), 
		Vector4(CrossProduct(Vector3(-1.f, 0.f, 0.f), Vector3(0.f, 0.f, 1.f)), 0.f));

	//BACK FACE
	g_mesh->AddVertex(Vector3(-scale, -scale, scale), RGBA::WHITE, Vector2(0.f, 0.f),
		Vector4(1.f, 0.f, 0.f, 0.f), Vector4(0.f, 0.f, 1.f, 0.f), 
		Vector4(CrossProduct(Vector3(1.f, 0.f, 0.f), Vector3(0.f, 0.f, 1.f)), 0.f));
	g_mesh->AddVertex(Vector3(-scale, -scale, -scale), RGBA::WHITE, Vector2(0.f, 1.f),
		Vector4(1.f, 0.f, 0.f, 0.f), Vector4(0.f, 0.f, 1.f, 0.f), 
		Vector4(CrossProduct(Vector3(1.f, 0.f, 0.f), Vector3(0.f, 0.f, 1.f)), 0.f));
	g_mesh->AddVertex(Vector3(scale, -scale, -scale), RGBA::WHITE, Vector2(1.f, 1.f),
		Vector4(1.f, 0.f, 0.f, 0.f), Vector4(0.f, 0.f, 1.f, 0.f), 
		Vector4(CrossProduct(Vector3(1.f, 0.f, 0.f), Vector3(0.f, 0.f, 1.f)), 0.f));
	g_mesh->AddVertex(Vector3(scale, -scale, scale), RGBA::WHITE, Vector2(1.f, 0.f),
		Vector4(1.f, 0.f, 0.f, 0.f), Vector4(0.f, 0.f, 1.f, 0.f), 
		Vector4(CrossProduct(Vector3(1.f, 0.f, 0.f), Vector3(0.f, 0.f, 1.f)), 0.f));

	//BOTTOM FACE																					
	g_mesh->AddVertex(Vector3(-scale, scale, -scale), RGBA::WHITE, Vector2(0.f, 1.f),
		Vector4(1.f, 0.f, 0.f, 0.f), Vector4(0.f, -1.f, 0.f, 0.f), 
		Vector4(CrossProduct(Vector3(1.f, 0.f, 0.f), Vector3(0.f, -1.f, 0.f)), 0.f));
	g_mesh->AddVertex(Vector3(scale, scale, -scale), RGBA::WHITE, Vector2(1.f, 1.f),
		Vector4(1.f, 0.f, 0.f, 0.f), Vector4(0.f, -1.f, 0.f, 0.f), 
		Vector4(CrossProduct(Vector3(1.f, 0.f, 0.f), Vector3(0.f, -1.f, 0.f)), 0.f));
	g_mesh->AddVertex(Vector3(scale, -scale, -scale), RGBA::WHITE, Vector2(1.f, 0.f),
		Vector4(1.f, 0.f, 0.f, 0.f), Vector4(0.f, -1.f, 0.f, 0.f), 
		Vector4(CrossProduct(Vector3(1.f, 0.f, 0.f), Vector3(0.f, -1.f, 0.f)), 0.f));
	g_mesh->AddVertex(Vector3(-scale, -scale, -scale), RGBA::WHITE, Vector2(0.f, 0.f),
		Vector4(1.f, 0.f, 0.f, 0.f), Vector4(0.f, -1.f, 0.f, 0.f), 
		Vector4(CrossProduct(Vector3(1.f, 0.f, 0.f), Vector3(0.f, -1.f, 0.f)), 0.f));

	//TOP FACE																						
	g_mesh->AddVertex(Vector3(-scale, scale, scale), RGBA::WHITE, Vector2(0.f, 0.f),
		Vector4(1.f, 0.f, 0.f, 0.f), Vector4(0.f, 1.f, 0.f, 0.f), 
		Vector4(CrossProduct(Vector3(1.f, 0.f, 0.f), Vector3(0.f, 1.f, 0.f)), 0.f));
	g_mesh->AddVertex(Vector3(-scale, -scale, scale), RGBA::WHITE, Vector2(0.f, 1.f),
		Vector4(1.f, 0.f, 0.f, 0.f), Vector4(0.f, 1.f, 0.f, 0.f), 
		Vector4(CrossProduct(Vector3(1.f, 0.f, 0.f), Vector3(0.f, 1.f, 0.f)), 0.f));
	g_mesh->AddVertex(Vector3(scale, -scale, scale), RGBA::WHITE, Vector2(1.f, 1.f),
		Vector4(1.f, 0.f, 0.f, 0.f), Vector4(0.f, 1.f, 0.f, 0.f), 
		Vector4(CrossProduct(Vector3(1.f, 0.f, 0.f), Vector3(0.f, 1.f, 0.f)), 0.f));
	g_mesh->AddVertex(Vector3(scale, scale, scale), RGBA::WHITE, Vector2(1.f, 0.f),
		Vector4(1.f, 0.f, 0.f, 0.f), Vector4(0.f, 1.f, 0.f, 0.f), 
		Vector4(CrossProduct(Vector3(1.f, 0.f, 0.f), Vector3(0.f, 1.f, 0.f)), 0.f));

	//RIGHT FACE																					
	g_mesh->AddVertex(Vector3(scale, scale, -scale), RGBA::WHITE, Vector2(1.f, 1.f),
		Vector4(0.f, 1.f, 0.f, 0.f), Vector4(0.f, 0.f, 1.f, 0.f), 
		Vector4(CrossProduct(Vector3(0.f, 1.f, 0.f), Vector3(0.f, 0.f, 1.f)), 0.f));
	g_mesh->AddVertex(Vector3(scale, scale, scale), RGBA::WHITE, Vector2(1.f, 0.f),
		Vector4(0.f, 1.f, 0.f, 0.f), Vector4(0.f, 0.f, 1.f, 0.f), 
		Vector4(CrossProduct(Vector3(0.f, 1.f, 0.f), Vector3(0.f, 0.f, 1.f)), 0.f));
	g_mesh->AddVertex(Vector3(scale, -scale, scale), RGBA::WHITE, Vector2(0.f, 0.f),
		Vector4(0.f, 1.f, 0.f, 0.f), Vector4(0.f, 0.f, 1.f, 0.f), 
		Vector4(CrossProduct(Vector3(0.f, 1.f, 0.f), Vector3(0.f, 0.f, 1.f)), 0.f));
	g_mesh->AddVertex(Vector3(scale, -scale, -scale), RGBA::WHITE, Vector2(0.f, 1.f),
		Vector4(0.f, 1.f, 0.f, 0.f), Vector4(0.f, 0.f, 1.f, 0.f), 
		Vector4(CrossProduct(Vector3(0.f, 1.f, 0.f), Vector3(0.f, 0.f, 1.f)), 0.f));

	//LEFT FACE																						
	g_mesh->AddVertex(Vector3(-scale, scale, -scale), RGBA::WHITE, Vector2(0.f, 1.f),
		Vector4(0.f, -1.f, 0.f, 0.f), Vector4(0.f, 0.f, 1.f, 0.f), 
		Vector4(CrossProduct(Vector3(0.f, -1.f, 0.f), Vector3(0.f, 0.f, 1.f)), 0.f));
	g_mesh->AddVertex(Vector3(-scale, -scale, -scale), RGBA::WHITE, Vector2(1.f, 1.f),
		Vector4(0.f, -1.f, 0.f, 0.f), Vector4(0.f, 0.f, 1.f, 0.f), 
		Vector4(CrossProduct(Vector3(0.f, -1.f, 0.f), Vector3(0.f, 0.f, 1.f)), 0.f));
	g_mesh->AddVertex(Vector3(-scale, -scale, scale), RGBA::WHITE, Vector2(1.f, 0.f),
		Vector4(0.f, -1.f, 0.f, 0.f), Vector4(0.f, 0.f, 1.f, 0.f), 
		Vector4(CrossProduct(Vector3(0.f, -1.f, 0.f), Vector3(0.f, 0.f, 1.f)), 0.f));
	g_mesh->AddVertex(Vector3(-scale, scale, scale), RGBA::WHITE, Vector2(0.f, 0.f),
		Vector4(0.f, -1.f, 0.f, 0.f), Vector4(0.f, 0.f, 1.f, 0.f), 
		Vector4(CrossProduct(Vector3(0.f, -1.f, 0.f), Vector3(0.f, 0.f, 1.f)), 0.f));

	uint32_t indices[] = { 2,1,0, 3,2,0, 4,5,6, 4,6,7, 8,9,10, 8,10,11, 12,13,14, 
		12,14,15, 16,17,18, 16,18,19, 20,21,22, 20,22,23 };


	g_mesh->CreateVertexBufferOnDevice();
	g_mesh->BindVertBufferToDeviceWindow();


	// Create index buffer
	uint numInds = ARRAYSIZE(indices);
	g_mesh->SetIndexBuffer(indices, numInds * sizeof(uint32_t), numInds);
	g_mesh->CreateIndexBufferOnDevice();
	g_mesh->BindIndBufferToDeviceWindow();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//SCENE INITIALIZATION
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//TICK
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------------
void TheGame::Update(float deltaSeconds) {

	m_playerCamera.Update(deltaSeconds);

	ScreenCoords cursorPos = InputSystem::GetCursorPosition();
	ScreenCoords screenMiddle = ScreenCoords(SCREEN_SIZE_X / 2, SCREEN_SIZE_Y / 2);
	InputSystem::SetCursorPosition(screenMiddle);
	ScreenCoords cursorDelta = cursorPos - screenMiddle;

	float pitch = m_playerCamera.m_pitchAboutX;

	if (pitch <= 89.f && cursorDelta.y > 0.f) {
		m_playerCamera.m_pitchAboutX += (float)cursorDelta.y * MOUSE_SENSITIVITY;
	}
	else if (pitch >= -89.f && cursorDelta.y < 0.f) {
		m_playerCamera.m_pitchAboutX += (float)cursorDelta.y * MOUSE_SENSITIVITY;
	}

	m_playerCamera.m_yawAboutY += (float)cursorDelta.x * MOUSE_SENSITIVITY;

	float moveSpeed = PLAYER_MOVE_SPEED * deltaSeconds;

	//FORWARD BACKWARD
	if (InputSystem::GetKey('W')) {
		m_playerCamera.m_position += m_playerCamera.GetForwardXZ() * moveSpeed; //Forward
	}
	else if (InputSystem::GetKey('S')) {
		m_playerCamera.m_position += m_playerCamera.GetForwardXZ() * -moveSpeed; //Backward
	}
	//LEFT RIGHT
	if (InputSystem::GetKey('A')) {
		m_playerCamera.m_position += m_playerCamera.GetLeftXZ() * -moveSpeed; //Left
	}
	else if (InputSystem::GetKey('D')) {
		m_playerCamera.m_position += m_playerCamera.GetLeftXZ() * moveSpeed; //Right
	}

	//UP DOWN
	if (InputSystem::GetKey(VK_SPACE)) {
		m_playerCamera.m_position += Vector3(0.f, moveSpeed, 0.f); //Up
	}
	else if (InputSystem::GetKey('C')) {
		m_playerCamera.m_position += Vector3(0.f, -moveSpeed, 0.f); //Down
	}
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//RENDER
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Vector3 pos = Vector3(0.f, 0.f, 0.f);
Vector3 pos2 = Vector3(10.f, 0.f, 0.f);

//---------------------------------------------------------------------------------------------------------------------------
void TheGame::Render() {

	ID3D11RenderTargetView* rt = RHIDeviceWindow::Get()->GetDefaultRenderTarget();

	GetDeviceContext()->OMSetRenderTargets(1, &rt, nullptr);

	// Setup the viewport
	D3D11_VIEWPORT vp;
	vp.Width = (FLOAT)3814;
	vp.Height = (FLOAT)2089;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	GetDeviceContext()->RSSetViewports(1, &vp);

	RHI::ClearRenderTarget(RGBA(0.1f, 0.1f, 0.1f, 1.f));

	BeirusEngine::Render();

	m_localModel.r[3] = XMVectorSet(pos.x, pos.y, pos.z, 1.f);
	m_World = XMMatrixTranspose(m_localModel);

	m_localModel2.r[3] = XMVectorSet(pos2.x, pos2.y, pos2.z, 1.f);
	m_World2 = XMMatrixTranspose(m_localModel2);

	g_meshRenderer.RenderMeshWithMaterial(g_mesh, brickMat);

	//UIRenderer::Get()->DrawAABB2(Vector2(200.f, 200.f), Vector2(200.f, 200.f), RGBA(0.f, 0.f, 0.f, 0.4f));

	//Font::DrawText2DWithDefaultFont(Vector2(100.f, 100.f), "Hello this is a test.", 1.f, RGBA::WHITE);

	RHIDeviceWindow::Get()->m_pSwapChain->Present(0, 0);
}