#version 430 core

uniform mat4 gModel;
uniform mat4 gProj;

in vec3 inPosition;
in vec4 inColor;
in vec2 inUV0;
in vec3 inNormal;
in vec3 inTangent;
in vec3 inBitangent;

void main( void ) {

	gl_Position = vec4(inPosition, 1.f) * gModel * gProj;
}