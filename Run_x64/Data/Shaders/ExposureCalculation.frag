#version 430 core

layout (binding = 0) uniform sampler2D gPreviousExposure;

uniform float gTargetExposure;
uniform float gMinExposure;
uniform float gMaxExposure;
uniform float gWhitePoint;
uniform float gDeltaSeconds;

const float SIGMA = 25.f;


in vec2 passUV0;
out vec4 outExposure;


//---------------------------------------------------------------------------------------------------------------------------
float CalculateExposure() {

	float prevExposure = texture2D(gPreviousExposure, vec2(1.f, 1.f)).r;

	float targetExposure	= 0.5f / gTargetExposure;
	float newExposure		= prevExposure + (targetExposure - prevExposure) * (.1);
	return newExposure;
}


//---------------------------------------------------------------------------------------------------------------------------
void main() { 

	float exposure	= CalculateExposure();
	outExposure		= vec4(vec3(gTargetExposure), 1.f);
}

//NOTE: EXPOSURE RANGE 1.6 to 6