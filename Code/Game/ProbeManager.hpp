#pragma once

#include "Engine/General/Core/EngineCommon.hpp"
#include "Engine/Console/Command.hpp"

class DynamicReflectionProbe;
class StaticReflectionProbe;

const float PROBE_MOVE_SPEED = 2.f;

const bool MODIFYING_STATIC = true;

class ProbeManager {
public:
	//GET
	static ProbeManager* Get();

	//UPDATE
	void UpdateSelectedDynamicProbe(float deltaSeconds);
	void UpdateSelectedStaticProbe(float deltaSeconds);
	void UpdateSelection(float deltaSeconds);
	void UpdateSelectedProbe(float deltaSeconds);
	void Update(float deltaSeconds);
	
	//RENDER
	void Render() const;

	//LOAD SAVE
	void SaveOutAllProbes(const String& path, const String& name);

private:
	//STRUCTORS
	ProbeManager();
	~ProbeManager();

	void CreateProbe();


	DynamicReflectionProbe* m_selectedProbe = nullptr;
	std::vector<DynamicReflectionProbe*> m_probes;
	bool m_showingAllProbeBounds = false;
	bool m_probesLoaded = false;

	StaticReflectionProbe* m_selectedStaticProbe = nullptr;
	std::vector<StaticReflectionProbe*> m_sProbes;


	static ProbeManager* s_pManager;

	//CONSOLE COMMANDS
	static void SaveOutProbes(Command& args);
	static void LoadProbes(Command& args);
	static void BindProbes(Command& args);
};