#version 430 core

layout (binding = 0) uniform sampler2D	gTexDiffuse;

uniform float		gExposure;
uniform float		gWhitePoint = 1.f;

uniform uint gDebugDraw;

in vec2 passUV0;

layout (location = 0) out vec4 outColor;

//---------------------------------------------------------------------------------------------------------------------------
vec4 FilmicTonemapping(vec3 linColor) {

	vec3 x = linColor;
	float A = 0.15;
	float B = 0.50;
	float C = 0.10;
	float D = 0.20;
	float E = 0.02;
	float F = 0.30;

	vec3 tmColor = ((x * (A * x + C * B) + D * E) / (x * (A * x + B) + D * F)) - E / F;
	return vec4(tmColor, 1.f);
}


//---------------------------------------------------------------------------------------------------------------------------
vec4 ReinhardtTonemapping(vec3 linColor) {

	return vec4(linColor / (vec3(linColor) + 1.f), 1.f);
}


//---------------------------------------------------------------------------------------------------------------------------
void main() { 

	const vec3 W = vec3(1.5f);

	//FILMIC TONEMAPPING
	vec3 linColor	= texture(gTexDiffuse, passUV0).rgb;
	vec4 reinhardt	= ReinhardtTonemapping(linColor * gExposure);
	vec4 linColor4	= vec4(linColor, 1.f) * gExposure;
	vec4 tmColor	= FilmicTonemapping(linColor * gExposure) / FilmicTonemapping(W);

	//vec4 ntmColor	= vec4(linColor * gExposure, 1.f) / vec4(vec3(gWhitePoint), 1.f);

	
	if(gDebugDraw == 0) { //REINHARDT
		outColor = vec4(reinhardt.xyz, 1.f);
	}
	else if(gDebugDraw == 1) { //LINEAR
		outColor = linColor4;
	}
	else if(gDebugDraw == 2) { //FILMIC
		outColor = tmColor;
	}
}