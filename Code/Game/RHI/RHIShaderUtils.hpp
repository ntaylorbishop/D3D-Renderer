#pragma once

#include "Game/RHI/D3D11Shader.hpp"


ID3DBlob* CompileShaderFromFile(const char* fileName, const char* entryPoint, const char* shaderModel);