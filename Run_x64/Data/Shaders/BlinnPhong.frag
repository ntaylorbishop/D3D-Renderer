#version 430 core

#define FALSE	0
#define TRUE	1

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//DEFINES
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define TILE_SIZE_PIXELS 16

#define FORWARDPLUS_DEBUG_DRAW
//#define DEBUG_DRAW_OPTIONS

#ifdef FORWARDPLUS_DEBUG_DRAW
	#define TILE_LINE_THICKNESS 0.001
#endif


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//UNIFORMS
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

layout(binding = 0) uniform sampler2D gTexDiffuse;
layout(binding = 1) uniform sampler2D gTexNormal;
layout(binding = 2) uniform sampler2D gTexSpecular;
layout(binding = 4) uniform sampler2D gTexSSAO;

uniform vec4	gAmbientLight;
uniform vec3	gCameraPosition;
uniform float	gSpecularPower;
uniform int		gWhatToRender;
uniform uint	gTileSizePixels;
uniform uint	gResX;
uniform uint	gResY;
uniform uint	gMaxLightsPerTile;

uniform mat4	gView;
uniform mat4	gProj;
uniform bool	gIsUsingSSAO;

uniform uint	gHeatmapView;

//FOR SHADOW MAP
layout(binding = 3) uniform samplerCubeShadow	gShadowMap;
uniform bool									gHasShadowLight;
uniform vec3									gShadowLightPos;
uniform float									gNearPlane;
uniform float									gFarPlane;


//FOR REFLECTIONS
layout (binding = 5)  uniform samplerCube gTexEnv0;
layout (binding = 6)  uniform samplerCube gTexEnv1;
layout (binding = 7)  uniform samplerCube gTexEnv2;
layout (binding = 8)  uniform samplerCube gTexEnv3;
layout (binding = 9)  uniform samplerCube gTexEnv4;
layout (binding = 10) uniform samplerCube gTexEnv5;
layout (binding = 11) uniform samplerCube gTexEnv6;
layout (binding = 12) uniform samplerCube gTexEnv7;
layout (binding = 13) uniform samplerCube gTexEnv8;
layout (binding = 14) uniform samplerCube gTexEnv9;
layout (binding = 15) uniform samplerCube gTexEnv10;
layout (binding = 16) uniform samplerCube gTexEnv11;
layout (binding = 17) uniform samplerCube gTexEnv12;
layout (binding = 18) uniform samplerCube gTexEnv13;
layout (binding = 19) uniform samplerCube gTexEnv14;
layout (binding = 20) uniform samplerCube gTexEnv15;

uniform vec3 envMins0;
uniform vec3 envMaxs0;
uniform vec3 envMins1;
uniform vec3 envMaxs1;
uniform vec3 envMins2;
uniform vec3 envMaxs2;
uniform vec3 envMins3;
uniform vec3 envMaxs3;
uniform vec3 envMins4;
uniform vec3 envMaxs4;
uniform vec3 envMins5;
uniform vec3 envMaxs5;
uniform vec3 envMins6;
uniform vec3 envMaxs6;
uniform vec3 envMins7;
uniform vec3 envMaxs7;
uniform vec3 envMins8;
uniform vec3 envMaxs8;
uniform vec3 envMins9;
uniform vec3 envMaxs9;
uniform vec3 envMins10;
uniform vec3 envMaxs10;
uniform vec3 envMins11;
uniform vec3 envMaxs11;
uniform vec3 envMins12;
uniform vec3 envMaxs12;
uniform vec3 envMins13;
uniform vec3 envMaxs13;
uniform vec3 envMins14;
uniform vec3 envMaxs14;
uniform vec3 envMins15;
uniform vec3 envMaxs15;

uniform vec3 envPos0;
uniform vec3 envPos1;
uniform vec3 envPos2;
uniform vec3 envPos3;
uniform vec3 envPos4;
uniform vec3 envPos5;
uniform vec3 envPos6;
uniform vec3 envPos7;
uniform vec3 envPos8;
uniform vec3 envPos9;
uniform vec3 envPos10;
uniform vec3 envPos11;
uniform vec3 envPos12;
uniform vec3 envPos13;
uniform vec3 envPos14;
uniform vec3 envPos15;

uniform uint envMapCount;



const uint MAX_NUM_LIGHTS_PER_TILE = 256;


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//SSBOs
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------------
struct LightData {

