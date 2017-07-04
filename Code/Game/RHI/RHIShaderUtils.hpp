#pragma once

#include "Game/RHI/D3D11Shader.hpp"


void CompileShaderFromFile(const char* fileName, const char* entryPoint, const char* shaderModel, ID3DBlob** ppBlobOut);