#include "Game/TheGame.hpp"
#include "Game/TheApp.hpp"
#include "Game/RHI/DeviceWindow.hpp"

TheGame* TheGame::s_theGame = nullptr;


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//STATIC INIT DESTROY
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------------
STATIC void TheGame::Initialize(HINSTANCE applicationInstanceHandle, int nCmdShow) {

	s_theGame = new TheGame(applicationInstanceHandle, nCmdShow);
	BeirusEngine::RegisterUpdateCallback(MakeDelegate(s_theGame, &TheGame::Update));
}


//---------------------------------------------------------------------------------------------------------------------------
STATIC void TheGame::Shutdown() {

	BeirusEngine::UnregisterUpdateCallback(MakeDelegate(s_theGame, &TheGame::Update));
	delete s_theGame;
}



STATIC TheGame* TheGame::Get() {

	if (s_theGame == nullptr) {
		ERROR_AND_DIE("nuuu");
	}

	return s_theGame;
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//STRUCTORS INITIALIZATION
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------------
TheGame::TheGame(HINSTANCE applicationInstanceHandle, int nCmdShow)
{
	RHIDeviceWindow::Initialize(applicationInstanceHandle, nCmdShow);
}


//---------------------------------------------------------------------------------------------------------------------------
TheGame::~TheGame() {
	RHIDeviceWindow::Destroy();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//SCENE INITIALIZATION
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//TICK
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------------
void TheGame::Update(float deltaSeconds) {

}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//RENDER
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------------
STATIC void TheGame::Render() {

	RHIDeviceWindow::ClearAndPresent();
}

