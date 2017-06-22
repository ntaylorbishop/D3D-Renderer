#version 430 core

//Based on a combination between two implementations by Daniel Holden and John Chapman
//Daniel Holden: http://theorangeduck.com/page/pure-depth-ssao
//John Chapman:  http://john-chapman-graphics.blogspot.com/2013/01/ssao-tutorial.html
//Algorithm originally designed by Crytek

uniform sampler2D	gTexDepth;
uniform sampler2D	gTexRandom;
uniform mat4		gSceneView;
uniform mat4		gSceneProj;


in vec2 passUV0;
out vec4 outColor;


const int KERNEL_SIZE = 16;
const float Z_NEAR = 0.1f;
const float Z_FAR = 1000.f;

const float KERNEL_RADIUS = 0.5f;

const vec2 RANDOM_TEX_UV_SCALE = vec2(1920.f / 256.f, 1080.f / 256.f);


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//UTILS
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

vec3 GetNormalFromDepth(vec2 uv);
float GetLinearDepth(vec2 uv);
vec3 GetPositionFromDepth(mat4 viewProjInverse);
mat3 ConstructTBNFromNormalAndRandomVec(vec3 normal);


//---------------------------------------------------------------------------------------------------------------------------
vec3 GetNormalFromDepth(vec2 uv) {
	
	float linearDepth = GetLinearDepth(uv);

	float xOffset = dFdx(uv.x);
	float yOffset = dFdy(uv.y);

	vec3 p0 = vec3(uv.x, uv.y, linearDepth);
	vec3 p1 = vec3(xOffset, uv.y, GetLinearDepth(vec2(xOffset, uv.y)));
	vec3 p2 = vec3(uv.x, yOffset, GetLinearDepth(vec2(uv.x, yOffset)));

	vec3 right	= p1 - p0;
	vec3 up		= p2 - p0;

	vec3 normal = cross(right, up);

	normal *= 2.f - 1.f;
	normalize(normal);
	return normal;
}

//---------------------------------------------------------------------------------------------------------------------------
float GetLinearDepth(vec2 uv) {

	return texture2D(gTexDepth, uv).r;

	float depth = texture2D(gTexDepth, uv).r;
	float linearDepth = (2.f * Z_NEAR) / (Z_FAR + Z_NEAR - depth * (Z_FAR - Z_NEAR));
	return linearDepth;
}


//---------------------------------------------------------------------------------------------------------------------------
vec3 GetPositionFromDepth(mat4 viewProjInverse) {
	
	float x = passUV0.x * 2.f - 1.f;
	float y = passUV0.y * 2.f - 1.f;
	
	float linearDepth = GetLinearDepth(passUV0);
	float z = linearDepth * 2.f - 1.f;

	vec4 posNDC = vec4(x, y, z, 1.f);
	posNDC *= viewProjInverse;
	vec3 posWorld = posNDC.xyz / posNDC.z;

	return posWorld;
}


//---------------------------------------------------------------------------------------------------------------------------
mat3 ConstructTBNFromNormalAndRandomVec(vec3 normal) {

	vec3 randVec = texture(gTexRandom, passUV0 * RANDOM_TEX_UV_SCALE).xyz * 2.f - 1.f;
	vec3 tangent = normalize(randVec - normal * dot(randVec, normal)); //Use Gram-Schmidt to construct orthonormal basis
	vec3 bitangent = cross(normal, tangent);
	mat3 tbn = mat3(tangent, bitangent, normal);
	return tbn;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//MAIN
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------------------------------------------
void main() {

	mat4 viewProjInverse = inverse(gSceneView * gSceneProj);
	vec3 fragPosWorld = GetPositionFromDepth(viewProjInverse);
	vec3 fragNormal = GetNormalFromDepth(passUV0);
	mat3 tbn = ConstructTBNFromNormalAndRandomVec(fragNormal);

	//Generate surface-space randomized normals
	vec3 kernel[KERNEL_SIZE];
	for (int i = 0; i < KERNEL_SIZE; ++i) {
		kernel[i] = vec3(noise1(0.f), noise1(0.f), (noise1(0.f) + 1.f) / 2.f);
		normalize(kernel[i]);

		//Evenly distribute within the hemisphere
		kernel[i] *= (noise1(0.f) + 1.f) / 2.f;

		//Cluster points closer to the center with exponential falloff
		float scale = float(i) / float(KERNEL_SIZE);
		scale = mix(0.1f, 1.0f, scale * scale);
		kernel[i] *= scale;
	}


	//ACCUMULATE OCCLUSION
	float occlusion = 0.0;
	for (int i = 0; i < KERNEL_SIZE; ++i) {

		// get sample position:
		vec3 rSample = tbn * kernel[i];
		rSample = rSample * KERNEL_RADIUS + fragPosWorld;
  
		// project sample position:
		vec4 offset = vec4(rSample, 1.f);
		offset = gSceneProj * offset;
		offset.xy /= offset.w;
		offset.xy = offset.xy * 0.5f + 0.5f;
  
		//Get linearized depth
		float sampleDepth = GetLinearDepth(offset.xy);
  
		// range check & accumulate:
		float rangeCheck= abs(fragPosWorld.z - sampleDepth) < KERNEL_RADIUS ? 1.0 : 0.0;
		occlusion += (sampleDepth <= rSample.z ? 1.0 : 0.0) * rangeCheck;
	}

	occlusion = 1.f - (occlusion / KERNEL_SIZE);
	outColor = vec4(vec3(occlusion), 1.f);
}