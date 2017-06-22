#version 430 core

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//UNIFORMs, IN-OUTs
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

uniform sampler2D gTexDiffuse;

in vec2 passUV0;
out vec4 outColor;

//---------------------------------------------------------------------------------------------------------------------------
layout (std430, binding = 1) buffer IndexBuffer {
	int m_idx[];
} indexBuffer;


const uint MAX_NUM_LIGHTS_PER_TILE	= 256;
const uint TILE_SIZE_PIXELS			= 16;

const uint RES_X				= 1920;
const uint RES_Y				= 1080;
const float TILE_LINE_THICKNESS	= 0.001f;



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//MAIN
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void RenderLightHeatmap();
bool ShouldDrawLineForTile();
uint GetTileIndexForFragCoord();
uint GetStartIntoIndexBuffer();
uint CountLightsInTile();

//---------------------------------------------------------------------------------------------------------------------------
void RenderLightHeatmap() {

	if(ShouldDrawLineForTile()) {
		outColor = vec4(0.f, 0.f, 0.f, 1.f);
	}
	else {

		uint numLights = CountLightsInTile();

		if(numLights > 0) {
			outColor = vec4(0.f, 0.f, 0.5f + float(numLights * 10) / MAX_NUM_LIGHTS_PER_TILE, 1.f);
		}
		else {
			outColor = vec4(texture2D(gTexDiffuse, passUV0).xyz, 1.f) * vec4(0.5f, 0.5f, 0.5f, 1.f);
		}
	}
}

//---------------------------------------------------------------------------------------------------------------------------
bool ShouldDrawLineForTile() {

	
	vec2 normFragCoord					= vec2(float(gl_FragCoord.x) / float(RES_X), float(gl_FragCoord.y) / float(RES_Y));
	vec2 tileBoundaryStep				= vec2(float(TILE_SIZE_PIXELS) / float(RES_X), float(TILE_SIZE_PIXELS) / float(RES_Y));
	vec2 fragClosenessToTileBoundary	= vec2(mod(normFragCoord.x, tileBoundaryStep.x), mod(normFragCoord.y, tileBoundaryStep.y));

	if(fragClosenessToTileBoundary.x <= TILE_LINE_THICKNESS || fragClosenessToTileBoundary.y <= TILE_LINE_THICKNESS) {
		return true;
	}
	else {
		return false;
	}
}


//---------------------------------------------------------------------------------------------------------------------------
uint GetTileIndexForFragCoord() {

	vec2 normalizedFragCoord	= vec2(floor(gl_FragCoord.x / TILE_SIZE_PIXELS), floor(gl_FragCoord.y / TILE_SIZE_PIXELS));
	uint numTilesWidth			= uint(ceil(RES_X / TILE_SIZE_PIXELS));

	return uint(normalizedFragCoord.x) + uint(normalizedFragCoord.y) * numTilesWidth;
}


//---------------------------------------------------------------------------------------------------------------------------
uint GetStartIntoIndexBuffer() {

	uint tileIdx	= GetTileIndexForFragCoord();
	uint lightIdx	= tileIdx * MAX_NUM_LIGHTS_PER_TILE;
	return lightIdx;
}


//---------------------------------------------------------------------------------------------------------------------------
uint CountLightsInTile() {

	uint startIdx = GetStartIntoIndexBuffer();
	uint lightCount = 0;

	for(uint idx = 0; idx < MAX_NUM_LIGHTS_PER_TILE && indexBuffer.m_idx[startIdx + idx] != -1; idx++) {
		lightCount++;
	}

	return lightCount;
}


//---------------------------------------------------------------------------------------------------------------------------
void main() { 

	RenderLightHeatmap();
}