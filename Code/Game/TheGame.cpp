#include "Game/TheGame.hpp"
#include "Game/TheApp.hpp"
#include "Game/RHI/RHIDeviceWindow.hpp"
#include "Game/RHI/D3D11VertexShader.hpp"
#include "Game/RHI/D3D11PixelShader.hpp"
#include "Game/RHI/D3D11Mesh.hpp"
#include "Game/RHI/D3D11ConstantBuffer.hpp"
#include "Game/RHI/Texture2D.hpp"
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

XMMATRIX            m_localModel;
XMMATRIX            m_localView;
XMMATRIX            m_localProjection;
D3D11ConstantBuffer* m_cBuffer;
ID3D11SamplerState*  g_pSamplerLinear = nullptr;


Texture2D* m_texDiffuse = nullptr;

UINT SCREEN_SIZE_X = 3840;
UINT SCREEN_SIZE_Y = 2160;
const float MOUSE_SENSITIVITY = 0.08f;
const float PLAYER_MOVE_SPEED = 20.f;



//---------------------------------------------------------------------------------------------------------------------------
TheGame::TheGame(HINSTANCE applicationInstanceHandle, int nCmdShow)
{
    RHIDeviceWindow::Initialize(applicationInstanceHandle, nCmdShow);

    HRESULT hr;

    UINT width = SCREEN_SIZE_X;
    UINT height = SCREEN_SIZE_Y;

    m_playerCamera = Camera3D(Vector3(0.f, 1.f, -5.f), 0.f, 0.f, 0.f);
    m_playerCamera.SetPerspectiveProjection(90.f, width / height, 0.01f, 1000.f);

    m_pVertexShader = new D3D11VertexShader("Data/Shaders/SimpleTriangle.hlsl", D3D11SHADERTYPE_VERTEX);
    m_pPixelShader = new D3D11PixelShader("Data/Shaders/SimpleTriangle.hlsl", D3D11SHADERTYPE_FRAGMENT);

    m_pVertexShader->BindVertexLayoutToDeviceWindow(VERTEX_TYPE_PCT);

    // Create vertex buffer
    D3D11Mesh newMesh(VERTEX_TYPE_PCT, 8);

    newMesh.AddVertex(Vector3(-1.0f,  1.0f, -1.0f), RGBA(0.0f, 0.0f, 1.0f, 1.0f), Vector2(0.f, 0.f));
    newMesh.AddVertex(Vector3( 1.0f,  1.0f, -1.0f), RGBA(0.0f, 1.0f, 0.0f, 1.0f), Vector2(0.f, 1.f));
    newMesh.AddVertex(Vector3( 1.0f,  1.0f,  1.0f), RGBA(0.0f, 1.0f, 1.0f, 1.0f), Vector2(1.f, 0.f));
    newMesh.AddVertex(Vector3(-1.0f,  1.0f,  1.0f), RGBA(1.0f, 0.0f, 0.0f, 1.0f), Vector2(1.f, 1.f));
    newMesh.AddVertex(Vector3(-1.0f, -1.0f, -1.0f), RGBA(1.0f, 0.0f, 1.0f, 1.0f), Vector2(0.f, 0.f));
    newMesh.AddVertex(Vector3( 1.0f, -1.0f, -1.0f), RGBA(1.0f, 1.0f, 0.0f, 1.0f), Vector2(0.f, 1.f));
    newMesh.AddVertex(Vector3( 1.0f, -1.0f,  1.0f), RGBA(1.0f, 1.0f, 1.0f, 1.0f), Vector2(1.f, 0.f));
    newMesh.AddVertex(Vector3(-1.0f, -1.0f,  1.0f), RGBA(0.0f, 0.0f, 0.0f, 1.0f), Vector2(1.f, 1.f));

    newMesh.CreateVertexBufferOnDevice();
    newMesh.BindVertBufferToDeviceWindow();

    // Create index buffer
    uint32_t  indices[] = { 3,1,0, 2,1,3, 0,5,4, 1,5,0, 3,4,7, 0,4,3, 1,6,5, 2,6,1, 2,7,6, 3,7,2, 6,4,5, 7,4,6 };
    newMesh.SetIndexBuffer(indices, ARRAYSIZE(indices) * sizeof(uint32_t), 36);
    newMesh.CreateIndexBufferOnDevice();
    newMesh.BindIndBufferToDeviceWindow();

    // Set primitive topology
    RHIDeviceWindow::Get()->m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    //Initialize MATs	
    m_localModel = XMMatrixIdentity();

	m_texDiffuse = new Texture2D("Data/Textures/Brick.png", true, TEXTURE_BIND_SHADER_RESOURCE, (eTextureCPUAccessFlags)0);

	// Create the sample state
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = GetDevice()->CreateSamplerState(&sampDesc, &g_pSamplerLinear);
	if (FAILED(hr)) {
		int a = 0;
	}

    // Create the constant buffer
    m_cBuffer = new D3D11ConstantBuffer(sizeof(Matrix4) * 3);
    m_cBuffer->CreateBufferOnDevice();

    D3D11Uniform* modelUni		= new D3D11Uniform("Model", UNIFORM_MAT4, 0, 0, &m_World);
    D3D11Uniform* viewUni		= new D3D11Uniform("View", UNIFORM_MAT4, 0, 0, &m_playerCamera.m_view);
    D3D11Uniform* projUni		= new D3D11Uniform("Proj", UNIFORM_MAT4, 0, 0, &m_playerCamera.m_proj);
    m_cBuffer->AddUniform(modelUni);
    m_cBuffer->AddUniform(viewUni);
    m_cBuffer->AddUniform(projUni);
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

//---------------------------------------------------------------------------------------------------------------------------
void TheGame::Render() {

    // Update our time
    static float t = 0.0f;
    static ULONGLONG timeStart = 0;
    ULONGLONG timeCur = GetTickCount64();
    if (timeStart == 0)
        timeStart = timeCur;
    t = (timeCur - timeStart) / 1000.0f;
    m_localModel = XMMatrixRotationY(t);

	float col[4] = { 0.1f, 0.1f, 0.1f, 1.f };
	RHIDeviceWindow::Get()->m_pDeviceContext->ClearRenderTargetView(RHIDeviceWindow::Get()->m_pRenderTargetView, col);

    m_World			= XMMatrixTranspose(m_localModel);
    m_View			= XMMatrixTranspose(m_localView);
    m_Projection	= XMMatrixTranspose(m_localProjection);

    m_cBuffer->UpdateBufferOnDevice();

	ID3D11ShaderResourceView* texID = m_texDiffuse->GetSRV();

    ID3D11Buffer* pConstBufferHandle = m_cBuffer->GetDeviceBufferHandle();
    RHIDeviceWindow::Get()->m_pDeviceContext->VSSetShader(m_pVertexShader->GetShaderHandle(), nullptr, 0);
    RHIDeviceWindow::Get()->m_pDeviceContext->VSSetConstantBuffers(0, 1, &pConstBufferHandle);
    RHIDeviceWindow::Get()->m_pDeviceContext->PSSetShader(m_pPixelShader->GetShaderHandle(), nullptr, 0);
	RHIDeviceWindow::Get()->m_pDeviceContext->PSSetShaderResources(0, 1, &texID);
	RHIDeviceWindow::Get()->m_pDeviceContext->PSSetSamplers(0, 1, &g_pSamplerLinear);


    RHIDeviceWindow::Get()->m_pDeviceContext->DrawIndexed(36, 0, 0);        // 36 vertices needed for 12 triangles in a triangle list
    RHIDeviceWindow::Get()->m_pSwapChain->Present(0, 0);
}











/*
        case D3D11_RESOURCE_DIMENSION_TEXTURE2D:
            {
                D3D11_TEXTURE2D_DESC desc;
                desc.Width = static_cast<UINT>( width );
                desc.Height = static_cast<UINT>( height );
                desc.MipLevels = static_cast<UINT>( mipCount );
                desc.ArraySize = static_cast<UINT>( arraySize );
                desc.Format = format;
                desc.SampleDesc.Count = 1;
                desc.SampleDesc.Quality = 0;
                desc.Usage = usage;
                desc.BindFlags = bindFlags;
                desc.CPUAccessFlags = cpuAccessFlags;
                if ( isCubeMap )
                {
                    desc.MiscFlags = miscFlags | D3D11_RESOURCE_MISC_TEXTURECUBE;
                }
                else
                {
                    desc.MiscFlags = miscFlags & ~D3D11_RESOURCE_MISC_TEXTURECUBE;
                }

                ID3D11Texture2D* tex = nullptr;
                hr = d3dDevice->CreateTexture2D( &desc,
                                                 initData,
                                                 &tex
                                               );
                if (SUCCEEDED( hr ) && tex != 0)
                {
                    if (textureView != 0)
                    {
                        D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
                        memset( &SRVDesc, 0, sizeof( SRVDesc ) );
                        SRVDesc.Format = format;

                        if ( isCubeMap )
                        {
                            if (arraySize > 6)
                            {
                                SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBEARRAY;
                                SRVDesc.TextureCubeArray.MipLevels = (!mipCount) ? -1 : desc.MipLevels;

                                // Earlier we set arraySize to (NumCubes * 6)
                                SRVDesc.TextureCubeArray.NumCubes = static_cast<UINT>( arraySize / 6 );
                            }
                            else
                            {
                                SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
                                SRVDesc.TextureCube.MipLevels = (!mipCount) ? -1 : desc.MipLevels;
                            }
                        }
                        else if (arraySize > 1)
                        {
                            SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
                            SRVDesc.Texture2DArray.MipLevels = (!mipCount) ? -1 : desc.MipLevels;
                            SRVDesc.Texture2DArray.ArraySize = static_cast<UINT>( arraySize );
                        }
                        else
                        {
                            SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
                            SRVDesc.Texture2D.MipLevels = (!mipCount) ? -1 : desc.MipLevels;
                        }

                        hr = d3dDevice->CreateShaderResourceView( tex,
                                                                  &SRVDesc,
                                                                  textureView
                                                                );
                        if ( FAILED(hr) )
                        {
                            tex->Release();
                            return hr;
                        }
                    }

                    if (texture != 0)
                    {
                        *texture = tex;
                    }
                    else
                    {
                        SetDebugObjectName(tex, "DDSTextureLoader");
                        tex->Release();
                    }
                }
            }
            break;

*/