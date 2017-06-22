#version 430 core

//Based on a combination between two implementations by Daniel Holden and John Chapman
//Daniel Holden: http://theorangeduck.com/page/pure-depth-ssao
//John Chapman:  http://john-chapman-graphics.blogspot.com/2013/01/ssao-tutorial.html
//Algorithm originally designed by Crytek

layout (binding = 0) uniform sampler2D	gTexDepth;
layout (binding = 1) uniform sampler2D	gTexRandom;
layout (binding = 2) uniform sampler2D	gTexNormals;
uniform mat4		gSceneView;
uniform mat4		gSceneProj;

const int			KERNEL_SIZE = 16;
uniform vec3		gKernel[KERNEL_SIZE];

uniform bool		gDebugDraw;


in vec2 passUV0;
out vec4 outColor;


const float		Z_NEAR				= 0.1f;
const float		Z_FAR				= 1000.f;
const float		KERNEL_RADIUS		= 20.f;
const vec2		RANDOM_TEX_UV_SCALE = vec2(1920.f / 4.f, 1080.f / 4.f);


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//UTILS
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

mat3 ConstructTBNFromNormalAndRandomVec(vec3 normal);
vec3 GetViewSpacePosition(vec2 uv);

//---------------------------------------------------------------------------------------------------------------------------
mat3 ConstructTBNFromNormalAndRandomVec(vec3 normal) {

	vec3 randNormalSample		= texture2D(gTexRandom, passUV0 * RANDOM_TEX_UV_SCALE).xyz;
	vec3 randomRotatingNormal	= randNormalSample * vec3(2.f, 2.f, 2.f) - vec3(1.f, 1.f, 1.f);
	vec3 tangent				= normalize(randomRotatingNormal - normal * dot(randomRotatingNormal, normal));
	vec3 bitangent				= cross(normal, tangent);
	mat3 tbn					= mat3(tangent, bitangent, normal);
	tbn = transpose(tbn);
	return tbn;
}


//---------------------------------------------------------------------------------------------------------------------------
vec3 GetViewSpacePosition(vec2 uv) {

	vec2 ndc = uv * vec2(2.f) - vec2(1.f);
	float depth = texture2D(gTexDepth, uv).r;

	vec4 projectedPos = vec4(ndc.x, ndc.y, depth, 1.f);
	vec4 viewPos = projectedPos * inverse(gSceneProj);
	return viewPos.xyz / viewPos.w; 

}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//MAIN
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------------
void main() {

	vec2 ndc = passUV0 * vec2(2.f) - vec2(1.f);
	float thfov = tan(120.f / 2.f); // can do this on the CPU

	vec3 origin		= GetViewSpacePosition(passUV0);
	vec3 fragNormal	= texture2D(gTexNormals, passUV0).xyz * vec3(2.f) - vec3(1.f);
	fragNormal		= normalize(fragNormal);

	mat3 tbn		= ConstructTBNFromNormalAndRandomVec(fragNormal);

	//ACCUMULATE OCCLUSION
	float occlusion = 0.0;

	for (int i = 0; i < KERNEL_SIZE; ++i) {

		//Get sample position
		vec3 rSample = gKernel[i] * tbn;
		rSample = rSample * KERNEL_RADIUS + origin;
  
		//Project sample position into screen space
		vec4 offset = vec4(rSample, 1.f);

		offset = offset * gSceneProj;
		offset.xyz /= offset.w;
		offset.xyz = offset.xyz * 0.5f + 0.5f;
  
		//Get linearized depth
		float sampleDepth = GetViewSpacePosition(offset.xy).z;
  
		//Range check and accumulate
		float rangeCheck = abs(origin.z - sampleDepth) < KERNEL_RADIUS ? 1.0 : 0.0;
		rangeCheck *= (texture(gTexDepth, passUV0).r < 0.9999) ? 1.0 : 0.0;
		occlusion += (sampleDepth <= rSample.z ? 1.0 : 0.0) * rangeCheck;
	}

	occlusion = 1.f - (occlusion / KERNEL_SIZE);


	if(gDebugDraw) {
		outColor = vec4(vec3(occlusion), 1.f);
	}
	else {
		outColor = vec4(1.f);
	}
}