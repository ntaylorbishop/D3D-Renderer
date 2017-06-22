#version 430 core

layout (binding = 0) uniform sampler2D gTexDiffuse;
uniform bool gIsUsingTex;

in vec2 passUV0;
in vec4 passColor;

out vec4 outColor;

void main ( void ) {
	vec4 diffuse = texture(gTexDiffuse, passUV0);

	if(gIsUsingTex) {
		outColor = vec4(vec3(diffuse.r), 1.f);
	}
	else {
		outColor = vec4(vec3(diffuse.r), 1.f);
	}
}