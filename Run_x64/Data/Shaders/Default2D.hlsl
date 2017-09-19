Texture2D txDiffuse : register(t0);
SamplerState samLinear : register(s0);


//---------------------------------------------------------------------------------------------------------------------------
struct VS_OUTPUT {
	float4 pos : SV_POSITION;
	float4 color : COLOR0;
	float2 tex : TEXCOORD0;
};


//---------------------------------------------------------------------------------------------------------------------------
cbuffer ViewProjection2D : register(b0) {
	matrix uOrtho;
}


//---------------------------------------------------------------------------------------------------------------------------
VS_OUTPUT VS(float3 pos : POSITION, float4 color : COLOR, float2 tex : TEXCOORD) {

	VS_OUTPUT output = (VS_OUTPUT)0;

	output.pos		= float4(pos.x, pos.y, pos.z, 1.f);
	output.pos		= mul(output.pos, uOrtho);
	output.color	= color;
	output.tex		= tex;

	return output;
}


//---------------------------------------------------------------------------------------------------------------------------
float4 PS(VS_OUTPUT input) : SV_Target {

	float4 texDiffuse = txDiffuse.Sample(samLinear, input.tex);
	return texDiffuse * input.color;
}