	float m_lightPositionX, m_lightPositionY,	m_lightPositionZ;
	float m_lightColorX,	m_lightColorY,		m_lightColorZ,		m_lightColorW;
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
//IN OUTS
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

in vec2 passUV0;
in vec3 passPosition;
in vec4 passColor;
in vec3 passNormal;
in vec3 passTangent;
in vec3 passBitangent;


layout (location = 0) out vec4 outColor;

vec2 moddedUVs = passUV0;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//GLOBAL CONSTANTS
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

vec3 m_normal		= vec3(0.f, 0.f, 0.f);
bool isUsingNormal	= false;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//FUNCTION PROTOTYPES
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//FORWARDPLUS
uint GetTileIndexForFragCoord();
vec3 GetFragCoordInRangeZeroToOne();
uint GetNumberOfLightsInMyTile();
uint GetLightIndexIntoBufferForThisTile(uint tileIdx, uint localLightIdx);
//LIGHT MODEL
vec3 GetNormal();
vec4 CalculateLightFromAllForThisTile();
//DEBUG
bool ShouldDrawLineForTile();
void RenderLightHeatmap();


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//SSAO UTILS
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------------
vec2 GetFullScreenUV() {

	vec4 fragCoord = gl_FragCoord;

	vec2 ndc = vec2(0.f);
	ndc.x = fragCoord.x / gResX;
	ndc.y = fragCoord.y / gResY;
	return ndc;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//UTILS
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------------
uint GetTileIndexForFragCoord() {

	vec2 normalizedFragCoord = vec2(floor(gl_FragCoord.x / gTileSizePixels), floor(gl_FragCoord.y / gTileSizePixels));
	uint numTilesWidth = uint(ceil(gResX / gTileSizePixels));

	return uint(normalizedFragCoord.x) + uint(normalizedFragCoord.y) * numTilesWidth;
}


//---------------------------------------------------------------------------------------------------------------------------
uint GetStartIntoIndexBuffer() {

	uint tileIdx	= GetTileIndexForFragCoord();
	uint lightIdx	= tileIdx * gMaxLightsPerTile;
	return lightIdx;
}


//---------------------------------------------------------------------------------------------------------------------------
uint GetLightIndexIntoBufferForThisTile(uint localLightIdx) {

	uint tileIdx	= GetTileIndexForFragCoord();
	uint lightIdx	= tileIdx * gMaxLightsPerTile + localLightIdx;
	return	indexBuffer.m_idx[lightIdx];
}


//---------------------------------------------------------------------------------------------------------------------------
vec3 GetNormal() {

	vec3 tangent		= passTangent;
	vec3 bitangent		= passBitangent;
	vec3 surfaceNormal	= cross(tangent, bitangent);
	mat3 tbn			= mat3(tangent, bitangent, passNormal);
	tbn					= transpose(tbn);

	vec3 normal			= texture(gTexNormal, moddedUVs).xyz;
	normal				= normal * vec3(2.f, 2.f, 1.f) - vec3(1.f, 1.f, 0.f);

	return normal * tbn;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//CALCULATE LIGHT FACTOR
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------------
vec4 GetAmbientLightFactor() {
	
	vec2 ssaoUV = GetFullScreenUV();
	float aoFactor = texture2D(gTexSSAO, ssaoUV).r;
	vec4 ambientColor = vec4(gAmbientLight.rgb * gAmbientLight.a, 1.f);

	if(gIsUsingSSAO) {
		return vec4(ambientColor.xyz * aoFactor, 1.f);
	}
	else {
		return ambientColor;
	}
}


float CalculateShadowVisibility();


//---------------------------------------------------------------------------------------------------------------------------
vec4 CalculateLightFromAllForThisTile() {

	vec3 ambientLight	= gAmbientLight.rgb * gAmbientLight.a;
	vec3 surfaceLight	= vec3(0.f, 0.f, 0.f);
	vec3 specColor		= vec3(0.f, 0.f, 0.f);

	float attenuation;
	float amountBasedOnNormalFromLightAngle;

	uint startIdx = GetStartIntoIndexBuffer();

	for(uint idx = 0; idx < MAX_NUM_LIGHTS_PER_TILE && indexBuffer.m_idx[startIdx + idx] != -1; idx++) {
		
		int lightIdx = indexBuffer.m_idx[startIdx + idx];
				
		vec3 lightPosition = vec3(lightBuffer.lightData[lightIdx].m_lightPositionX, 
								  lightBuffer.lightData[lightIdx].m_lightPositionY, 
								  lightBuffer.lightData[lightIdx].m_lightPositionZ);
		vec3 lightColor    = vec3(lightBuffer.lightData[lightIdx].m_lightColorX, 
								  lightBuffer.lightData[lightIdx].m_lightColorY, 
								  lightBuffer.lightData[lightIdx].m_lightColorZ);

		vec3 vec_to_light	= lightPosition - passPosition;
		float dist			= length(vec_to_light);
		vec_to_light		= normalize(vec_to_light);
		vec3 normal			= GetNormal();

		//ATTENUATION
		float minLightDist		= lightBuffer.lightData[lightIdx].m_minLightDistance;
		float maxLightDist		= lightBuffer.lightData[lightIdx].m_maxLightDistance;
		float minLightPower		= lightBuffer.lightData[lightIdx].m_minLightPower;
		float maxLightPower		= lightBuffer.lightData[lightIdx].m_maxLightPower;

		attenuation = smoothstep(minLightDist, maxLightDist, dist);
		attenuation = mix(minLightPower, maxLightPower, attenuation);

		amountBasedOnNormalFromLightAngle = clamp(dot(vec_to_light, normal), 0.f, 1.f);
	
		surfaceLight += lightColor * amountBasedOnNormalFromLightAngle * attenuation;

		//SPECULAR
		float specular_intensity = 1.f;
		vec3 vec_to_eye			= normalize(gCameraPosition - passPosition);
		vec3 half_vector		= vec_to_light + vec_to_eye;
		half_vector				= normalize(half_vector);
		float half_dot_normal	= clamp(dot(half_vector, normal), 0.f, 1.f);
		float intensity			= pow(half_dot_normal, gSpecularPower) * specular_intensity * attenuation;

		specColor += intensity * lightColor;
	}


	//SHADOW
	if(gHasShadowLight) {
		const float shadowIntensity = 1.f;
		const vec3 shadowLightColor = vec3(1.f, 1.f, 1.f);

		float shadowFactor = CalculateShadowVisibility();
		
		vec3 vecToShadowLight	= gShadowLightPos - passPosition;
		float dist				= length(vecToShadowLight);
		vecToShadowLight		= normalize(vecToShadowLight);
		vec3 normForShadow		= GetNormal();
		float angleAmount		= clamp(dot(vecToShadowLight, normForShadow), 0.f, 1.f);

		surfaceLight += shadowLightColor * angleAmount * shadowIntensity * shadowFactor;

		//SPECULAR
		vec3 vec_to_light			= gShadowLightPos - passPosition;
		vec_to_light				= normalize(vec_to_light);
		float specular_intensity	= 0.5f;
		vec3 vec_to_eye				= normalize(gCameraPosition - passPosition);
		vec3 half_vector			= vec_to_light + vec_to_eye;
		half_vector					= normalize(half_vector);
		float half_dot_normal		= clamp(dot(half_vector, normForShadow), 0.f, 1.f);
		float intensity				= pow(half_dot_normal, gSpecularPower) * specular_intensity;

		vec3 reflected				= reflect(vec_to_eye, normForShadow);
	}
	//END SHADOW

	vec4 textureColor	= texture(gTexDiffuse, moddedUVs);
	return textureColor * (GetAmbientLightFactor() + vec4(surfaceLight, 0.f)) + vec4(specColor, 0.f);
}


//---------------------------------------------------------------------------------------------------------------------------
float VectorToDepthValue(vec3 Vec) {

	vec3 AbsVec = abs(Vec);
	float LocalZcomp = max(AbsVec.x, max(AbsVec.y, AbsVec.z));

	const float f = gFarPlane;
	const float n = gNearPlane;
	float NormZComp = (f + n) / (f - n) - (2 * f * n) / (f - n) / LocalZcomp;
	return (NormZComp + 1.0) * 0.5;
}


//---------------------------------------------------------------------------------------------------------------------------
float CalculateShadowVisibility() {  

	float SHADOW_BIAS = 0.0005;

	if(!gHasShadowLight) {
		return 1.f;
	}

	const int numSamples = 20;
	float shadowFactor = 0.0;

	vec3 gridSamplingDisk[20] = vec3[] (
		vec3(1, 1,  1),	vec3( 1, -1,  1),	vec3(-1, -1,  1),	vec3(-1, 1,  1), 
		vec3(1, 1, -1), vec3( 1, -1, -1),	vec3(-1, -1, -1),	vec3(-1, 1, -1),
		vec3(1, 1,  0),	vec3( 1, -1,  0),	vec3(-1, -1,  0),	vec3(-1, 1,  0),
		vec3(1, 0,  1),	vec3(-1,  0,  1),	vec3( 1,  0, -1),	vec3(-1, 0, -1),
		vec3(0, 1,  1),	vec3( 0, -1,  1),	vec3( 0, -1, -1),	vec3( 0, 1, -1)
	);

	vec3 VertToLightWS	= gShadowLightPos - passPosition;
	float LightDepth    = VectorToDepthValue(VertToLightWS);
	float viewDistance	= length(gCameraPosition - passPosition);
	float diskRadius	= (1.0 + (viewDistance / gFarPlane)) / 25.0;  

	for(uint i = 0; i < numSamples; i++) {
		float sizeOfCubeTexture = 1024;
		float diskRadius = (1.0 + (1.0 - LightDepth) * 3) / sizeOfCubeTexture;
		float depthCompare = texture(gShadowMap, vec4(normalize(VertToLightWS) + gridSamplingDisk[i] * diskRadius, LightDepth - SHADOW_BIAS));
		shadowFactor += depthCompare;
	}

	shadowFactor = shadowFactor / float(numSamples);

	return shadowFactor;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//MAIN
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------------
void main() {

	vec4 finalColor	= CalculateLightFromAllForThisTile();
	outColor	= finalColor;
}