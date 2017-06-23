#pragma once

#include "Engine/General/Core/BeirusEngine.hpp"
#include "Engine/General/Core/EngineCommon.hpp"
#include "Engine/Renderer/SpriteRenderer/TheSpriteRenderer.hpp"
#include "Engine/Input/InputSystem.hpp"
#include <windows.h>

class TheGame {
public:
	//INIT DESTROY
	static void Initialize(HINSTANCE applicationInstanceHandle, int nCmdShow);
	static void Shutdown();

	static TheGame* Get();

	//UPDATE
	void Update(float deltaSeconds);

	//RENDER
	void Render();

private:
	//STRUCTORS INIT
	TheGame(HINSTANCE applicationInstanceHandle, int nCmdShow);
	~TheGame();

	static TheGame* s_theGame;
};