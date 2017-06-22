#version 430

#define FALSE	0
#define TRUE	1

layout (binding = 0) uniform sampler2D	gTexDiffuse;
layout (binding = 1) uniform sampler2D	gTexNormal;
uniform vec4		gDiffuseColor;
uniform uint		gUsesDiffuseTex;

in vec2 passUV0;
in vec3 passPosition;
in vec4 passColor;
in vec3 passNormal;
in vec3 passTangent;
in vec3 passBitangent;

layout (location = 0) out vec4	outAccum;
layout (location = 1) out vec4	outRevealage;


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//UTILS
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------------
vec3 GetNormal() {

	vec3 tangent = passTangent;
	vec3 bitangent = passBitangent;
	vec3 surfaceNormal = cross(tangent, bitangent);
	mat3 tbn = mat3(tangent, bitangent, passNormal);
	tbn = transpose(tbn);

	vec3 normal = texture(gTexNormal, passUV0).xyz;
	normal = normal * vec3(2.f, 2.f, 1.f) - vec3(1.f, 1.f, 0.f);

	return normal * tbn;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//OIT
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------------
float GetTrueDepth() {

	return gl_FragCoord.z;
}


//---------------------------------------------------------------------------------------------------------------------------
void WritePixel(vec4 color) { 
	
	float max1	= max(color.r, color.g);
	float max2	= max(max1, color.b) * color.a;
	float min1	= min(1.0, max2);
	float z		= GetTrueDepth();

	float weight = max(min(1.0, max(max(color.r, color.g), color.b) * color.a), color.a) * clamp(0.03 / (1e-5 + pow(z / 200, 4.0)), 1e-2, 3e3);

	// Blend Func: GL_ONE, GL_ONE
	outAccum = vec4(color.rgb * color.a, color.a) * weight;
	 
	// Blend Func: GL_ZERO, GL_ONE_MINUS_SRC_ALPHA
	outRevealage = vec4(0.f, 0.f, 0.f, color.a);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//MAIN
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------------
void main() {

	WritePixel(gDiffuseColor);
}