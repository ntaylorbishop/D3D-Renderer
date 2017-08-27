//--------------------------------------------------------------------------------------
// File: Tutorial04.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
cbuffer ConstantBuffer : register(b0) {
	matrix uModel;
	matrix uView;
	matrix uProjection;
}

//--------------------------------------------------------------------------------------
struct VS_OUTPUT {
	float4 Pos : SV_POSITION;
	float4 Color : COLOR0;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS(float4 Pos : POSITION, float4 Color : COLOR) {

	VS_OUTPUT output = (VS_OUTPUT)0;
	output.Pos = mul(Pos, uModel);
	output.Pos = mul(output.Pos, uView);
	output.Pos = mul(output.Pos, uProjection);
	output.Color = Color;
	return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(VS_OUTPUT input) : SV_Target{
	return input.Color;
}