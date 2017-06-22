#include "Game/ProbeManager.hpp"
#include "Game/TheGame.hpp"
#include "Engine/Scene/ClickableObject.hpp"
#include "Engine/Scene/PickRay.hpp"
#include "Engine/Renderer/ReflectionProbes/DynamicReflectionProbe.hpp"
#include "Engine/Renderer/ReflectionProbes/StaticReflectionProbeManager.hpp"
#include "Engine/Renderer/ReflectionProbes/StaticReflectionProbe.hpp"


STATIC ProbeManager* ProbeManager::s_pManager = nullptr;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//GET
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------------
STATIC ProbeManager* ProbeManager::Get() {

	if (s_pManager == nullptr) {
		s_pManager = new ProbeManager();
	}

	return s_pManager;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//UPDATE
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------------
void ProbeManager::UpdateSelection(float deltaSeconds) {

	if (TheGame::HasMouseInput() && InputSystem::GetMouseBtnDown(MOUSE_LEFT)) {
		std::vector<ClickableObject*> objs;

		Vector3 start = Vector3::ZERO;
		Vector3 end = Vector3::ZERO;
		CreatePickRay(objs, start, end);

		for (size_t i = 0; i < ClickableObject::s_sceneClickableObjs.size(); i++) {
			ClickableObject::s_sceneClickableObjs[i]->m_isSelected = false;
		}
		m_selectedProbe = nullptr;

		if (!objs.empty()) {
			objs[0]->m_isSelected = true;
			if (MODIFYING_STATIC) {
				m_selectedStaticProbe = (StaticReflectionProbe*)objs[0];
			}
			else {
				m_selectedProbe = (DynamicReflectionProbe*)objs[0];
			}
		}
	}
}


//---------------------------------------------------------------------------------------------------------------------------
void ProbeManager::UpdateSelectedProbe(float deltaSeconds) {

	if (m_showingAllProbeBounds) {

		if (MODIFYING_STATIC) {
			for (size_t i = 0; i < m_sProbes.size(); i++) {

				AABB3 radiusBox;
				radiusBox.mins = m_sProbes[i]->m_position + m_sProbes[i]->m_boxMins;
				radiusBox.maxs = m_sProbes[i]->m_position + m_sProbes[i]->m_boxMaxs;

				BeirusRenderer::DrawBoxLineMesh(radiusBox, RGBA::WHITE);
			}
		}
		else {
			for (size_t i = 0; i < m_probes.size(); i++) {

				AABB3 radiusBox;
				radiusBox.mins = m_probes[i]->m_position + m_probes[i]->m_boxMins;
				radiusBox.maxs = m_probes[i]->m_position + m_probes[i]->m_boxMaxs;

				BeirusRenderer::DrawBoxLineMesh(radiusBox, RGBA::WHITE);
			}
		}
	}
	else {
		
		if (MODIFYING_STATIC) {
			if (m_selectedStaticProbe != nullptr) {

				AABB3 radiusBox;
				radiusBox.mins = m_selectedStaticProbe->m_position + m_selectedStaticProbe->m_boxMins;
				radiusBox.maxs = m_selectedStaticProbe->m_position + m_selectedStaticProbe->m_boxMaxs;

				BeirusRenderer::DrawBoxLineMesh(radiusBox, RGBA::WHITE);
			}
		}
		else {
			if (m_selectedProbe != nullptr) {

				AABB3 radiusBox;
				radiusBox.mins = m_selectedProbe->m_position + m_selectedProbe->m_boxMins;
				radiusBox.maxs = m_selectedProbe->m_position + m_selectedProbe->m_boxMaxs;

				BeirusRenderer::DrawBoxLineMesh(radiusBox, RGBA::WHITE);
			}
		}
	}
}


//---------------------------------------------------------------------------------------------------------------------------
void ProbeManager::UpdateSelectedDynamicProbe(float deltaSeconds) {

	if (m_selectedProbe == nullptr) {
		return;
	}

	if (InputSystem::GetKeyDown(VK_LEFT)) {
		m_selectedProbe->m_position += Vector3(1.f, 0.f, 0.f) * PROBE_MOVE_SPEED * deltaSeconds;
	}
	else if (InputSystem::GetKeyDown(VK_RIGHT)) {
		m_selectedProbe->m_position -= Vector3(1.f, 0.f, 0.f) * PROBE_MOVE_SPEED * deltaSeconds;
	}
	else if (InputSystem::GetKeyDown(VK_UP)) {
		m_selectedProbe->m_position += Vector3(0.f, 0.f, 1.f) * PROBE_MOVE_SPEED * deltaSeconds;
	}
	else if (InputSystem::GetKeyDown(VK_DOWN)) {
		m_selectedProbe->m_position -= Vector3(0.f, 0.f, 1.f) * PROBE_MOVE_SPEED * deltaSeconds;
	}
	else if (InputSystem::GetKeyDown('Y')) {
		m_selectedProbe->m_position += Vector3(0.f, 1.f, 0.f) * PROBE_MOVE_SPEED * deltaSeconds;
	}
	else if (InputSystem::GetKeyDown('H')) {
		m_selectedProbe->m_position -= Vector3(0.f, 1.f, 0.f) * PROBE_MOVE_SPEED * deltaSeconds;
	}

	if (InputSystem::GetKeyDown('O')) {
		m_selectedProbe->m_boxMins.x -= PROBE_MOVE_SPEED * deltaSeconds;
		m_selectedProbe->m_boxMaxs.x += PROBE_MOVE_SPEED * deltaSeconds;
	}
	else if (InputSystem::GetKeyDown('L')) {
		m_selectedProbe->m_boxMins.x += PROBE_MOVE_SPEED * deltaSeconds;
		m_selectedProbe->m_boxMaxs.x -= PROBE_MOVE_SPEED * deltaSeconds;
	}

	if (InputSystem::GetKeyDown('I')) {
		m_selectedProbe->m_boxMins.y -= PROBE_MOVE_SPEED * deltaSeconds;
		m_selectedProbe->m_boxMaxs.y += PROBE_MOVE_SPEED * deltaSeconds;
	}
	else if (InputSystem::GetKeyDown('K')) {
		m_selectedProbe->m_boxMins.y += PROBE_MOVE_SPEED * deltaSeconds;
		m_selectedProbe->m_boxMaxs.y -= PROBE_MOVE_SPEED * deltaSeconds;
	}

	if (InputSystem::GetKeyDown('U')) {
		m_selectedProbe->m_boxMins.z -= PROBE_MOVE_SPEED * deltaSeconds;
		m_selectedProbe->m_boxMaxs.z += PROBE_MOVE_SPEED * deltaSeconds;
	}
	else if (InputSystem::GetKeyDown('J')) {
		m_selectedProbe->m_boxMins.z += PROBE_MOVE_SPEED * deltaSeconds;
		m_selectedProbe->m_boxMaxs.z -= PROBE_MOVE_SPEED * deltaSeconds;
	}
}


//---------------------------------------------------------------------------------------------------------------------------
void ProbeManager::UpdateSelectedStaticProbe(float deltaSeconds) {

	if (m_selectedStaticProbe == nullptr) {
		return;
	}

	if (InputSystem::GetKeyDown(VK_LEFT)) {
		m_selectedStaticProbe->m_position += Vector3(1.f, 0.f, 0.f) * PROBE_MOVE_SPEED * deltaSeconds;
	}
	else if (InputSystem::GetKeyDown(VK_RIGHT)) {
		m_selectedStaticProbe->m_position -= Vector3(1.f, 0.f, 0.f) * PROBE_MOVE_SPEED * deltaSeconds;
	}
	else if (InputSystem::GetKeyDown(VK_UP)) {
		m_selectedStaticProbe->m_position += Vector3(0.f, 0.f, 1.f) * PROBE_MOVE_SPEED * deltaSeconds;
	}
	else if (InputSystem::GetKeyDown(VK_DOWN)) {
		m_selectedStaticProbe->m_position -= Vector3(0.f, 0.f, 1.f) * PROBE_MOVE_SPEED * deltaSeconds;
	}
	else if (InputSystem::GetKeyDown('Y')) {
		m_selectedStaticProbe->m_position += Vector3(0.f, 1.f, 0.f) * PROBE_MOVE_SPEED * deltaSeconds;
	}
	else if (InputSystem::GetKeyDown('H')) {
		m_selectedStaticProbe->m_position -= Vector3(0.f, 1.f, 0.f) * PROBE_MOVE_SPEED * deltaSeconds;
	}

	if (InputSystem::GetKeyDown('O')) {
		m_selectedStaticProbe->m_boxMins.x -= PROBE_MOVE_SPEED * deltaSeconds;
		m_selectedStaticProbe->m_boxMaxs.x += PROBE_MOVE_SPEED * deltaSeconds;
	}
	else if (InputSystem::GetKeyDown('L')) {
		m_selectedStaticProbe->m_boxMins.x += PROBE_MOVE_SPEED * deltaSeconds;
		m_selectedStaticProbe->m_boxMaxs.x -= PROBE_MOVE_SPEED * deltaSeconds;
	}

	if (InputSystem::GetKeyDown('I')) {
		m_selectedStaticProbe->m_boxMins.y -= PROBE_MOVE_SPEED * deltaSeconds;
		m_selectedStaticProbe->m_boxMaxs.y += PROBE_MOVE_SPEED * deltaSeconds;
	}
	else if (InputSystem::GetKeyDown('K')) {
		m_selectedStaticProbe->m_boxMins.y += PROBE_MOVE_SPEED * deltaSeconds;
		m_selectedStaticProbe->m_boxMaxs.y -= PROBE_MOVE_SPEED * deltaSeconds;
	}

	if (InputSystem::GetKeyDown('U')) {
		m_selectedStaticProbe->m_boxMins.z -= PROBE_MOVE_SPEED * deltaSeconds;
		m_selectedStaticProbe->m_boxMaxs.z += PROBE_MOVE_SPEED * deltaSeconds;
	}
	else if (InputSystem::GetKeyDown('J')) {
		m_selectedStaticProbe->m_boxMins.z += PROBE_MOVE_SPEED * deltaSeconds;
		m_selectedStaticProbe->m_boxMaxs.z -= PROBE_MOVE_SPEED * deltaSeconds;
	}
}


//---------------------------------------------------------------------------------------------------------------------------
void ProbeManager::Update(float deltaSeconds) {

	for (size_t i = 0; i < m_sProbes.size(); i++) {
		m_sProbes[i]->Update(deltaSeconds);
	}

	UpdateSelection(deltaSeconds);
	UpdateSelectedProbe(deltaSeconds);

	if (InputSystem::GetKeyDown('M')) {
		CreateProbe();
	}

	if (InputSystem::GetKeyDown('N')) {
		m_showingAllProbeBounds = !m_showingAllProbeBounds;
	}

	if (m_probesLoaded) {
		StaticReflectionProbeManager::Get()->BindProbesToShader("Water");
	}

	if (MODIFYING_STATIC) {
		UpdateSelectedStaticProbe(deltaSeconds);
	}
	else {
		UpdateSelectedDynamicProbe(deltaSeconds);
	}
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//RENDER
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------------
void ProbeManager::Render() const {

	for (size_t i = 0; i < m_sProbes.size(); i++) {
		m_sProbes[i]->Render();
	}
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//LOAD SAVE
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------------
void ProbeManager::SaveOutAllProbes(const String& path, const String& name) {

	for (size_t i = 0; i < m_probes.size(); i++) {
		String writeName = StringUtils::Stringf("_%u", i);
		m_probes[i]->WriteToFile(path, name + writeName);
	}
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//STRUCTORS
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------------
ProbeManager::ProbeManager() {
	
	Console::RegisterCommand("save_probes", "Save out all scene probes.", ProbeManager::SaveOutProbes);
	Console::RegisterCommand("load_probes", "Load static probes from directory.", ProbeManager::LoadProbes);
	Console::RegisterCommand("bind_probes", "Bind probes to BlinnPhong shader.", ProbeManager::BindProbes);

	//Console::RunCommand("load_probes realOne");
	//Console::RunCommand("bind_probes");
}


//---------------------------------------------------------------------------------------------------------------------------
ProbeManager::~ProbeManager() {

}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//CREATE PROBE
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------------
void ProbeManager::CreateProbe() {


	Vector3 posInFrontOfCam = Vector3(0.f, 0.f, 1.f);
	Matrix4 view = *BeirusRenderer::GetViewMatrix();

	Vector3 posToSpawn = (Vector4(posInFrontOfCam, 1.f) * view.Inverse()).xyz();

	DynamicReflectionProbe* probeToAdd = DynamicReflectionProbe::Create(posToSpawn, TheGame::GetScene());
	m_probes.push_back(probeToAdd);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//CONSOLE COMMANDS
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------------
STATIC void ProbeManager::SaveOutProbes(Command& args) {

	String folderName;
	args.GetNextString(folderName);

	String folder = "Data/Probes/" + folderName + "/";
	FileUtils::CreateDirectoryIfDoesntExist(folder);

	s_pManager->SaveOutAllProbes(folder, folderName);
}


//---------------------------------------------------------------------------------------------------------------------------
STATIC void ProbeManager::LoadProbes(Command& args) {

	String folder;
	args.GetNextString(folder);

	String baseFolder = "Data/Probes/";
	ProbeManager::Get()->m_probesLoaded = true;

	StaticReflectionProbeManager::Get()->LoadProbesInDirectory(baseFolder + folder + "/", TheGame::GetScene());
}


//---------------------------------------------------------------------------------------------------------------------------
STATIC void ProbeManager::BindProbes(Command& args) {

	StaticReflectionProbeManager::Get()->BindProbesToShader("Water");
}