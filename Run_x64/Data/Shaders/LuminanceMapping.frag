#version 430 core

layout (binding = 0) uniform sampler2D gTexSceneDiffuse;

in vec2 passUV0;
out vec4 outLuminance;


//---------------------------------------------------------------------------------------------------------------------------
void main() { 

	vec3 pBrightness	= texture2D(gTexSceneDiffuse, passUV0).rgb;
	float lum			= dot(pBrightness, vec3(0.2125, 0.7154, 0.0721));
	outLuminance = vec4(vec3(lum), 1.f);
}

//NOTE: EXPOSURE RANGE 1.6 to 6