#pragma once

#include "Engine/Math/AABB2.hpp"
#include "Game/DebugSceneTypes/DebugSceneType.hpp"

class Framebuffer;
class TextureBuffer;
class Texture;
class Material;

class SceneTypeForwardPlus : public DebugSceneType {
public:
	//STRUCTORS
	SceneTypeForwardPlus();
	~SceneTypeForwardPlus();

	//UPDATE
	void HandleLeftRight();
	void HandleUpDown();
	VIRTUAL void Update(float deltaSeconds);
	
	//RENDER
	void RenderToLightHeatmap() const;
	void RenderLightHeatmap() const;
	void RenderText() const;
	VIRTUAL void Render() const;


private:

	static Texture*			m_lightHeatmap;
	static Framebuffer*		m_heatmapFBO;
	static TextureBuffer*	m_lightHeatmapBuffer;
	static Material*		m_heatmapGenMat;
	static Material*		m_heatmapDrawMat;

	bool					m_isHeatmapEnabled		= false;
	uint					m_currNumLightsInScene	= 0;
};