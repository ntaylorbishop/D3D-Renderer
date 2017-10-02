#pragma once

#include "Game/TheApp.hpp"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//DRIVER
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int WINAPI WinMain(HINSTANCE applicationInstanceHandle, HINSTANCE prevAppInstanceHandle, LPSTR commandLineString, int nCmdShow) {

	//_CrtSetDbgFlag(_CRTDBG_CHECK_ALWAYS_DF);

	UNREFERENCED_PARAMETER(prevAppInstanceHandle);
	UNREFERENCED_PARAMETER(commandLineString);

	TheApp::Initialize(applicationInstanceHandle, nCmdShow);

	while (!InputSystem::HasQuit())
		TheApp::RunFrame();

	TheApp::Shutdown();
	return 0;
}