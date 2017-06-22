#version 430 core


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//UNIFORMS
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

layout(binding = 0) uniform samplerCube gTexEnvironmentMap;
layout(binding = 1) uniform sampler2D	gTexNormal;
uniform vec3 gCameraPosition;


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


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//MAIN
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
void main() {

	vec3 surfaceNormal = GetNormal();

	vec3 viewDirToSurface	= -normalize(passPosition - gCameraPosition);
	vec3 reflected			= reflect(viewDirToSurface, normalize(surfaceNormal));

	outColor = texture(gTexEnvironmentMap, reflected);
}