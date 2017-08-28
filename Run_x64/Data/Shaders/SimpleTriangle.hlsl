//--------------------------------------------------------------------------------------
// File: Tutorial04.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------


Texture2D txDiffuse : register(t0);
SamplerState samLinear : register(s0);

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
	float2 Tex : TEXCOORD0;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS(float4 Pos : POSITION, float4 Color : COLOR, float2 Tex : TEXCOORD) {

	VS_OUTPUT output = (VS_OUTPUT)0;
	output.Pos = mul(Pos, uModel);
	output.Pos = mul(output.Pos, uView);
	output.Pos = mul(output.Pos, uProjection);
	output.Color = Color;
	output.Tex = Tex;
	return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(VS_OUTPUT input) : SV_Target {
	return txDiffuse.Sample(samLinear, input.Tex);
}