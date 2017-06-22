#version 430 core

//Cook-Torrance BRDF Shader based on code by Scott Eaton
//at Pixar Animation Studios
//https://renderman.pixar.com/view/cook-torrance-shader#Files

//Fresnel term calculation based on NVidia's "Fresnel Empirical Approximation"
//http://http.developer.nvidia.com/CgTutorial/cg_tutorial_chapter07.html


//The Cook-Torrance BRDF is of the form:
//cook += Cl*(F * D * G) / (PI * NdotV);
//
//Where 
//Cl	= light color
//F		= Fresnel term
//D		= Microfacet distribution probability based on slope of view angle from light
//G		= Geometric attenuation factor (Less luminance as you view from sharper angles)
//PI	= The constant pi
//NdotV	= Dot product of surface normal with the viewing position






#define FALSE					0
#define TRUE					1

const float PI					= 3.1415926535897932384626433832795f;


const vec4	AMBIENT_LIGHT		= vec4(1.f, 1.f, 1.f, 0.6f);


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//UNIFORMS
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

layout(binding = 0) uniform samplerCube	gTexSkybox;
layout(binding = 1) uniform sampler2D	gTexDiffuse;
layout(binding = 2) uniform sampler2D	gTexNormal;
layout(binding = 3) uniform sampler2D	gTexRoughness;

uniform vec3	gCameraPosition;
uniform vec3	gLightPos;
uniform vec4	gLightColor;
uniform float	gMinLightDist;
uniform float	gMaxLightDist;
uniform float	gMinLightPower;
uniform float	gMaxLightPower;

uniform float gFresnelBias;
uniform float gFresnelScale;
uniform float gFresnelPower;
uniform float gSpecularIntensity;

uniform float gRoughness;


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//IN OUTS
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

in vec2 passUV0;
in vec3 passPosition;
in vec4 passColor;
in vec3 passNormal;
in vec3 passTangent;
in vec3 passBitangent;
in vec3 passCubeCoords;

layout (location = 0) out vec4 outColor;
layout (location = 1) out vec4 outNormals;



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//GENERAL LIGHTING
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------------
vec3 GetNormal() {
	vec3 tangent		= passTangent;
	vec3 bitangent		= passBitangent;
	vec3 surfaceNormal	= cross(tangent, bitangent);
	mat3 tbn			= mat3(tangent, bitangent, passNormal);
	tbn					= transpose(tbn);

	vec3 normal			= texture(gTexNormal, passUV0).xyz;
	normal				= normal * vec3(2.f, 2.f, 1.f) - vec3(1.f, 1.f, 0.f);
	return normal * tbn;
}


