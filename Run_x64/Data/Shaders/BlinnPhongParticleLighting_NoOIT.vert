#version 430 core

uniform mat4 gModel;
uniform mat4 gView;
uniform mat4 gProj;
uniform vec4 gVertColor;

//FOR SPRITESHEET PARTICLES
uniform uint gSpriteIndex;
uniform vec2 gTileSize;

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

	uint x = gSpriteIndex % uint(gTileSize.x);
	uint y = gSpriteIndex / uint(gTileSize.y);

	vec2 uvPerTile	= vec2(1.f / gTileSize.x, 1.f / gTileSize.y);
	vec2 tileInds	= vec2(float(x), float(y));

	passUV0.x = uvPerTile.x * tileInds.x + inUV0.x * uvPerTile.x;
	passUV0.y = uvPerTile.y * tileInds.y + inUV0.y * uvPerTile.y;

	passColor = inColor;

	vec4 pos = vec4(inPosition, 1.0f);
	
	passPosition	= (vec4(inPosition, 1.f)	* gModel).xyz;
	passNormal		= (vec4(inNormal, 0.f)		* gModel).xyz;
	passTangent		= (vec4(inTangent, 0.f)		* gModel).xyz;
	passBitangent	= (vec4(inBitangent, 0.f)	* gModel).xyz;	
	passColor		= gVertColor;

	gl_Position		= vec4(inPosition, 1.0f) * gModel * gView * gProj;
}