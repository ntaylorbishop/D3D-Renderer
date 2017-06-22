#version 430 core

uniform mat4 gModel;
uniform mat4 gView;
uniform mat4 gProj;
uniform vec4 gVertColor;

in vec3 inPosition;
in vec4 inColor;
in vec2 inUV0;
in vec3 inNormal;
in vec3 inTangent;
in vec3 inBitangent;

out vec4 passColor;
out vec2 passUV0;
out vec3 passPosition;
out vec3 passNormal;
out vec3 passTangent;
out vec3 passBitangent;


void main ( void ) {

	passUV0		= inUV0;
	passColor	= inColor;

	vec3 pos = inPosition;
	
	passPosition	= (vec4(pos, 1.f)			* gModel).xyz;
	passNormal		= (vec4(inNormal, 0.f)		* gModel).xyz;
	passTangent		= (vec4(inTangent, 0.f)		* gModel).xyz;
	passBitangent	= (vec4(inBitangent, 0.f)	* gModel).xyz;	
	passColor		= gVertColor;



	gl_Position		= vec4(pos, 1.0f) * gModel * gView * gProj;
}