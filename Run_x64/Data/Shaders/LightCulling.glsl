#version 430 core

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//UNIFORMS
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define MAX_NUM_LIGHTS_PER_TILE 256
#define TILE_SIZE_PIXELS 16
layout(local_size_x = TILE_SIZE_PIXELS, local_size_y = TILE_SIZE_PIXELS, local_size_z = 1) in;


//Assume model is identity since we're just doing point lights
uniform mat4 uProj;
uniform mat4 uView;
uniform uint uResX;
uniform uint uResY;
uniform uint uTileSizePixels;
uniform uint uMaxLightsPerTile;
uniform uint uNumLights;

struct Quad2D {
	vec2 topLeft;
	vec2 topRight;
	vec2 bottomLeft;
	vec2 bottomRight;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//SSBOS
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------------
struct LightData {
	float m_lightPositionX, m_lightPositionY, m_lightPositionZ;
	float m_lightColorX, m_lightColorY, m_lightColorZ, m_lightColorW;
	float m_minLightDistance;
	float m_maxLightDistance;
	float m_minLightPower;
	float m_maxLightPower;
};


//---------------------------------------------------------------------------------------------------------------------------
layout (std430, binding = 0) buffer LightDataBuffer { 
	LightData lightData[];
} lightBuffer;



//---------------------------------------------------------------------------------------------------------------------------
layout (std430, binding = 1) buffer IndexBuffer {
	int m_idx[];
} indexBuffer;


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//PLANE UTILS
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------------
struct Plane {
	vec3 m_p0;
	vec3 m_normal;
};

shared Plane leftFrustrum;
shared Plane rightFrustrum;
shared Plane topFrustrum;
shared Plane bottomFrustrum;
shared Plane backFrustrum;
shared Plane frontFrustrum;


//---------------------------------------------------------------------------------------------------------------------------
bool DoesSphereIntersectOrLieBelow(Plane plane, vec3 pos, float radius) {

	vec3 eVec = pos - plane.m_p0;	
	float distOfCenterToPlane = dot(plane.m_normal, eVec);

	return (abs(distOfCenterToPlane) <= radius || (abs(distOfCenterToPlane) >= radius && distOfCenterToPlane < 0.f));
}


//---------------------------------------------------------------------------------------------------------------------------
bool IsSphereInFrustrum(vec3 spherePos,    float radius,
	                    Plane leftFrustrum,   Plane rightFrustrum, Plane topFrustrum,
	                    Plane bottomFrustrum, Plane backFrustrum,  Plane frontFrustrum)
{
	
	bool intersects = false;

	bool aboveLeftFrustrum		= DoesSphereIntersectOrLieBelow(leftFrustrum,   spherePos, radius);
	bool aboveRightFrustrum		= DoesSphereIntersectOrLieBelow(rightFrustrum,  spherePos, radius);
	bool aboveTopFrustrum		= DoesSphereIntersectOrLieBelow(topFrustrum,    spherePos, radius);
	bool aboveBottomFrustrum	= DoesSphereIntersectOrLieBelow(bottomFrustrum, spherePos, radius);
	bool aboveBackFrustrum		= DoesSphereIntersectOrLieBelow(backFrustrum,   spherePos, radius);
	bool aboveFrontFrustrum		= DoesSphereIntersectOrLieBelow(frontFrustrum,  spherePos, radius);

	return (aboveLeftFrustrum && aboveRightFrustrum	&& aboveTopFrustrum	&& aboveBottomFrustrum && aboveBackFrustrum	&& aboveFrontFrustrum);
}


//---------------------------------------------------------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//MAIN COMPUTE
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------------------------------------------------------


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//UTILS
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------------
Quad2D GetThisTilesQuad(uint currInvocation) {

	ivec2 numTilesOnGrid	= ivec2(int(ceil(float(uResX) / float(uTileSizePixels))), int(ceil(float(uResY) / float(uTileSizePixels))));

	uint rowOrColumnWhicheverIsBigger = numTilesOnGrid.x;
	if (numTilesOnGrid.y > rowOrColumnWhicheverIsBigger) {
		rowOrColumnWhicheverIsBigger = numTilesOnGrid.y;
	}

	vec2 currTileGridLoc	= vec2(int(currInvocation % rowOrColumnWhicheverIsBigger), int(currInvocation / rowOrColumnWhicheverIsBigger));

	vec2 topLeft		= (vec2(uTileSizePixels, uTileSizePixels) * currTileGridLoc + vec2(0.f, uTileSizePixels));
	vec2 topRight		= (vec2(uTileSizePixels, uTileSizePixels) * currTileGridLoc + vec2(uTileSizePixels, uTileSizePixels));
	vec2 bottomLeft		= (vec2(uTileSizePixels, uTileSizePixels) * currTileGridLoc);
	vec2 bottomRight	= (vec2(uTileSizePixels, uTileSizePixels) * currTileGridLoc + vec2(uTileSizePixels, 0.f));

	Quad2D tileQuad;
	tileQuad.topLeft		= vec2(topLeft.x		/ float(uResX), topLeft.y		/ float(uResY))	* vec2(2.f, 2.f) - vec2(1.f, 1.f);
	tileQuad.topRight		= vec2(topRight.x		/ float(uResX), topRight.y		/ float(uResY))	* vec2(2.f, 2.f) - vec2(1.f, 1.f);
	tileQuad.bottomLeft		= vec2(bottomLeft.x		/ float(uResX), bottomLeft.y	/ float(uResY))	* vec2(2.f, 2.f) - vec2(1.f, 1.f);
	tileQuad.bottomRight	= vec2(bottomRight.x	/ float(uResX), bottomRight.y	/ float(uResY))	* vec2(2.f, 2.f) - vec2(1.f, 1.f);

	return tileQuad;
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//CULL
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------------
void ConstructTileFrustum(uint localCurrInvocation) {

	Quad2D tileQuad = GetThisTilesQuad(localCurrInvocation);
	mat4 viewProjInverse = inverse(uView * uProj);

	vec4 frontTopLeftClip		= vec4(vec4(tileQuad.topLeft,		-1.f,	1.f) * viewProjInverse);
	vec4 frontTopRightClip		= vec4(vec4(tileQuad.topRight,		-1.f,	1.f) * viewProjInverse);
	vec4 frontBottomLeftClip	= vec4(vec4(tileQuad.bottomLeft,	-1.f,	1.f) * viewProjInverse);
	vec4 frontBottomRightClip	= vec4(vec4(tileQuad.bottomRight,	-1.f,	1.f) * viewProjInverse);
	vec4 backTopLeftClip		= vec4(vec4(tileQuad.topLeft,		1.f,	1.f) * viewProjInverse);
	vec4 backTopRightClip		= vec4(vec4(tileQuad.topRight,		1.f,	1.f) * viewProjInverse);
	vec4 backBotLeftClip		= vec4(vec4(tileQuad.bottomLeft,	1.f,	1.f) * viewProjInverse);
	vec4 backBotRightClip		= vec4(vec4(tileQuad.bottomRight,	1.f,	1.f) * viewProjInverse);

	vec3 frontTopLeftWS			= frontTopLeftClip.xyz		/ frontTopLeftClip.w;
	vec3 frontTopRightWS		= frontTopRightClip.xyz		/ frontTopRightClip.w;
	vec3 frontBottomLeftWS		= frontBottomLeftClip.xyz	/ frontBottomLeftClip.w;
	vec3 frontBottomRightWS		= frontBottomRightClip.xyz	/ frontBottomRightClip.w;

	vec3 backTopLeftWS			= backTopLeftClip.xyz		/ backTopLeftClip.w;
	vec3 backTopRightWS			= backTopRightClip.xyz		/ backTopRightClip.w;
	vec3 backBottomLeftWS		= backBotLeftClip.xyz		/ backBotLeftClip.w;
	vec3 backBottomRightWS		= backBotRightClip.xyz		/ backBotRightClip.w;

	leftFrustrum.m_normal	= -normalize(cross(backTopLeftWS - frontTopLeftWS, frontBottomLeftWS - frontTopLeftWS));
	leftFrustrum.m_p0		= frontTopLeftWS;

	rightFrustrum.m_normal	= -normalize(cross(frontBottomRightWS - frontTopRightWS, backTopRightWS - frontTopRightWS));
	rightFrustrum.m_p0		= frontTopRightWS;

	topFrustrum.m_normal	= -normalize(cross(backTopRightWS - frontTopRightWS, frontTopLeftWS - frontTopRightWS));
	topFrustrum.m_p0		= frontTopRightWS;

	bottomFrustrum.m_normal	= -normalize(cross(backBottomLeftWS - frontBottomLeftWS, frontBottomRightWS - frontBottomLeftWS));
	bottomFrustrum.m_p0		= frontBottomLeftWS;

	backFrustrum.m_normal	= -normalize(cross(backTopRightWS - backTopLeftWS, backBottomLeftWS - backTopLeftWS));
	backFrustrum.m_p0		= backTopLeftWS;

	frontFrustrum.m_normal	= -normalize(cross(frontTopLeftWS - frontTopRightWS, frontBottomRightWS - frontTopRightWS));
	frontFrustrum.m_p0		= frontTopLeftWS;
}


//---------------------------------------------------------------------------------------------------------------------------
bool CullLight(uint whichLightIdx) {

	LightData currLight = lightBuffer.lightData[whichLightIdx];

	float radius		= currLight.m_maxLightDistance;
	vec3 position		= vec3(currLight.m_lightPositionX, currLight.m_lightPositionY, currLight.m_lightPositionZ);

	return IsSphereInFrustrum(position, radius, leftFrustrum, rightFrustrum, topFrustrum, bottomFrustrum, backFrustrum, frontFrustrum);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//MAIN
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

shared uint numLightsInTileSoFar = 0;
shared uint visibleLightCount = 0;
shared int visibleLightIndices[MAX_NUM_LIGHTS_PER_TILE];

//---------------------------------------------------------------------------------------------------------------------------
void main() {

	ivec2 location		= ivec2(gl_GlobalInvocationID.xy);
	ivec2 itemID		= ivec2(gl_LocalInvocationID.xy);
	ivec2 tileID		= ivec2(gl_WorkGroupID.xy);
	ivec2 tileNumber	= ivec2(gl_NumWorkGroups.xy);
	uint currInvocation = tileID.y * tileNumber.x + tileID.x;

	uint invocationOffsetIntoIndexBuffer	= currInvocation * uMaxLightsPerTile; //Give this tile a spot to write into the light buffer
	uint numLightsToCull					= uNumLights;

	if(gl_LocalInvocationIndex == 0) { //First thread in tile
		ConstructTileFrustum(currInvocation);
	}

	barrier();


	uint threadCount = TILE_SIZE_PIXELS * TILE_SIZE_PIXELS;
	uint passCount = (numLightsToCull + threadCount - 1) / threadCount;
	for(int i = 0; i < passCount; i++) {

		uint lightIndex = i * threadCount + gl_LocalInvocationIndex;

		if (lightIndex >= numLightsToCull) {
			break;
		}

		if(CullLight(lightIndex)) {
			uint offset = atomicAdd(visibleLightCount, 1);
			visibleLightIndices[offset] = int(lightIndex);
		}
	}

	barrier();

	if(gl_LocalInvocationIndex == 0) {

		uint offset = currInvocation * MAX_NUM_LIGHTS_PER_TILE;
		for (uint i = 0; i < visibleLightCount; i++) {
			indexBuffer.m_idx[offset + i] = visibleLightIndices[i];
		}

		if (visibleLightCount != MAX_NUM_LIGHTS_PER_TILE) {
			indexBuffer.m_idx[offset + visibleLightCount] = -1;
		}
	}
}