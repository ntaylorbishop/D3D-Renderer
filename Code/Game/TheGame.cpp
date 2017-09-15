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


D3D11VertexShader*		m_pVertexShader = nullptr;
D3D11PixelShader*		m_pPixelShader = nullptr;
ID3D11InputLayout*		m_pVertexLayout = nullptr;
ID3D11Buffer*			m_pVertexBuffer = nullptr;
ID3D11Buffer*			m_pIndexBuffer = nullptr;
ID3D11Buffer*			m_pConstantBuffer = nullptr;
XMMATRIX				m_World;
XMMATRIX				m_View;
XMMATRIX				m_Projection;

XMMATRIX				m_localModel;
XMMATRIX				m_localView;
XMMATRIX				m_localProjection;
D3D11ConstantBuffer*	m_cBuffer;
D3D11ConstantBuffer*	m_lightBuffer;
ID3D11SamplerState*		g_pSamplerLinear = nullptr;
ID3D11Debug*			g_debugDevice = nullptr;
D3D11SamplerState		g_samplerState;

D3D11ShaderProgram*		g_blinnPhongShader;
Light3D m_light;


Texture2D* m_texDiffuse = nullptr;
Texture2D* m_texNormal = nullptr;

UINT SCREEN_SIZE_X = 3840;
UINT SCREEN_SIZE_Y = 2160;
const float MOUSE_SENSITIVITY = 0.08f;
const float PLAYER_MOVE_SPEED = 20.f;



//---------------------------------------------------------------------------------------------------------------------------
TheGame::TheGame(HINSTANCE applicationInstanceHandle, int nCmdShow)
{
	RHIDeviceWindow::Initialize(applicationInstanceHandle, nCmdShow);

	D3D11Renderer::Get();

	InputSystem::HideMouseCursor();

	HRESULT hr = GetDevice()->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&g_debugDevice));
	ASSERT_OR_DIE(SUCCEEDED(hr), "ERROR: Could not query d3d11 debug device.");

	UINT width = (float)SCREEN_SIZE_X;
	UINT height = (float)SCREEN_SIZE_Y;

	m_playerCamera = Camera3D(Vector3(0.f, 1.f, -5.f), 0.f, 0.f, 0.f);
	m_playerCamera.SetPerspectiveProjection(90.f, width / height, 0.01f, 1000.f);

	CreateMesh();
	CreateShaderProgram();

	//Initialize MATs	
	m_localModel = XMMatrixIdentity();
}


//---------------------------------------------------------------------------------------------------------------------------
TheGame::~TheGame() {
	RHIDeviceWindow::Destroy();
}


