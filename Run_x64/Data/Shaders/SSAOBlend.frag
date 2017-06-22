#version 430 core

uniform sampler2D	gTexDiffuse;
uniform sampler2D	gTexDepth;
uniform sampler2D	gTexNormals;
uniform sampler2D	gTexSSAO;
uniform int			gWhichView;
uniform float		gNearZ;
uniform float		gFarZ;
uniform float		gExposure;
uniform float		gGamma;

in vec2 passUV0;

out vec4 outColor;

void main() { 

	//REINHARD TONE-MAPPING		  
	//FILMIC TONEMAPPING
	vec3 linColor = texture(gTexDiffuse, passUV0).rgb;
	linColor *= vec3(gExposure);

	float r = max(0.f, linColor.r - 0.004f);
	float g = max(0.f, linColor.g - 0.004f);
	float b = max(0.f, linColor.b - 0.004f);
	vec3 x = vec3(r, g, b);

	vec4 tonemappedColor = vec4((x * (6.2f * x + 0.5f)) / (x * (6.2f * x + 1.7f) + 0.06f), 1.f);


	//vec3 hdrColor			= texture(gTexDiffuse, passUV0).rgb;
	////vec3 mapped			= vec3(1.0) - exp(-hdrColor * gExposure);
	////mapped				= pow(mapped, vec3(1.0 / gGamma));
	//vec4 tonemappedColor	= vec4(hdrColor, 1.f);

	//SSAO VIEW
	if(gWhichView == 1) {
		vec4 ssaoFactor = texture2D(gTexSSAO, passUV0);
		outColor		= tonemappedColor * ssaoFactor;
	}

	//NO SSAO VIEW
	else if(gWhichView == 0) {
		outColor = tonemappedColor;
	}

	//DRAW DEPTH VIEW
	else if(gWhichView == 2) {
		float depth			= texture2D(gTexDepth, passUV0).r;
		float z				= (gFarZ * gNearZ) / (gFarZ + gNearZ - depth * (gFarZ - gNearZ));
		float z_ZeroToOne	= ((z - gNearZ) / (gFarZ - gNearZ));
		outColor			= vec4(vec3(z_ZeroToOne), 1.f);
	}

	//outColor = texture2D(gTexSSAO, passUV0);
}