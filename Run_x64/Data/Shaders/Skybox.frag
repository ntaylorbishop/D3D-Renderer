#version 430 core

layout (binding = 0) uniform samplerCube gCubemap;
uniform mat4 gView;


in vec3 passUV0;
in vec4 passColor;
in vec3 passNormal;

out vec4 outColor;
out vec4 outNormals;


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//MAIN
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------------
void main() {

	vec4 diffuse = texture(gCubemap, passUV0);
	outColor = diffuse;

	vec3 normal = vec4(vec4(passNormal, 0.f) * gView).xyz;

	normal = -normal;

	normal = (normal + vec3(1.f)) / vec3(2.f);
	outNormals = vec4(normal, 1.f);
}