//---------------------------------------------------------------------------------------------------------------------------
float CalcLightAttenuation(float distFromLightToSurface) {

	float attenuation = 0.f;

	attenuation = smoothstep(gMinLightDist, gMaxLightDist, distFromLightToSurface);
	attenuation = mix(gMinLightPower, gMaxLightPower, attenuation);
	return attenuation;
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//COOK-TORRANCE BRDF
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------------
float CalculateFresnelTerm(vec3 incidence, vec3 surfaceNormal) {

	return max(0.f, min(1.f, gFresnelBias + gFresnelScale * (1.f + dot(incidence, surfaceNormal)) * gFresnelPower));
}




//---------------------------------------------------------------------------------------------------------------------------
float ChiGGX(float viewAngle) {

	return viewAngle > 0.f ? 1.f : 0.f;
}


//---------------------------------------------------------------------------------------------------------------------------
float G1GGX(vec3 surfaceToPos, vec3 surfaceNormal, float roughness) {

	float NdotV = clamp(dot(surfaceToPos, surfaceNormal), 0.f, 1.f);
	float roughness2 = roughness * roughness;

	return (2.f * NdotV) / max(NdotV + sqrt(roughness2 + (1.0f - roughness2) * NdotV * NdotV), 1e-7f);
}


//---------------------------------------------------------------------------------------------------------------------------
float GeometricAttenuation(vec3 viewDirToSurface, vec3 lightDirToSurface, vec3 halfVec, vec3 surfaceNormal, float roughness) {

	return G1GGX(viewDirToSurface, surfaceNormal, roughness) * G1GGX(lightDirToSurface, halfVec, roughness);
}


//---------------------------------------------------------------------------------------------------------------------------
float CalcGeometricAttenuation(vec3 viewDirToSurface, vec3 halfVec, vec3 surfaceNormal, float roughness) {

	float VdotH2	= clamp(dot(viewDirToSurface, halfVec), 0.f, 1.f);
	float chi		= ChiGGX(VdotH2 / clamp(dot(viewDirToSurface, surfaceNormal), 0.f, 1.f));
	VdotH2			= VdotH2 * VdotH2;
	float tan2		= ( 1.f - VdotH2 ) / VdotH2;

	return (chi * 2.f) / (1.f + sqrt(1.f + roughness * roughness * tan2));
}


//---------------------------------------------------------------------------------------------------------------------------
float CalcMicrofacetDistrib(vec3 surfaceNormal, vec3 halfVec, float roughness) {

	float NdotH = dot(surfaceNormal, halfVec);
	float roughness2 = roughness * roughness;
	float NdotH2 = NdotH * NdotH;
	float den = NdotH2 * roughness2 + (1.f - NdotH2);
	return (ChiGGX(NdotH) * roughness2) / ( PI * den * den );
}


//---------------------------------------------------------------------------------------------------------------------------
vec4 CalcReflectionDiffuse(vec3 viewDirToSurface, vec3 surfaceNormal) {

	vec3 reflectedVec = reflect(viewDirToSurface, normalize(surfaceNormal));
	vec4 reflectionDiffuse = texture(gTexSkybox, reflectedVec);
	return reflectionDiffuse;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//MAIN
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------------
void main() {

	const float Kd = 0.5f;
	const float Ks = 0.5f;

	//FINAL FORMULA:
	//final = Kd * diffuse + Ks * spec;
	//brdf = (F * D * G) / (pi * NdotL * NdotV)


	//vec3	surfaceToEye		= normalize(gCameraPosition - passPosition);
	//vec3	surfaceToLight		= normalize(gLightPos - passPosition);
	//vec3	halfVec				= normalize(surfaceToEye + surfaceToLight);
	//vec3	surfaceNormal		= GetNormal();
	//vec4	diffuseTex			= texture2D(gTexDiffuse, passUV0);
	//vec4	reflectionDiffuse	= CalcReflectionDiffuse(-surfaceToEye, surfaceNormal);
	//float	lightAttenuation	= CalcLightAttenuation(length(gLightPos - passPosition));
	//
	//float	roughness			= 0.2f;
	//
	//
	//float	fresnel				= CalculateFresnelTerm(-surfaceToEye, surfaceNormal);
	//float	distrib				= CalcMicrofacetDistrib(surfaceNormal, halfVec, roughness);
	//float	geoAtt				= GeometricAttenuation(surfaceToEye, surfaceToLight, halfVec, surfaceNormal, roughness);
	//
	//float	NdotL				= dot(surfaceNormal, surfaceToLight);
	//float	NdotV				= dot(surfaceNormal, surfaceToEye);
	//
	//float	specFactor			= (fresnel * distrib * geoAtt) / (PI * NdotV);
	//
	//vec4	diffuseColor		= mix(reflectionDiffuse, diffuseTex, fresnel);
	//vec4	specColor			= gLightColor;
	//
	//
	//vec4 ambientLight = vec4(vec3(AMBIENT_LIGHT.rgb * AMBIENT_LIGHT.a), 1.f);
	//vec4 diffuse = (diffuseTex) * lightAttenuation * gLightColor * NdotL + (diffuseTex) * ambientLight;
	//
	//outColor = Kd * (diffuse) + Ks * lightAttenuation * specColor * specFactor;

	//outColor = vec4( GetNormal(), 1.f);



///***********************************************CORRECTING FRESNEL***********************************************************

	//vec3 vecToEye				= normalize(passPosition - gCameraPosition);
	//vec3 surfaceNormal			= GetNormal();
	//
	//
	//vec4 reflection				= CalcReflectionDiffuse(vecToEye, surfaceNormal);
	//float reflectionFactor		= CalculateFresnelTerm(vecToEye, surfaceNormal);
	//vec4 diffuse				= texture2D(gTexDiffuse, passUV0);
	//vec4 diffuseWithFresnel		= mix(reflection, diffuse, reflectionFactor);
	//
	//outColor					= diffuseWithFresnel;

//*****************************************************************************************************************************




///***********************************************CORRECTING MICROFACET SLOPE DISTRIBUTION*************************************

	//vec3 surfaceToEye			= normalize(gCameraPosition - passPosition);
	//vec3 surfaceToLight			= normalize(gLightPos - passPosition);
	//vec3 halfVec				= normalize(surfaceToEye + surfaceToLight);
	//float dist					= length(gLightPos - passPosition);
	//
	//vec3 surfaceNormal			= GetNormal();
	//
	//float roughness = 0.7f;
	//
	//float distrib				= CalcMicrofacetDistrib(surfaceNormal, halfVec, roughness);
	//float lightAttenuation		= CalcLightAttenuation(dist);
	//
	//vec4 diffuse				= texture2D(gTexDiffuse, passUV0);
	//outColor					= vec4(vec3(distrib), 1.f);

//*****************************************************************************************************************************



///***********************************************CORRECTING GEOMETRIC ATTENUATION*********************************************

	vec3 surfaceToEye			= normalize(gCameraPosition - passPosition);
	vec3 surfaceToLight			= normalize(gLightPos - passPosition);
	vec3 halfVec				= normalize(surfaceToEye + surfaceToLight);
	vec3 surfaceNormal			= GetNormal();

	float roughness = 0.2f;
	
	float geoAtt				= CalcGeometricAttenuation(surfaceToEye, surfaceToLight, surfaceNormal, roughness);
	outColor					= vec4(vec3(geoAtt), 1.f);  

//*****************************************************************************************************************************


	outNormals	= vec4(0.f);
}