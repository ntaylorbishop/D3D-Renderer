#version 430 core

layout(binding = 0) uniform sampler2D gTexSSAO;

in vec2 passUV0;
out vec4 outColor;

const int BLUR_SIZE = 4;


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//MAIN
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------------
void main() {
   
	vec2 texelSize	= 1.0 / vec2(textureSize(gTexSSAO, 0));
	float result	= 0.0;
	vec2 hlim		= vec2(float(-BLUR_SIZE) * 0.5 + 0.5);
	
	for (int i = 0; i < BLUR_SIZE; ++i) {
   
		for (int j = 0; j < BLUR_SIZE; ++j) {

			vec2 offset  = (hlim + vec2(float(i), float(j))) * texelSize;
			result		+= texture2D(gTexSSAO, passUV0 + offset).r;
		}
	}

	vec3 outOcc = vec3(result / float(BLUR_SIZE * BLUR_SIZE));
	outColor	= vec4(outOcc, 1.f);
}