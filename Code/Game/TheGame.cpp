#include "Game/TheGame.hpp"
#include "Game/TheApp.hpp"
#include "Game/RHI/RHIDeviceWindow.hpp"
#include "Game/RHI/D3D11VertexShader.hpp"
#include "Game/RHI/D3D11PixelShader.hpp"
#include "Game/RHI/D3D11Mesh.hpp"
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


D3D11VertexShader*	m_pVertexShader		= nullptr;
D3D11PixelShader*	m_pPixelShader		= nullptr;
ID3D11InputLayout*	m_pVertexLayout		= nullptr;
ID3D11Buffer*       m_pVertexBuffer		= nullptr;
ID3D11Buffer*       m_pIndexBuffer		= nullptr;
ID3D11Buffer*       m_pConstantBuffer	= nullptr;
XMMATRIX            m_World;
XMMATRIX            m_View;
XMMATRIX            m_Projection;



//---------------------------------------------------------------------------------------------------------------------------
TheGame::TheGame(HINSTANCE applicationInstanceHandle, int nCmdShow)
{
	RHIDeviceWindow::Initialize(applicationInstanceHandle, nCmdShow);

	HRESULT hr;

	UINT width = 800;
	UINT height = 600;

	m_pVertexShader = new D3D11VertexShader("Data/Shaders/SimpleTriangle.hlsl", D3D11SHADERTYPE_VERTEX);
	m_pPixelShader = new D3D11PixelShader("Data/Shaders/SimpleTriangle.hlsl", D3D11SHADERTYPE_FRAGMENT);

	m_pVertexShader->BindVertexLayoutToDeviceWindow(VERTEX_TYPE_PC);

	// Create vertex buffer
	D3D11Mesh newMesh(VERTEX_TYPE_PC, 8);

	newMesh.AddVertex(Vector3(-1.0f,  1.0f, -1.0f), RGBA(0.0f, 0.0f, 1.0f, 1.0f));
	newMesh.AddVertex(Vector3( 1.0f,  1.0f, -1.0f), RGBA(0.0f, 1.0f, 0.0f, 1.0f));
	newMesh.AddVertex(Vector3( 1.0f,  1.0f,  1.0f), RGBA(0.0f, 1.0f, 1.0f, 1.0f));
	newMesh.AddVertex(Vector3(-1.0f,  1.0f,  1.0f), RGBA(1.0f, 0.0f, 0.0f, 1.0f));
	newMesh.AddVertex(Vector3(-1.0f, -1.0f, -1.0f), RGBA(1.0f, 0.0f, 1.0f, 1.0f));
	newMesh.AddVertex(Vector3( 1.0f, -1.0f, -1.0f), RGBA(1.0f, 1.0f, 0.0f, 1.0f));
	newMesh.AddVertex(Vector3( 1.0f, -1.0f,  1.0f), RGBA(1.0f, 1.0f, 1.0f, 1.0f));
	newMesh.AddVertex(Vector3(-1.0f, -1.0f,  1.0f), RGBA(0.0f, 0.0f, 0.0f, 1.0f));

	newMesh.CreateVertexBufferOnDevice();
	newMesh.BindVertBufferToDeviceWindow();

	// Create index buffer
	uint32_t  indices[] = { 3,1,0, 2,1,3, 0,5,4, 1,5,0, 3,4,7, 0,4,3, 1,6,5, 2,6,1, 2,7,6, 3,7,2, 6,4,5, 7,4,6 };
	newMesh.SetIndexBuffer(indices, ARRAYSIZE(indices) * sizeof(uint32_t), 36);
	newMesh.CreateIndexBufferOnDevice();
	newMesh.BindIndBufferToDeviceWindow();


	// Set primitive topology
	RHIDeviceWindow::Get()->m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Create the constant buffer
	//D3D11_BUFFER_DESC bd;
	//ZeroMemory(&bd, sizeof(bd));
	newMesh.m_bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	newMesh.m_bufferDesc.ByteWidth = sizeof(ConstantBuffer);
	newMesh.m_bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	newMesh.m_bufferDesc.CPUAccessFlags = 0;
	hr = RHIDeviceWindow::Get()->m_pd3dDevice->CreateBuffer(&newMesh.m_bufferDesc, nullptr, &m_pConstantBuffer);
	if (FAILED(hr))
		ERROR_AND_DIE("HR FAILED");

	// Initialize the world matrix
	m_World = XMMatrixIdentity();

	// Initialize the view matrix
	XMVECTOR Eye = XMVectorSet(0.0f, 1.0f, -5.0f, 0.0f);
	XMVECTOR At = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	m_View = XMMatrixLookAtLH(Eye, At, Up);

	// Initialize the projection matrix
	m_Projection = XMMatrixPerspectiveFovLH(XM_PIDIV2, width / (FLOAT)height, 0.01f, 100.0f);
}


//---------------------------------------------------------------------------------------------------------------------------
TheGame::~TheGame() {
	RHIDeviceWindow::Destroy();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//SCENE INITIALIZATION
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//TICK
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------------
void TheGame::Update(float deltaSeconds) {

}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//RENDER
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------------
STATIC void TheGame::Render() {

	// Update our time
	static float t = 0.0f;
	static ULONGLONG timeStart = 0;
	ULONGLONG timeCur = GetTickCount64();
	if (timeStart == 0)
		timeStart = timeCur;
	t = (timeCur - timeStart) / 1000.0f;

	//
	// Animate the cube
	//
	m_World = XMMatrixRotationY(t);

	//
	// Clear the back buffer
	//
	RHIDeviceWindow::Get()->m_pImmediateContext->ClearRenderTargetView(RHIDeviceWindow::Get()->m_pRenderTargetView, Colors::MidnightBlue);

	//
	// Update variables
	//
	ConstantBuffer cb;
	cb.mWorld = XMMatrixTranspose(m_World);
	cb.mView = XMMatrixTranspose(m_View);
	cb.mProjection = XMMatrixTranspose(m_Projection);
	RHIDeviceWindow::Get()->m_pImmediateContext->UpdateSubresource(m_pConstantBuffer, 0, nullptr, &cb, 0, 0);

	//
	// Renders a triangle
	//
	RHIDeviceWindow::Get()->m_pImmediateContext->VSSetShader(m_pVertexShader->GetShaderHandle(), nullptr, 0);
	RHIDeviceWindow::Get()->m_pImmediateContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
	RHIDeviceWindow::Get()->m_pImmediateContext->PSSetShader(m_pPixelShader->GetShaderHandle(), nullptr, 0);
	RHIDeviceWindow::Get()->m_pImmediateContext->DrawIndexed(36, 0, 0);        // 36 vertices needed for 12 triangles in a triangle list

													   //
													   // Present our back buffer to our front buffer
													   //
	RHIDeviceWindow::Get()->m_pSwapChain->Present(0, 0);
}

