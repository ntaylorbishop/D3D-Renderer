#pragma once

#include "Engine/General/Core/BeirusEngine.hpp"

class TheGame {
public:
	//INIT DESTROY
	static void Initialize();
	static void Shutdown();

	static TheGame* Get();

	//UPDATE
	void Update(float deltaSeconds);

	//RENDER
	void Render();

private:
	//STRUCTORS INIT
	TheGame();
	~TheGame();

	static TheGame* s_theGame;
};