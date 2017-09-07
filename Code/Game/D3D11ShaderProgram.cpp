#include "Game/D3D11ShaderProgram.hpp"


//---------------------------------------------------------------------------------------------------------------------------
void D3D11ShaderProgram::Use() {

	GetDeviceContext()->VSSetShader(m_pVertexShader->GetShaderHandle(), nullptr, 0);

	for (size_t i = 0; i < m_constBuffers.size(); i++) {

		ID3D11Buffer* pConstBufferHandle = m_constBuffers[i].m_pConstBuffer->GetDeviceBufferHandle();


	}

	here,

	//ID3D11Buffer* pConstBufferHandle = m_cBuffer->GetDeviceBufferHandle();
	//ID3D11Buffer* pLightBufferHandle = m_lightBuffer->GetDeviceBufferHandle();
	//ID3D11SamplerState* pSamplerStateHandle = g_samplerState.GetSamplerHandle();
	//
	//GetDeviceContext()->VSSetShader(m_pVertexShader->GetShaderHandle(), nullptr, 0);
	//GetDeviceContext()->VSSetConstantBuffers(0, 1, &pConstBufferHandle);
	//GetDeviceContext()->PSSetShader(m_pPixelShader->GetShaderHandle(), nullptr, 0);
	//GetDeviceContext()->PSSetShaderResources(0, 1, &texID);
	//GetDeviceContext()->PSSetShaderResources(1, 1, &normID);
	//GetDeviceContext()->PSSetConstantBuffers(1, 1, &pLightBufferHandle);
	//GetDeviceContext()->PSSetSamplers(0, 1, &pSamplerStateHandle);
}