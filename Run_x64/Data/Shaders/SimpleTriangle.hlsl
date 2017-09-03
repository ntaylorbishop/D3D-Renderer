

Texture2D txDiffuse : register(t0);
Texture2D txNormal : register(t1);

SamplerState samLinear : register(s0);

struct Light3D {
	float4	color;
	float	minLightDistangentce;
	float	maxLightDistangentce;
	float	powerAtMin;
	float	powerAtMax;
	float3	position;
};


//---------------------------------------------------------------------------------------------------------------------------
struct VS_OUTPUT {
	float4 pos : SV_POSITION;
	float4 color : COLOR0;
	float2 tex : TEXCOORD0;
	float3 tan : TANGENT0;
	float3 bitan : BINORMAL0;
	float3 norm : NORMAL0;
};


//---------------------------------------------------------------------------------------------------------------------------
cbuffer ConstangenttBuffer : register(b0) {
	matrix uModel;
	matrix uView;
	matrix uProjection;
}


//---------------------------------------------------------------------------------------------------------------------------
cbuffer LightConstBuffer : register(b1) {
	float4	color;
	float	minLightDistangentce;
	float	maxLightDistangentce;
	float	powerAtMin;
	float	powerAtMax;
	float3	position;
}


//---------------------------------------------------------------------------------------------------------------------------
float3 GetSurfaceNormal(float3 passTan, float3 passBitan, float3 passNormal, float4 txNormal) {

	float3 tan = float3(passTan.x, passTan.y, passTan.z);
	float3 bitan = float3(passBitan.x, passBitan.y, passBitan.z);
	float3x3 tbn = float3x3(tan, bitan, float3(passNormal.x, passNormal.y, passNormal.z));
	tbn = transpose(tbn);

	float3 normal = float3(txNormal.x, txNormal.y, txNormal.z);
	normal = normal * float3(2.f, 2.f, 1.f) - float3(1.f, 1.f, 0.f);

	return mul(normal, tbn);
}

//---------------------------------------------------------------------------------------------------------------------------
VS_OUTPUT VS(float3 pos : POSITION, float4 color : COLOR, float2 tex : TEXCOORD, float4 tan : TANGENT, float4 bitan : BINORMAL, float4 norm : NORMAL) {

	VS_OUTPUT output = (VS_OUTPUT)0;

	output.pos = mul(float4(pos.x, pos.y, pos.z, 1.f), uModel);
	output.pos = mul(output.pos, uView);
	output.pos = mul(output.pos, uProjection);

	output.color = color;
	output.tex = tex;

	output.tan = float3(tan.x, tan.y, tan.z);
	output.bitan = float3(bitan.x, bitan.y, bitan.z);
	output.norm = float3(norm.x, norm.y, norm.z);

	return output;
}


//---------------------------------------------------------------------------------------------------------------------------
float4 PS(VS_OUTPUT input) : SV_Target {

	float4 texNorm = txNormal.Sample(samLinear, input.tex);
	float3 surfaceNormal = GetSurfaceNormal(input.tan, input.bitan, input.norm, texNorm);

	return float4(surfaceNormal.x, surfaceNormal.y, surfaceNormal.z, 1.f);
	//return txNormal.Sample(samLinear, input.tex);
}