//---------------------------------------------------------------------------------------------------------------------------
void TheGame::CreateShaderProgram() {

	m_texDiffuse = new Texture2D("Data/Textures/Brick2.png", true, TEXTURE_BIND_SHADER_RESOURCE, (eTextureCPUAccessFlags)0);
	m_texNormal = new Texture2D("Data/Textures/Brick_Normal.png", true, TEXTURE_BIND_SHADER_RESOURCE, (eTextureCPUAccessFlags)0);

	// Create the constant buffer
	m_cBuffer = D3D11ConstantBuffer::CreateOrGetConstantBuffer("ViewProj", sizeof(Matrix4) * 3);
	m_cBuffer->CreateBufferOnDevice();

	D3D11Uniform* modelUni	= new D3D11Uniform("Model", UNIFORM_MAT4, &m_World);
	D3D11Uniform* viewUni	= new D3D11Uniform("View",	UNIFORM_MAT4, &m_playerCamera.m_view);
	D3D11Uniform* projUni	= new D3D11Uniform("Proj",	UNIFORM_MAT4, &m_playerCamera.m_proj);
	m_cBuffer->AddUniform(modelUni);
	m_cBuffer->AddUniform(viewUni);
	m_cBuffer->AddUniform(projUni);

	//Create the light constant buffer
	m_lightBuffer = D3D11ConstantBuffer::CreateOrGetConstantBuffer("LightBuffer", 64);
	m_lightBuffer->CreateBufferOnDevice();

	D3D11Uniform* colUni		= new D3D11Uniform("LightColor",	UNIFORM_RGBA,		&m_light.m_color);
	D3D11Uniform* minLightDist	= new D3D11Uniform("MinLightDist",	UNIFORM_FLOAT,		&m_light.m_minLightDistance);
	D3D11Uniform* maxLightdist	= new D3D11Uniform("MaxLightDist",	UNIFORM_FLOAT,		&m_light.m_maxLightDistance);
	D3D11Uniform* minPower		= new D3D11Uniform("MinPower",		UNIFORM_FLOAT,		&m_light.m_powerAtMin);
	D3D11Uniform* maxPower		= new D3D11Uniform("MaxPower",		UNIFORM_FLOAT,		&m_light.m_powerAtMax);
	D3D11Uniform* posUni		= new D3D11Uniform("LightPosition", UNIFORM_VECTOR3,	&m_light.m_position);
	D3D11Uniform* camPosUni		= new D3D11Uniform("CameraPos",		UNIFORM_VECTOR3,	&m_playerCamera.m_position);

	m_light.m_position = Vector3(0.f, 10.f, 0.f);
	m_light.m_color = RGBA::WHITE;
	m_light.m_minLightDistance = 9.f;
	m_light.m_maxLightDistance = 11.f;
	m_light.m_powerAtMin = 1.f;
	m_light.m_powerAtMax = 0.f;

	m_lightBuffer->AddUniform(colUni);
	m_lightBuffer->AddUniform(minLightDist);
	m_lightBuffer->AddUniform(maxLightdist);
	m_lightBuffer->AddUniform(minPower);
	m_lightBuffer->AddUniform(maxPower);
	m_lightBuffer->AddUniform(posUni);
	m_lightBuffer->AddUniform(camPosUni);

	//CREATE SHADER PROGRAM
	m_pVertexShader = new D3D11VertexShader("Data/Shaders/SimpleTriangle.hlsl", D3D11SHADERTYPE_VERTEX);
	m_pPixelShader = new D3D11PixelShader("Data/Shaders/SimpleTriangle.hlsl", D3D11SHADERTYPE_FRAGMENT);

	m_pVertexShader->BindVertexLayoutToDeviceWindow(VERTEX_TYPE_PCTTBN);

	g_samplerState = D3D11SamplerState(D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP, D3D11_TEXTURE_ADDRESS_WRAP,
		D3D11_TEXTURE_ADDRESS_WRAP, D3D11_COMPARISON_NEVER);

	g_blinnPhongShader = D3D11ShaderProgram::CreateOrGetShaderProgram("BlinnPhong");

	g_blinnPhongShader->SetVertexShader(m_pVertexShader);
	g_blinnPhongShader->SetPixelShader(m_pPixelShader);

	D3D11Resource* texID = m_texDiffuse->GetSRVResource();
	D3D11Resource* normID = m_texNormal->GetSRVResource();

	g_blinnPhongShader->AddConstantBuffer(0, m_cBuffer, WHICH_SHADER_VERTEX);
	g_blinnPhongShader->AddConstantBuffer(1, m_lightBuffer, WHICH_SHADER_FRAGMENT);
	g_blinnPhongShader->AddResource(0, texID, WHICH_SHADER_FRAGMENT);
	g_blinnPhongShader->AddResource(1, normID, WHICH_SHADER_FRAGMENT);
	g_blinnPhongShader->AddSampler(0, &g_samplerState, WHICH_SHADER_FRAGMENT);
}



