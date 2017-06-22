#version 430 core

layout(binding = 0) uniform sampler2D gTexNormal;
uniform mat4 gView;

in vec2 passUV0;
in vec3 passPosition;
in vec4 passColor;
in vec3 passNormal;
in vec3 passTangent;
in vec3 passBitangent;

layout (location = 0) out vec4 outNormal;


//---------------------------------------------------------------------------------------------------------------------------
vec3 GetViewSpaceNormal() {

	vec3 tangent		= passTangent;
	vec3 bitangent		= passBitangent;
	vec3 surfaceNormal	= cross(tangent, bitangent);
	mat3 tbn			= mat3(tangent, bitangent, passNormal);
	tbn					= transpose(tbn);

	vec3 normal			= texture(gTexNormal, passUV0).xyz;
	normal				= normal * vec3(2.f, 2.f, 1.f) - vec3(1.f, 1.f, 0.f);	
	normal				= normal * tbn;
	normal				= (vec4(normal, 0.f) * gView).xyz;
	normal				= normalize(normal);
	normal				= (normal + vec3(1.f)) / vec3(2.f);

	return normal;
}


//---------------------------------------------------------------------------------------------------------------------------
void main() {
	
	outNormal = vec4(GetViewSpaceNormal(), 1.f);
}
