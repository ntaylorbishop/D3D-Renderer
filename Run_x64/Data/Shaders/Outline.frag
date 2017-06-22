#version 430

uniform vec3 gViewDir;
uniform mat4 gView;

in vec2 passUV0;
in vec3 passPosition;
in vec4 passColor;
in vec3 passNormal;
in vec3 passTangent;
in vec3 passBitangent;

layout (location = 0) out vec4	outColor;


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//MAIN
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------------
void main() {

	vec3 viewDir = normalize(passPosition);

	float d = abs(dot(viewDir, passNormal));
	if(d < 0.2) {
		outColor = vec4(1.f, 1.f, 1.f, 1.f);
	}
	else {
		outColor = vec4(0.f);
	}
}