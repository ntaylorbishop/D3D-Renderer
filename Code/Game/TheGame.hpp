#pragma once

#include "Engine/General/Core/BeirusEngine.hpp"
#include "Engine/General/Core/EngineCommon.hpp"
#include "Engine/Renderer/SpriteRenderer/TheSpriteRenderer.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/Cameras/Camera3D.hpp"
#include "Engine/Scene/D3D11Scene.hpp"
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

	void CreateShaderProgram();
	void CreateMesh();

	Camera3D m_playerCamera;
	D3D11Scene* m_scene;
	Matrix44 m_model;

	static TheGame* s_theGame;
};