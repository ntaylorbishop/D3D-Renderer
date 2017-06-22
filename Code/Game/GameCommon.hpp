#pragma once

#include "Engine/General/Core/EngineCommon.hpp"
#include "Engine/Renderer/SpriteRenderer/Sprite.hpp"

//---------------------------------------------------------------------------------------------------------------------------
//GLOBAL DEFINES
//---------------------------------------------------------------------------------------------------------------------------
#define IS_SOUND_ENABLED true
#define IS_SIM_LAG_LOSS_ENABLED true
//---------------------------------------------------------------------------------------------------------------------------
//GAME DEFINES
//---------------------------------------------------------------------------------------------------------------------------

#define AXIS_FORWARD Vector2(0.f, 1.f)
#define AXIS_BACKWARD Vector2(0.f, -1.f)
#define AXIS_LEFT Vector2(-1.f, 0.f)
#define AXIS_RIGHT Vector2(1.f, 0.f)

#define NORTH     IntVector2( 0,  1) 
#define NORTHEAST IntVector2( 1,  1) 
#define EAST      IntVector2( 1,  0) 
#define SOUTHEAST IntVector2( 1, -1) 
#define SOUTH     IntVector2( 0, -1) 
#define SOUTHWEST IntVector2(-1, -1) 
#define WEST      IntVector2(-1,  0) 
#define NORTHWEST IntVector2(-1,  1) 

//---------------------------------------------------------------------------------------------------------------------------
//CHARACTER DEFINES
//---------------------------------------------------------------------------------------------------------------------------
#define CHARACTER_SPEED_FLAMER  3.f
#define CHARACTER_SPEED_BUCK    3.f
#define CHARACTER_SPEED_MCGEE   3.f
#define CHARACTER_SPEED_SLEEPER 3.f

#define RADIUS_FLAMER  0.5f
#define RADIUS_BUCK    0.5f
#define RADIUS_MCGEE   0.5f
#define RADIUS_SLEEPER 0.5f

//---------------------------------------------------------------------------------------------------------------------------
//ENUMS
//---------------------------------------------------------------------------------------------------------------------------
enum eTileType;
enum eCharacterType {
	CHARACTER_BUCK = 0,
	CHARACTER_FLAMER,
	CHARACTER_MCGEE,
	CHARACTER_SLEEPER
};

enum eTeamID {
	TEAM_RED = 0,
	TEAM_BLUE
};

#define TILE_SIZE 1.72f
#define DEFAULT_MAP "Data/Maps/Map1.png"

//---------------------------------------------------------------------------------------------------------------------------
//UI
//---------------------------------------------------------------------------------------------------------------------------
#define UI_TOP_BOTTOM_BAR_HEIGHT 100.f

//---------------------------------------------------------------------------------------------------------------------------
//FUNCTIONS
//---------------------------------------------------------------------------------------------------------------------------
namespace GameCommon { }