#pragma once

#include "Engine/Math/AABB2.hpp"
#include "Game/DebugSceneTypes/DebugSceneType.hpp"

class ShadowCastingLight3D;
class ShadowSphere;

const float SHADOW_MOVE_SPEED = 10.f;

class SceneTypeShadows : public DebugSceneType {
public:
	SceneTypeShadows();
	~SceneTypeShadows();

	//UPDATE
	void HandleToggleMoveLight();
	void MoveLight(float deltaSeconds);
	void UpdateSpheres(float deltaSeconds);
	void HandleAttachLight();

	VIRTUAL void Update(float deltaSeconds);

	//RENDER
	VIRTUAL void Render() const;


private:

	ShadowCastingLight3D* m_shadowLight = nullptr;
	bool m_isLightMoving = false;
	float m_age = 0.f;

	std::vector<ShadowSphere*> m_spheres;
	static ShadowSphere* s_lightSphere;
	bool m_isAttached = false;
};