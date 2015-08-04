#pragma once

#include "Doge.h"

#include <vector>
#include <string>
#include <algorithm>
#include <iterator>


#include "Light.h"
#include "Shader.h"
#include "Mesh.h"
#include "PipelineState.h"
#include "Camera.h"

#include "Utils\timer.h"

#include "RenderPass.h"
#include "RenderGraph.h"
#include "RenderableObject.h"
#include "ViewPoint.h"
#include "Visibility.h"

#include "SystemCBuffersManager.h"

class ShadowEmittingLight
{
public:

};

const RenderGraph& GetDefaultRenderGraph_ShadowMap();

const RenderGraph& GetDefaultRenderGraph_ScenePhoto();

#define G_RENDERER2 ((*Renderer2::GetInstance()))

class Renderer2
{
public:
	static Renderer2* GetInstance();

	void RegisterLight(const Light& light);
	void RegisterDirLight(const DirLight& dirLight);

	void RegisterShadowEmittingLight(const ShadowEmittingLight& light);
	void RegisterRenderableObject(const RenderableObject& object);
	void RegisterViewPoint(const ViewPoint& viewPoint);

	const std::vector<const ShadowEmittingLight*>& GetShadowEmittingLights();
	const std::vector<const RenderableObject*>& GetRenderableObjects();
	const std::vector<const ViewPoint*>& GetViewPoints();

	bool IsViewVisible(const RenderableObject* pObject);

	void PerformVisibilitySorting();

	std::vector<const RenderableObject*> GetObjectsWithVisibility(uint32_t visibilityBitfield) const;
	std::vector<const RenderableObject*> GetObjectsWithVisibility(EVisibility eVisibility) const;

	void DrawScreenQuadById() const;
	void DrawScreenQuadByVertices() const;

	void Flush();

	void DeployRenderGraph(const RenderGraph& renderGraph) const;
	void RenderPassWithInput(const RenderPass& renderPass) const;

	void CleanUp();

	void SetDimensions(int width, int height);

	int GetWidth() const;
	int GetHeight() const;
	Dim2D GetDim() const;

	GLuint GetDefaultGLDepthStencilId() const;
	GLuint GetDefaultPointSamplerId() const;
	GLuint GetDefaultLinearSamplerId() const;
	GLuint GetDefaultDiffuseSamplerId() const;
	GLuint GetDefaultAnisotropicSamplerId() const;

	void AddOnRenderResizeListener(RenderGraph* renderGraph);

private:
	Renderer2();

private:
	struct GLDependentData
	{
		Mesh m_screenQuad;
		VAO m_nullVAO;
	};

	std::unique_ptr<GLDependentData> m_glDepData;

	int m_width;
	int m_height;

	std::vector<const Light*> m_lights;

	std::vector<const DirLight*> m_dirLights;

	std::vector<const ShadowEmittingLight*> m_shadowEmittingLights;

	std::vector<const ViewPoint*> m_viewPoints;

	std::vector<const RenderableObject*> m_renderableObjects;

	std::vector<const RenderableObject*> m_byMetaVisibilityRenderableObjects[EVisibility_COUNT];

	bool m_bVisibilityPerformed = false;

	Timer m_rendererTimer;

	Camera m_mainCamera;

	GLuint m_glDefaultDepthRenderBufferId;
	GLuint m_glPointSamplerId;
	GLuint m_glLinearSamplerId;
	GLuint m_glDiffuseSamplerId;
	GLuint m_glAnisotropicSamplerId;

	std::vector<RenderGraph*> m_onRenderResizeListeners;

public:
	bool InitGL();

	void InitializeGLDependentData();

	void InitializeDemoData();

	void initializeGL();

	void resizeGL(int width, int height);

	vec3 GenRandomVec3() const;

	void paintGL();

	void reloadShader();

	void rotateCamera(int mouse_x_rel, int mouse_y_rel);
	void translateCamera(int mouse_x_rel, int mouse_y_rel, int mouse_z_rel);

	void onKeyPress(int qt_key);

	void toggleWireframe();
	void setFinalFboTarget(int targetIndex);
	void setTesselationFactor(float tesselation_factor);
	void setDispFactor(float disp_factor);
	void setNbSamples(int nb_samples);
	void setRoughnessOffset(double roughnessOffset);
	void setFresnel0(double fresnel0);
	void loadModel(const std::string &model_path);
	void loadLEADRTexture(const std::string & leadr);
	void loadTexture(const std::string & texture);
	void loadDispMap(const std::string & disp_map);
	void loadNormalMap(const std::string & normal_map);
	void loadEnvTexture(const std::string & env_path);
	void loadIrradianceMap(const std::string & irr_path);
	void setStandardRendering();
	void setWireframeRendering();
	void setDepthRendering();
	void setNormalRendering();
	void setTexcoordRendering();
	void setBRDF(int brdf_choice);
	void setNormalMode(bool normal_mode);
	void setFilteringMode(bool enabled);
	void setDiffuseEnabled(bool enabled);
	void setSpecularDirectEnabled(bool enabled);
	void setSpecularEnvEnabled(bool enabled);
	void setDiffuseDirectEnabled(bool enabled);
	void setDiffuseEnvEnabled(bool enabled);
};