//---------------------------------------------------------------------------------------------------------------------------
void TheGame::CreateMesh() {

	// Create vertex buffer
	D3D11Mesh newMesh(VERTEX_TYPE_PCTTBN, 24);

	float scale = 1.f;

	//FRONT FACE
	newMesh.AddVertex(Vector3(-scale, scale, scale), RGBA::WHITE, Vector2(1.f, 0.f),
		Vector4(-1.f, 0.f, 0.f, 0.f), Vector4(0.f, 0.f, 1.f, 0.f), Vector4(CrossProduct(Vector3(-1.f, 0.f, 0.f), Vector3(0.f, 0.f, 1.f)), 0.f));
	newMesh.AddVertex(Vector3(-scale, scale, -scale), RGBA::WHITE, Vector2(1.f, 1.f),
		Vector4(-1.f, 0.f, 0.f, 0.f), Vector4(0.f, 0.f, 1.f, 0.f), Vector4(CrossProduct(Vector3(-1.f, 0.f, 0.f), Vector3(0.f, 0.f, 1.f)), 0.f));
	newMesh.AddVertex(Vector3(scale, scale, -scale), RGBA::WHITE, Vector2(0.f, 1.f),
		Vector4(-1.f, 0.f, 0.f, 0.f), Vector4(0.f, 0.f, 1.f, 0.f), Vector4(CrossProduct(Vector3(-1.f, 0.f, 0.f), Vector3(0.f, 0.f, 1.f)), 0.f));
	newMesh.AddVertex(Vector3(scale, scale, scale), RGBA::WHITE, Vector2(0.f, 0.f),
		Vector4(-1.f, 0.f, 0.f, 0.f), Vector4(0.f, 0.f, 1.f, 0.f), Vector4(CrossProduct(Vector3(-1.f, 0.f, 0.f), Vector3(0.f, 0.f, 1.f)), 0.f));

	//BACK FACE
	newMesh.AddVertex(Vector3(-scale, -scale, scale), RGBA::WHITE, Vector2(0.f, 0.f),
		Vector4(1.f, 0.f, 0.f, 0.f), Vector4(0.f, 0.f, 1.f, 0.f), Vector4(CrossProduct(Vector3(1.f, 0.f, 0.f), Vector3(0.f, 0.f, 1.f)), 0.f));
	newMesh.AddVertex(Vector3(-scale, -scale, -scale), RGBA::WHITE, Vector2(0.f, 1.f),
		Vector4(1.f, 0.f, 0.f, 0.f), Vector4(0.f, 0.f, 1.f, 0.f), Vector4(CrossProduct(Vector3(1.f, 0.f, 0.f), Vector3(0.f, 0.f, 1.f)), 0.f));
	newMesh.AddVertex(Vector3(scale, -scale, -scale), RGBA::WHITE, Vector2(1.f, 1.f),
		Vector4(1.f, 0.f, 0.f, 0.f), Vector4(0.f, 0.f, 1.f, 0.f), Vector4(CrossProduct(Vector3(1.f, 0.f, 0.f), Vector3(0.f, 0.f, 1.f)), 0.f));
	newMesh.AddVertex(Vector3(scale, -scale, scale), RGBA::WHITE, Vector2(1.f, 0.f),
		Vector4(1.f, 0.f, 0.f, 0.f), Vector4(0.f, 0.f, 1.f, 0.f), Vector4(CrossProduct(Vector3(1.f, 0.f, 0.f), Vector3(0.f, 0.f, 1.f)), 0.f));

	//BOTTOM FACE																					
	newMesh.AddVertex(Vector3(-scale, scale, -scale), RGBA::WHITE, Vector2(0.f, 1.f),
		Vector4(1.f, 0.f, 0.f, 0.f), Vector4(0.f, -1.f, 0.f, 0.f), Vector4(CrossProduct(Vector3(1.f, 0.f, 0.f), Vector3(0.f, -1.f, 0.f)), 0.f));
	newMesh.AddVertex(Vector3(scale, scale, -scale), RGBA::WHITE, Vector2(1.f, 1.f),
		Vector4(1.f, 0.f, 0.f, 0.f), Vector4(0.f, -1.f, 0.f, 0.f), Vector4(CrossProduct(Vector3(1.f, 0.f, 0.f), Vector3(0.f, -1.f, 0.f)), 0.f));
	newMesh.AddVertex(Vector3(scale, -scale, -scale), RGBA::WHITE, Vector2(1.f, 0.f),
		Vector4(1.f, 0.f, 0.f, 0.f), Vector4(0.f, -1.f, 0.f, 0.f), Vector4(CrossProduct(Vector3(1.f, 0.f, 0.f), Vector3(0.f, -1.f, 0.f)), 0.f));
	newMesh.AddVertex(Vector3(-scale, -scale, -scale), RGBA::WHITE, Vector2(0.f, 0.f),
		Vector4(1.f, 0.f, 0.f, 0.f), Vector4(0.f, -1.f, 0.f, 0.f), Vector4(CrossProduct(Vector3(1.f, 0.f, 0.f), Vector3(0.f, -1.f, 0.f)), 0.f));

	//TOP FACE																						
	newMesh.AddVertex(Vector3(-scale, scale, scale), RGBA::WHITE, Vector2(0.f, 0.f),
		Vector4(1.f, 0.f, 0.f, 0.f), Vector4(0.f, 1.f, 0.f, 0.f), Vector4(CrossProduct(Vector3(1.f, 0.f, 0.f), Vector3(0.f, 1.f, 0.f)), 0.f));
	newMesh.AddVertex(Vector3(-scale, -scale, scale), RGBA::WHITE, Vector2(0.f, 1.f),
		Vector4(1.f, 0.f, 0.f, 0.f), Vector4(0.f, 1.f, 0.f, 0.f), Vector4(CrossProduct(Vector3(1.f, 0.f, 0.f), Vector3(0.f, 1.f, 0.f)), 0.f));
	newMesh.AddVertex(Vector3(scale, -scale, scale), RGBA::WHITE, Vector2(1.f, 1.f),
		Vector4(1.f, 0.f, 0.f, 0.f), Vector4(0.f, 1.f, 0.f, 0.f), Vector4(CrossProduct(Vector3(1.f, 0.f, 0.f), Vector3(0.f, 1.f, 0.f)), 0.f));
	newMesh.AddVertex(Vector3(scale, scale, scale), RGBA::WHITE, Vector2(1.f, 0.f),
		Vector4(1.f, 0.f, 0.f, 0.f), Vector4(0.f, 1.f, 0.f, 0.f), Vector4(CrossProduct(Vector3(1.f, 0.f, 0.f), Vector3(0.f, 1.f, 0.f)), 0.f));

	//RIGHT FACE																					
	newMesh.AddVertex(Vector3(scale, scale, -scale), RGBA::WHITE, Vector2(1.f, 1.f),
		Vector4(0.f, 1.f, 0.f, 0.f), Vector4(0.f, 0.f, 1.f, 0.f), Vector4(CrossProduct(Vector3(0.f, 1.f, 0.f), Vector3(0.f, 0.f, 1.f)), 0.f));
	newMesh.AddVertex(Vector3(scale, scale, scale), RGBA::WHITE, Vector2(1.f, 0.f),
		Vector4(0.f, 1.f, 0.f, 0.f), Vector4(0.f, 0.f, 1.f, 0.f), Vector4(CrossProduct(Vector3(0.f, 1.f, 0.f), Vector3(0.f, 0.f, 1.f)), 0.f));
	newMesh.AddVertex(Vector3(scale, -scale, scale), RGBA::WHITE, Vector2(0.f, 0.f),
		Vector4(0.f, 1.f, 0.f, 0.f), Vector4(0.f, 0.f, 1.f, 0.f), Vector4(CrossProduct(Vector3(0.f, 1.f, 0.f), Vector3(0.f, 0.f, 1.f)), 0.f));
	newMesh.AddVertex(Vector3(scale, -scale, -scale), RGBA::WHITE, Vector2(0.f, 1.f),
		Vector4(0.f, 1.f, 0.f, 0.f), Vector4(0.f, 0.f, 1.f, 0.f), Vector4(CrossProduct(Vector3(0.f, 1.f, 0.f), Vector3(0.f, 0.f, 1.f)), 0.f));

	//LEFT FACE																						
	newMesh.AddVertex(Vector3(-scale, scale, -scale), RGBA::WHITE, Vector2(0.f, 1.f),
		Vector4(0.f, -1.f, 0.f, 0.f), Vector4(0.f, 0.f, 1.f, 0.f), Vector4(CrossProduct(Vector3(0.f, -1.f, 0.f), Vector3(0.f, 0.f, 1.f)), 0.f));
	newMesh.AddVertex(Vector3(-scale, -scale, -scale), RGBA::WHITE, Vector2(1.f, 1.f),
		Vector4(0.f, -1.f, 0.f, 0.f), Vector4(0.f, 0.f, 1.f, 0.f), Vector4(CrossProduct(Vector3(0.f, -1.f, 0.f), Vector3(0.f, 0.f, 1.f)), 0.f));
	newMesh.AddVertex(Vector3(-scale, -scale, scale), RGBA::WHITE, Vector2(1.f, 0.f),
		Vector4(0.f, -1.f, 0.f, 0.f), Vector4(0.f, 0.f, 1.f, 0.f), Vector4(CrossProduct(Vector3(0.f, -1.f, 0.f), Vector3(0.f, 0.f, 1.f)), 0.f));
	newMesh.AddVertex(Vector3(-scale, scale, scale), RGBA::WHITE, Vector2(0.f, 0.f),
		Vector4(0.f, -1.f, 0.f, 0.f), Vector4(0.f, 0.f, 1.f, 0.f), Vector4(CrossProduct(Vector3(0.f, -1.f, 0.f), Vector3(0.f, 0.f, 1.f)), 0.f));

	uint32_t indices[] = { 2,1,0, 3,2,0, 4,5,6, 4,6,7, 8,9,10, 8,10,11, 12,13,14, 12,14,15, 16,17,18, 16,18,19, 20,21,22, 20,22,23 };


	newMesh.CreateVertexBufferOnDevice();
	newMesh.BindVertBufferToDeviceWindow();

	// Create index buffer
	newMesh.SetIndexBuffer(indices, ARRAYSIZE(indices) * sizeof(uint32_t), 36);
	newMesh.CreateIndexBufferOnDevice();
	newMesh.BindIndBufferToDeviceWindow();

	// Set primitive topology
	RHIDeviceWindow::Get()->m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
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

//---------------------------------------------------------------------------------------------------------------------------
void TheGame::Render() {

	m_localModel.r[3] = XMVectorSet(pos.x, pos.y, pos.z, 1.f);

	RHI::ClearRenderTarget(RGBA(0.1f, 0.1f, 0.1f, 1.f));

	m_World = XMMatrixTranspose(m_localModel);

	m_cBuffer->UpdateBufferOnDevice();
	m_lightBuffer->UpdateBufferOnDevice();

	g_blinnPhongShader->Use();

	RHIDeviceWindow::Get()->m_pDeviceContext->DrawIndexed(36, 0, 0);        // 6 vertices needed for 12 triangles in a triangle list
	RHIDeviceWindow::Get()->m_pSwapChain->Present(0, 0);
}