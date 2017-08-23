#include "Game/TheGame.hpp"
#include "Game/TheApp.hpp"
#include "Game/RHI/RHIDeviceWindow.hpp"
#include "Game/RHI/D3D11VertexShader.hpp"
#include "Game/RHI/D3D11PixelShader.hpp"
#include "Game/RHI/D3D11Mesh.hpp"
#include "Game/RHI/D3D11ConstantBuffer.hpp"
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
D3D11ConstantBuffer m_cBuffer;



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

	//Initialize MATs	
	m_World = XMMatrixIdentity();

	XMVECTOR Eye	= XMVectorSet(0.0f, 1.0f, -5.0f, 0.0f);
	XMVECTOR At		= XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR Up		= XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	m_View			= XMMatrixLookAtLH(Eye, At, Up);

	m_Projection = XMMatrixPerspectiveFovLH(XM_PIDIV2, width / (FLOAT)height, 0.01f, 100.0f);

	// Create the constant buffer
	m_cBuffer.CreateBufferOnDevice();

	Uniform modelUni	= new Uniform("Model", UNIFORM_MAT4, 0, 0, &m_World);
	Uniform viewUni		= new Uniform("View", UNIFORM_MAT4, 0, 0, &m_View);
	Uniform projUni		= new Uniform("Proj", UNIFORM_MAT4, 0, 0, &m_Projection);
	m_cBuffer.AddUniform(modelUni);
	m_cBuffer.AddUniform(viewUni);
	m_cBuffer.AddUniform(projUni);
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