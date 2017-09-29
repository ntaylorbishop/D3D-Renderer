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

cbuffer Model2D : register(b1) {
	matrix uModel;
}

cbuffer TexCoordsAndTint : register (b2) {
	float4 tint;
	float4 texCoordsQuad;
}


//---------------------------------------------------------------------------------------------------------------------------
VS_OUTPUT VS(float3 pos : POSITION, float4 color : COLOR, float2 tex : TEXCOORD) {

	VS_OUTPUT output = (VS_OUTPUT)0;

	output.pos		= mul(float4(pos.x, pos.y, pos.z, 1.f), uModel);
	output.pos		= mul(output.pos, uOrtho);
	output.color	= color;
	output.tex		= tex;

	return output;
}


//---------------------------------------------------------------------------------------------------------------------------
float4 PS(VS_OUTPUT input) : SV_Target {

	//Note that the quad is encoded as such: float4(mins.x, mins.y, maxs.x, maxs.y)
	float2 texCoords;
	texCoords.x = (texCoordsQuad.z - texCoordsQuad.x) * input.tex.x;
	texCoords.y = (texCoordsQuad.w - texCoordsQuad.y) * input.tex.y;

	float4 texDiffuse = txDiffuse.Sample(samLinear, input.tex);
	return texDiffuse * tint;
}