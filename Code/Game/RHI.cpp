#include "Game/RHI.hpp"
#include "Game/RHI/RHIDeviceWindow.hpp"


//---------------------------------------------------------------------------------------------------------------------------
void RHI::ClearRenderTarget(const RGBA& clearColor) {

	float col[4] = { clearColor.r, clearColor.g, clearColor.b, clearColor.a };
	GetDeviceContext()->ClearRenderTargetView(RHIDeviceWindow::Get()->m_pRenderTargetView, col);

}
