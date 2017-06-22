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

layout (location = 0) out vec4 outColor;
layout (location = 1) out vec4 outNormals;


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
//MAIN
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------------
void main() {

	vec4 diffuse	= texture(gTexDiffuse, passUV0);

	if(gUsesDiffuseTex == TRUE) {
		outColor = diffuse * gDiffuseColor;
	}
	else if(gUsesDiffuseTex == FALSE) {
		outColor = gDiffuseColor;
	}

	outNormals		= vec4(GetNormal(), 1.f);
}