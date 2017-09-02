//--------------------------------------------------------------------------------------
// File: Tutorial04.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------


Texture2D txDiffuse : register(t0); 
Texture2D txNormal : register(t1);

SamplerState samLinear : register(s0);

struct Light3D {
	float4	color;
	float	minLightDistance;
	float	maxLightDistance;
	float	powerAtMin;
	float	powerAtMax;
	float3	position;
};

cbuffer LightConstBuffer : register(b1) {
	float4	color;
	float	minLightDistance;
	float	maxLightDistance;
	float	powerAtMin;
	float	powerAtMax;
	float3	position;
}

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
	float3 Tan : TANGENT0;
	float3 Bitan : BINORMAL0;
	float3 Norm : NORMAL0;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS(float4 Pos : POSITION, float4 Color : COLOR, float2 Tex : TEXCOORD, float3 Tan : TANGENT, float3 Bitan : BINORMAL, float3 Norm : NORMAL) {

	VS_OUTPUT output = (VS_OUTPUT)0;
	output.Pos = mul(Pos, uModel);
	output.Pos = mul(output.Pos, uView);
	output.Pos = mul(output.Pos, uProjection);
	output.Color = Color;
	output.Tex = Tex;
	output.Tan = Tan;
	output.Bitan = Bitan;
	output.Norm = Norm;
	return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(VS_OUTPUT input) : SV_Target {

	return float4(input.Norm.x, input.Norm.y, input.Norm.z, 1.f);
	//return txNormal.Sample(samLinear, input.Tex);
}