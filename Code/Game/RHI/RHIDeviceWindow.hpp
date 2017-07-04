#pragma once

#include "Engine/General/Core/EngineCommon.hpp"
#include <windows.h>
#include <d3d11_1.h>
#include <directxcolors.h>

class RHIDeviceWindow {
public:
	static void Initialize(HINSTANCE applicationInstanceHandle, int nCmdShow);
	static void Destroy();

	static RHIDeviceWindow* GetDeviceWindow() { return s_deviceWindow; }

	static void ClearAndPresent();

	void GetD3DDevice() { return m_pd3dDevice; }

private:
	//STRUCTORS
	RHIDeviceWindow(HINSTANCE applicationInstanceHandle, int nCmdShow);
	~RHIDeviceWindow();

	//INIT DESTROY
	void InitWindow(HINSTANCE applicationInstanceHandle, int nCmdShow);
	void CreateDevice();
	void CleanupDeviceAndWindow();

	HINSTANCE               m_hInst					= nullptr;
	HWND                    m_hWnd					= nullptr;
	D3D_DRIVER_TYPE         m_driverType			= D3D_DRIVER_TYPE_NULL;
	D3D_FEATURE_LEVEL       m_featureLevel			= D3D_FEATURE_LEVEL_11_0;
	ID3D11Device*           m_pd3dDevice			= nullptr;
	ID3D11Device1*          m_pd3dDevice1			= nullptr;
	ID3D11DeviceContext*    m_pImmediateContext		= nullptr;
	ID3D11DeviceContext1*   m_pImmediateContext1	= nullptr;
	IDXGISwapChain*         m_pSwapChain			= nullptr;
	IDXGISwapChain1*        m_pSwapChain1			= nullptr;
	ID3D11RenderTargetView* m_pRenderTargetView		= nullptr;
	ID3D11VertexShader*     m_pVertexShader			= nullptr;
	ID3D11PixelShader*      m_pPixelShader			= nullptr;
	ID3D11InputLayout*      m_pVertexLayout			= nullptr;
	ID3D11Buffer*           m_pVertexBuffer			= nullptr;

	static RHIDeviceWindow* s_deviceWindow;
};							