#pragma once

#include "Engine/General/Core/EngineCommon.hpp"

class DebugSceneType {
public:
	DebugSceneType() { }
	virtual ~DebugSceneType() { }

	virtual void Update(float deltaSeconds) = 0;
	virtual void Render() const = 0;

private:

};