#include "Renderer2.h"

#include <QKeyEvent>

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

static PipelineState GetZPrepass_PipelineState()
{
	PipelineState ZPrepass_pipelineState;

	DepthStencilState& ds = ZPrepass_pipelineState.m_depthStencilState;

	ds.bDepthTest = true;
	ds.depthTest.depthFunc = EGlFunc_LEQUAL;

	ds.bDepthMask = true;

	ds.bStencilTest = false;

	RasterizerState& rs = ZPrepass_pipelineState.m_rasterizerState;

	// WARNING, THIS ONE MIGHT BETTER BE THE MESH/MATERIAL RESPONSIBILITY
	rs.bCullFace = true;
	rs.cullFace.cullMode = EGlCullMode_FRONT;
	rs.cullFace.cullOrientation = EGlCullOrientation_CW;

	BlendState& bs = ZPrepass_pipelineState.m_blendState;

	bs.bBlend = false;
	bs.blendMask = { false, false, false, false }; // COLOR WRITE DISABLED

	return ZPrepass_pipelineState;
}

static RenderPassDesc ZPRepass_3DPassDesc{
	{}, 
	RenderPassFBODesc{ 
		true, 
		true, 
		{ 
			EFBOTargetSizeMode_USE_SCREEN_SIZE, 
			{}, 
			{},
			EFBOTargetBeforeResizeModifier_NONE,
			{}
		},
		std::vector<FBOColorEntryDesc>{}, 
		FBODepthStencilEntryDesc{ -666, EFBOTargetClearMode_CLEAR_BEFORE_USE, 1.f }
	},
	// Shader
	{
		// Active stages
		{ true, false, false, false, false },
		// Stages filenames
		{ "simple3D.vert", "", "", "", "" }
	},

	// Pipeline state
	GetZPrepass_PipelineState(),

	// Input type and its visibility if any
	EPassInputType_3DScene,
	EVisibility_Any,

	// Is lighting
	false
};

static PipelineState GetSimple3D_PipelineState()
{
	PipelineState simple3D_pipelineState;

	DepthStencilState& ds = simple3D_pipelineState.m_depthStencilState;

	ds.bDepthTest = true;
	ds.depthTest.depthFunc = EGlFunc_LEQUAL;

	ds.bDepthMask = false;

	ds.bStencilTest = false;

	RasterizerState& rs = simple3D_pipelineState.m_rasterizerState;

	// WARNING, THIS ONE MIGHT BETTER BE THE MESH/MATERIAL RESPONSIBILITY
	rs.bCullFace = true;
	rs.cullFace.cullMode = EGlCullMode_FRONT;
	rs.cullFace.cullOrientation = EGlCullOrientation_CW;

	BlendState& bs = simple3D_pipelineState.m_blendState;

	bs.bBlend = false;
	bs.blendMask = { true, true, true, true }; // COLOR WRITE ALL

	return simple3D_pipelineState;
}

static RenderPassDesc Simple3D_3DPassDesc{
	{}, 
	RenderPassFBODesc{ 
		false, true, 
		{
			EFBOTargetSizeMode_USE_SCREEN_SIZE,
			{},
			{},
			EFBOTargetBeforeResizeModifier_NONE,
			{}
		},
		std::vector<FBOColorEntryDesc>{ 
			{ "SceneRender", -666, EFBOTargetClearMode_CLEAR_BEFORE_USE, { 0.f, 1.f, 1.f, 0.f } }
		}, 
			FBODepthStencilEntryDesc{ -666, EFBOTargetClearMode_NO_CLEAR, {} }
	},
	// Shader
	{
		// Active stages
		{ true, false, false, false, true },
		// Stages filenames
		{ "simple3D.vert", "", "", "", "simple3D.frag" }
	},

		// Pipeline state
		GetSimple3D_PipelineState(),

		// Input type and its visibility if any
		EPassInputType_3DScene,
		EVisibility_Any,

		// Is lighting
		true
};

static PipelineState GetScreenSpacePass_PipelineState()
{
	PipelineState screenSpacePass_pipelineState;

	DepthStencilState& ds = screenSpacePass_pipelineState.m_depthStencilState;

	ds.bDepthTest = false;
	ds.bDepthMask = false;
	ds.bStencilTest = false;

	RasterizerState& rs = screenSpacePass_pipelineState.m_rasterizerState;

	rs.bCullFace = true;
	rs.cullFace.cullMode = EGlCullMode_FRONT;
	rs.cullFace.cullOrientation = EGlCullOrientation_CW;

	BlendState& bs = screenSpacePass_pipelineState.m_blendState;

	bs.bBlend = false;
	bs.blendMask = { true, true, true, true }; // COLOR WRITE ALL

	return screenSpacePass_pipelineState;
}

static ShaderDesc GetScreenSpacePass_Downscale4x4_ShaderDesc()
{
	return
	{
		// Active stages
		{ true, false, false, false, true },
		// Stages filenames
		{ "screenQuadById.vert", "", "", "", "downscale4x4.frag" }
	};
}

static RenderPassDesc Downscale2x2_ScreenPassDesc{
	std::vector<RenderPassTextureInputDesc>{
		{0, "SceneRender", -666}
	},
		RenderPassFBODesc{
		false, true,
		{
			EFBOTargetSizeMode_USE_SCREEN_SIZE,
			{},
			{},
			EFBOTargetBeforeResizeModifier_NONE,
			{}
		},
		std::vector<FBOColorEntryDesc>{
			{ "SceneRenderDownscaled2x2", -666, EFBOTargetClearMode_NO_CLEAR, {} }
		},
			FBODepthStencilEntryDesc{ -666, EFBOTargetClearMode_NO_CLEAR, {} }
	},
	// Shader
	{
		// Active stages
		{ true, false, false, false, true },
		// Stages filenames
		{ "screenQuadById.vert", "", "", "", "downscale2x2.frag" }
	},

	// Pipeline state
	GetScreenSpacePass_PipelineState(),

	// Input type and its visibility if any
	EPassInputType_ScreenQuadById,
	EVisibility_Any,

	// Is lighting
	false
};

static RenderPassDesc GetStandardIntermediateScreenSpaceDesc(
	EFBOTargetSizeMode eStandardDownscaleMode, EFBOTargetBeforeResizeModifier eStandardPreDownscaleModifier,
	const std::string& strFragmentShader)
{
	
	DogeAssert(eStandardDownscaleMode != EFBOTargetSizeMode_CUSTOM_SIZE &&
		eStandardDownscaleMode != EFBOTargetSizeMode_USE_PREVIOUS_DOWNSCALED_CUSTOM &&
		eStandardDownscaleMode != EFBOTargetSizeMode_USE_SCREEN_DOWNSCALED_CUSTOM);

	DogeAssert(eStandardPreDownscaleModifier != EFBOTargetBeforeResizeModifier_ROUND_NEXT_CUSTOM);

	return{
		/*inTextures : */		std::vector<RenderPassTextureInputDesc>{{0, "", -666}},
		/*FBODesc : */			RenderPassFBODesc{ 
			/*useDefaultColor : */	false, 
			/*useDefaultDepth : */	true,
			/*fboSizeHeuristic : */	{
				eStandardDownscaleMode, {}, {},
				eStandardPreDownscaleModifier, {}
			},
			/*colorTargets : */		std::vector<FBOColorEntryDesc>{{ "", -666, EFBOTargetClearMode_NO_CLEAR, {} }},
			/*depthTarget : */		FBODepthStencilEntryDesc{ -666, EFBOTargetClearMode_NO_CLEAR, {} }
		},
		/*shaderDesc : */		ShaderDesc{
			{ true, false, false, false, true },
			{ "screenQuadById.vert", "", "", "", strFragmentShader }
		},
		/*pipelineState : */	GetScreenSpacePass_PipelineState(),
		/*inputType : */		EPassInputType_ScreenQuadById,
		/*inputVisibility : */	EVisibility_Any,
		/*isLighting : */		false
	};
}

static RenderPassDesc Downscale4x4_ScreenPassDesc{
	/*inTextures : */		std::vector<RenderPassTextureInputDesc>{{0, "", -666}},
	/*FBODesc : */			RenderPassFBODesc{ false, true, {
			EFBOTargetSizeMode_USE_PREVIOUS_DOWNSCALED_4x4,{},{},
			EFBOTargetBeforeResizeModifier_ROUND_NEXT_W8_H8,{}
		},
		std::vector<FBOColorEntryDesc>{{ "", -666, EFBOTargetClearMode_NO_CLEAR, {} }},
		FBODepthStencilEntryDesc{ -666, EFBOTargetClearMode_NO_CLEAR, {} }
	},
	/*shaderDesc : */		GetScreenSpacePass_Downscale4x4_ShaderDesc(),
	/*pipelineState : */	GetScreenSpacePass_PipelineState(),
	/*inputType : */		EPassInputType_ScreenQuadById,
	/*inputVisibility : */	EVisibility_Any,
	/*isLighting : */		false
};


static RenderPassDesc FakeTonemap_ScreenPassDesc{
	std::vector<RenderPassTextureInputDesc>{
		{0, "SceneRender", -666}
	},
	RenderPassFBODesc{
		false, true,
		{
			EFBOTargetSizeMode_USE_SCREEN_SIZE,
			{},
			{},
			EFBOTargetBeforeResizeModifier_NONE,
			{}
		},
		std::vector<FBOColorEntryDesc>{
			{ "ScreenTonemapped", -666, EFBOTargetClearMode_NO_CLEAR, {} }
		},
			FBODepthStencilEntryDesc{ -666, EFBOTargetClearMode_NO_CLEAR, {} }
	},
	// Shader
	{
		// Active stages
		{ true, false, false, false, true },
		// Stages filenames
		{ "screenQuadById.vert", "", "", "", "fakeTonemap.frag" }
	},

	// Pipeline state
	GetScreenSpacePass_PipelineState(),

	// Input type and its visibility if any
	EPassInputType_ScreenQuadById,
	EVisibility_Any,

	// Is lighting
	false
};

static RenderPassDesc KeepRedOnly_ScreenPassDesc{
	std::vector<RenderPassTextureInputDesc>{
		{0, "ScreenTonemapped", -666}
	},
		RenderPassFBODesc{
		false, true,
		{
			EFBOTargetSizeMode_USE_SCREEN_SIZE,
			{},
			{},
			EFBOTargetBeforeResizeModifier_NONE,
			{}
		},
		std::vector<FBOColorEntryDesc>{
			{ "ScreenRedOnly", -666, EFBOTargetClearMode_NO_CLEAR, {} }
		},
			FBODepthStencilEntryDesc{ -666, EFBOTargetClearMode_NO_CLEAR, {} }
	},
	// Shader
	{
		// Active stages
		{ true, false, false, false, true },
		// Stages filenames
		{ "screenQuadById.vert", "", "", "", "keepRedOnly.frag" }
	},

	// Pipeline state
	GetScreenSpacePass_PipelineState(),

	// Input type and its visibility if any
	EPassInputType_ScreenQuadById,
	EVisibility_Any,

	// Is lighting
	false
};

static RenderGraphDesc defaultRenderGraph_ScenePhotoDesc{
	// Passes vector
	{
		ZPRepass_3DPassDesc,
		Simple3D_3DPassDesc,
		//Downscale2x2_ScreenPassDesc,
		//Downscale4x4_ScreenPassDesc,
		//GetStandardIntermediateScreenSpaceDesc(EFBOTargetSizeMode_USE_PREVIOUS_DOWNSCALED_4x4, EFBOTargetBeforeResizeModifier_NONE, "downscale4x4.frag"),
		//GetStandardIntermediateScreenSpaceDesc(EFBOTargetSizeMode_USE_PREVIOUS_DOWNSCALED_4x4, EFBOTargetBeforeResizeModifier_NONE, "downscale4x4.frag"),
		//GetStandardIntermediateScreenSpaceDesc(EFBOTargetSizeMode_USE_PREVIOUS_DOWNSCALED_4x4, EFBOTargetBeforeResizeModifier_NONE, "downscale4x4.frag"),
		//GetStandardIntermediateScreenSpaceDesc(EFBOTargetSizeMode_USE_PREVIOUS_DOWNSCALED_2x2, EFBOTargetBeforeResizeModifier_NONE, "downscale2x2.frag")
		//FakeTonemap_ScreenPassDesc,
		//KeepRedOnly_ScreenPassDesc
	}
};

const RenderGraph& GetDefaultRenderGraph_ShadowMap()
{
	static RenderGraph defaultRenderGraph_ShadowMap;

	return defaultRenderGraph_ShadowMap;
}

const RenderGraph& GetDefaultRenderGraph_ScenePhoto()
{
	static RenderGraph defaultRenderGraph_ScenePhoto(defaultRenderGraph_ScenePhotoDesc);

	return defaultRenderGraph_ScenePhoto;
}

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

Renderer2::Renderer2()
{

}

Renderer2* Renderer2::GetInstance()
{
	static Renderer2 s_instance;

	return &s_instance;
}

void Renderer2::RegisterLight(const Light& light)
{
	m_lights.push_back(&light);
}

void Renderer2::RegisterDirLight(const DirLight& dirLight)
{
	m_dirLights.push_back(&dirLight);
}

void Renderer2::RegisterShadowEmittingLight(const ShadowEmittingLight& light)
{
	m_shadowEmittingLights.push_back(&light);
}

void Renderer2::RegisterRenderableObject(const RenderableObject& object)
{
	m_renderableObjects.push_back(&object);
}

void Renderer2::RegisterViewPoint(const ViewPoint& viewPoint)
{
	m_viewPoints.push_back(&viewPoint);
}

const std::vector<const ShadowEmittingLight*>& Renderer2::GetShadowEmittingLights()
{
	return m_shadowEmittingLights;
}

const std::vector<const RenderableObject*>& Renderer2::GetRenderableObjects()
{
	return m_renderableObjects;
}

const std::vector<const ViewPoint*>& Renderer2::GetViewPoints()
{
	return m_viewPoints;
}

bool Renderer2::IsViewVisible(const RenderableObject* pObject)
{
	// BOB_TODO : Try to have fun implementing visibility culling
	(void)pObject;
	return true;
}

void Renderer2::PerformVisibilitySorting()
{
	if (m_bVisibilityPerformed)
		return;

	for (const RenderableObject* pObject : GetRenderableObjects())
	{
		uint32_t visibilityBitfield = 0;

		// Determining visibility attributes and filling basic collections
		m_byMetaVisibilityRenderableObjects[EVisibility_Any].push_back(pObject);

		if (pObject->IsShadowCasting())
		{
			visibilityBitfield |= BIT_VISIBILITY_SHADOWCASTING;
			m_byMetaVisibilityRenderableObjects[EVisibility_ShadowCasting].push_back(pObject);
		}

		if (IsViewVisible(pObject))
		{
			visibilityBitfield |= BIT_VISIBILITY_VIEWVISIBLE;
			m_byMetaVisibilityRenderableObjects[EVisibility_ViewVisible].push_back(pObject);
		}

		// Filling the meta collections based on visibility attributes union
		if (visibilityBitfield == BITS_VISIBILITY_META_VIEWVISIBLE_AND_SHADOWCASTING)
		{
			m_byMetaVisibilityRenderableObjects[EVisibility_Meta_ViewVisible_And_ShadowCasting].push_back(pObject);
		}
	}

	m_bVisibilityPerformed = true;
}

std::vector<const RenderableObject*> Renderer2::GetObjectsWithVisibility(uint32_t visibilityBitfield) const
{
	DogeAssert(m_bVisibilityPerformed);

	return m_byMetaVisibilityRenderableObjects[VisibilityBitfieldToVisibilityEnum(visibilityBitfield)];
}

std::vector<const RenderableObject*> Renderer2::GetObjectsWithVisibility(EVisibility eVisibility) const
{
	DogeAssert(m_bVisibilityPerformed);

	return m_byMetaVisibilityRenderableObjects[eVisibility];
}

void Renderer2::DrawScreenQuadById() const
{
	//GL(glViewport(0, 0, m_width, m_height));

	m_glDepData->m_nullVAO.bind();
	GL(glDrawArrays(GL_TRIANGLES, 0, 3));
	m_glDepData->m_nullVAO.unbind();
}

void Renderer2::DrawScreenQuadByVertices() const
{
	DogeAssertAlways();

	//GLint oldViewport[4];
	//GL(glGetIntegerv(GL_VIEWPORT, oldViewport));

	//GL(glViewport(0, 0, m_width, m_height));

	m_glDepData->m_screenQuad.draw();

	//GL(glViewport(oldViewport[0], oldViewport[1], oldViewport[2], oldViewport[3]));
}

void Renderer2::Flush()
{
	for (const ShadowEmittingLight* pShadowEmittingLight : GetShadowEmittingLights())
	{
		// BOB_TODO : More flexibility here please
		const RenderGraph& shadowRenderGraph = GetDefaultRenderGraph_ShadowMap();

		DeployRenderGraph(shadowRenderGraph);
	}

	for (const ViewPoint* pViewPoint : GetViewPoints())
	{
		// BOB_TODO : More flexibility here please
		const RenderGraph& scenePhotoRenderGraph = GetDefaultRenderGraph_ScenePhoto();

		pViewPoint->Bind();

		DeployRenderGraph(scenePhotoRenderGraph);

		GL(glViewport(0, 0, m_width, m_height));
		scenePhotoRenderGraph.BlitLastFBOToMainDefaultIfAny();
	}
}

void Renderer2::RenderPassWithInput(const RenderPass& renderPass) const
{
	if (renderPass.GetInputType() == EPassInputType_3DScene)
	{
		const auto& toRenderObjectList = GetObjectsWithVisibility(renderPass.GetInputVisibility());

		for (const RenderableObject* pToRenderObject : toRenderObjectList)
		{
			pToRenderObject->Draw();
		}
	}
	else if (renderPass.GetInputType() == EPassInputType_ScreenQuadById)
	{
		DrawScreenQuadById();
	}
	else if (renderPass.GetInputType() == EPassInputType_ScreenQuadByVertices)
	{
		DrawScreenQuadByVertices();
	}
	else
	{
		DogeAssertMessage(false, "Unsuported right now");
	}
}

#define LIGHTS_LOOP_OUTSIDE_SHADER 0

void Renderer2::DeployRenderGraph(const RenderGraph& renderGraph) const
{
	for (auto nPass = 0; nPass < renderGraph.GetPassCount(); ++nPass)
	{
		renderGraph.BeginPass(nPass);

		const RenderPass& renderPass = renderGraph.GetPass(nPass);

		if (renderPass.IsLightingPass())
		{
#if LIGHTS_LOOP_OUTSIDE_SHADER
			for (const Light* pLight : m_lights)
			{
				pLight->UpdateRenderConstants();

				RenderPassWithInput(renderPass);
			}
#else // LIGHTS_LOOP_INSIDE_SHADER
			int dirLightsCount = m_dirLights.size();

			SystemCBuffersManager::GetMutableLightsCB().numDirLights = dirLightsCount;

			for (int nDirLight = 0; nDirLight < dirLightsCount; ++nDirLight)
			{
				const DirLight* pDirLight = m_dirLights[nDirLight];

				SystemCBuffersManager::GetMutableLightsCB().dirLights[nDirLight].m_color = pDirLight->m_color;
				SystemCBuffersManager::GetMutableLightsCB().dirLights[nDirLight].m_direction = pDirLight->m_direction;
			}

			SystemCBuffersManager::CommitLightsCB();

			RenderPassWithInput(renderPass);
#endif
		}
		else
		{
			RenderPassWithInput(renderPass);
		}

		renderGraph.EndPass(nPass);
	}
}

void Renderer2::CleanUp()
{
	m_lights.clear();

	m_viewPoints.clear();

	m_renderableObjects.clear();

	for (auto& metaVisibilityContainer : m_byMetaVisibilityRenderableObjects)
	{
		metaVisibilityContainer.clear();
	}
}

void Renderer2::SetDimensions(int width, int height)
{
	m_width = width;
	m_height = height;
}

int Renderer2::GetWidth() const
{
	return m_width;
}

int Renderer2::GetHeight() const
{
	return m_height;
}

Dim2D Renderer2::GetDim() const
{
	return { m_width, m_height };
}

GLuint Renderer2::GetDefaultGLDepthStencilId() const
{
	return m_glDefaultDepthRenderBufferId;
}

GLuint Renderer2::GetDefaultPointSamplerId() const
{
	return m_glPointSamplerId;
}

GLuint Renderer2::GetDefaultLinearSamplerId() const
{
	return m_glLinearSamplerId;
}

GLuint Renderer2::GetDefaultDiffuseSamplerId() const
{
	return m_glDiffuseSamplerId;
}

GLuint Renderer2::GetDefaultAnisotropicSamplerId() const
{
	DogeAssertAlways(); // Need to specifically ask for extension GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, not done yet
	return m_glAnisotropicSamplerId;
}

void Renderer2::AddOnRenderResizeListener(RenderGraph* renderGraph)
{
	m_onRenderResizeListeners.push_back(renderGraph);
}

bool Renderer2::InitGL()
{
	std::cerr << "OpenGL Version : " << glGetString(GL_VERSION) << std::endl;
	GL((void)0);
	std::cerr << "OpenGL Vendor : " << glGetString(GL_VENDOR) << std::endl;
	GL((void)0);

	/* Potentiel fix pour le crash quand SDL contexte > OpenGL 3.1 + GLEW */
	glewExperimental = GL_TRUE;
	GLenum GLEWinitialization(glewInit());
	GL_flushError(); // Glew might set gl error flag even when apparently successfull (know "bug")

	std::cerr << "Initializing GLEW...\n";

	if (GLEWinitialization != GLEW_OK)
	{
		std::cout << "Erreur d'initialisation de GLEW : " << glewGetErrorString(GLEWinitialization) << std::endl;
		return false;
	}

	return true;
}

void Renderer2::InitializeGLDependentData()
{
	m_glDepData = std::make_unique <GLDependentData>();

	m_glDepData->m_screenQuad.loadFullscreenQuad();

	SystemCBuffersManager::Initialize();

	GL(glGenRenderbuffers(1, &m_glDefaultDepthRenderBufferId));
	GL(glBindRenderbuffer(GL_RENDERBUFFER, m_glDefaultDepthRenderBufferId));
	GL(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, m_width, m_height));
	GL(glBindRenderbuffer(GL_RENDERBUFFER, 0));

	GL(glGenSamplers(1, &m_glPointSamplerId));
	GL(glSamplerParameteri(m_glPointSamplerId, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	GL(glSamplerParameteri(m_glPointSamplerId, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	GL(glSamplerParameteri(m_glPointSamplerId, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GL(glSamplerParameteri(m_glPointSamplerId, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

	GL(glGenSamplers(1, &m_glLinearSamplerId));
	GL(glSamplerParameteri(m_glLinearSamplerId, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GL(glSamplerParameteri(m_glLinearSamplerId, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	GL(glSamplerParameteri(m_glLinearSamplerId, GL_TEXTURE_WRAP_S, GL_REPEAT));
	GL(glSamplerParameteri(m_glLinearSamplerId, GL_TEXTURE_WRAP_T, GL_REPEAT));

	GL(glGenSamplers(1, &m_glDiffuseSamplerId));
	GL(glSamplerParameteri(m_glDiffuseSamplerId, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
	GL(glSamplerParameteri(m_glDiffuseSamplerId, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	GL(glSamplerParameteri(m_glDiffuseSamplerId, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GL(glSamplerParameteri(m_glDiffuseSamplerId, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

	GL(glGenSamplers(1, &m_glAnisotropicSamplerId));
#if 0 // Wait for correct GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT extension support
	GL(glSamplerParameterf(m_glAnisotropicSamplerId, GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, 16.0f));
#endif
	GL(glSamplerParameteri(m_glAnisotropicSamplerId, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
	GL(glSamplerParameteri(m_glAnisotropicSamplerId, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	GL(glSamplerParameteri(m_glAnisotropicSamplerId, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GL(glSamplerParameteri(m_glAnisotropicSamplerId, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
}

#include "Model.h"

void Renderer2::InitializeDemoData()
{
	m_rendererTimer.start();

	m_mainCamera.SetPositionProperties(vec3{ 0.f, 0.f, -5.f }, vec3{ 0.f, 0.f, 1.f }, vec3{ 0.f, 1.f, 0.f });
	m_mainCamera.SetProjectionProperties(70.f, (float)m_width / (float)m_height, 0.1f, 1000.f);

	static ViewPoint cameraViewPoint = m_mainCamera.ComputeViewPoint();

	RegisterViewPoint(cameraViewPoint);

	static Model testModel;
	testModel.loadFromFile("cubenorm.obj");

	RegisterRenderableObject(testModel);

	static DirLight testDirLight;
	testDirLight.m_color = vec3{ 0.f, 1.f, 0.f };
	testDirLight.m_direction = vec3{ 1.f, 0.f, 0.f };

	static DirLight testDirLight2;
	testDirLight2.m_color = vec3{ 1.f, 0.f, 0.f };
	testDirLight2.m_direction = vec3{ -1.f, 0.f, 0.f };

	RegisterDirLight(testDirLight);
	RegisterDirLight(testDirLight2);
}

void Renderer2::initializeGL()
{
	InitGL();

	GL(glViewport(0, 0, m_width, m_height));

	InitializeGLDependentData();
	InitializeDemoData();
}

void Renderer2::resizeGL(int width, int height)
{
	GL(glViewport(0, 0, width, height));

	m_width = width;
	m_height = height;

	std::cerr << "Resizing to " << width << " x " << height << '\n';

	// BOB_TOTO : Resize main rendertargets
	glDeleteRenderbuffers(1, &m_glDefaultDepthRenderBufferId);

	GL(glGenRenderbuffers(1, &m_glDefaultDepthRenderBufferId));
	GL(glBindRenderbuffer(GL_RENDERBUFFER, m_glDefaultDepthRenderBufferId));
	GL(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, m_width, m_height));
	GL(glBindRenderbuffer(GL_RENDERBUFFER, 0));

	for (const auto& resizeListeners : m_onRenderResizeListeners)
	{
		resizeListeners->Resize(width, height);
	}
}

vec3 Renderer2::GenRandomVec3() const
{
	return vec3{ (std::rand() % 100) / 100.f, (std::rand() % 100) / 100.f, (std::rand() % 100) / 100.f };
}

void Renderer2::paintGL()
{
	GL(glBindFramebuffer(GL_FRAMEBUFFER, 0));

	GL(glViewport(0, 0, m_width, m_height));

	GL(glClearColor(0, 0, 1, 0));
	GL(glClearDepth(1));

	// Make sure those buffer are writable again before clearing them (ahem)
	GL(glColorMask(true, true, true, true));
	GL(glDepthMask(true));

	GL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

	m_mainCamera.UpdateLookAt();

	ViewPoint cameraViewPoint = m_mainCamera.ComputeViewPoint();

	m_viewPoints.clear();
	RegisterViewPoint(cameraViewPoint);

	SystemCBuffersManager::GetMutablePerFrameCB().m_randRgbColor = GenRandomVec3();
	SystemCBuffersManager::GetMutablePerFrameCB().m_time = static_cast<float>(m_rendererTimer.getElapsedTimeInSec());

	SystemCBuffersManager::CommitPerFrameCB();

	PerformVisibilitySorting();

	Flush();
}

void Renderer2::reloadShader()
{

}

void Renderer2::onKeyPress(int qt_key)
{
    if (qt_key == Qt::Key_Z /*Z*/) {
		m_mainCamera.StepAdvance(Camera::EStepAdvance_FORWARD);
	}
    if (qt_key == Qt::Key_S /*S*/) {
		m_mainCamera.StepAdvance(Camera::EStepAdvance_BACKWARD);
	}
    if (qt_key == Qt::Key_Q /*Q*/) {
		m_mainCamera.StepAdvance(Camera::EStepAdvance_LEFT);
	}
    if (qt_key == Qt::Key_D /*D*/) {
		m_mainCamera.StepAdvance(Camera::EStepAdvance_RIGHT);
	}
    if (qt_key == Qt::Key_Shift /*SHIFT*/) {
		m_mainCamera.StepAdvance(Camera::EStepAdvance_UP);
	}
    if (qt_key == Qt::Key_Control /*CTRL*/) {
		m_mainCamera.StepAdvance(Camera::EStepAdvance_DOWN);
	}
}

void Renderer2::rotateCamera(int mouse_x_rel, int mouse_y_rel)
{
	m_mainCamera.RotateByMouseRelCoords(mouse_x_rel, mouse_y_rel);
}

void Renderer2::translateCamera(int mouse_x_rel, int mouse_y_rel, int mouse_z_rel)
{
	m_mainCamera.TranslateByMouseRelCoords(mouse_x_rel, mouse_y_rel, mouse_z_rel);
}

void Renderer2::toggleWireframe()
{

}

void Renderer2::setFinalFboTarget(int targetIndex)
{

}

void Renderer2::setTesselationFactor(float tesselation_factor)
{

}

void Renderer2::setDispFactor(float disp_factor)
{

}

void Renderer2::setNbSamples(int nb_samples)
{

}

void Renderer2::setRoughnessOffset(double roughnessOffset)
{

}

void Renderer2::setFresnel0(double fresnel0)
{

}

void Renderer2::loadModel(const std::string &model_path)
{

}

void Renderer2::loadLEADRTexture(const std::string & leadr)
{

}

void Renderer2::loadTexture(const std::string & texture)
{

}

void Renderer2::loadDispMap(const std::string & disp_map)
{

}

void Renderer2::loadNormalMap(const std::string & normal_map)
{

}

void Renderer2::loadEnvTexture(const std::string & env_path)
{

}

void Renderer2::loadIrradianceMap(const std::string & irr_path)
{

}

void Renderer2::setStandardRendering()
{

}

void Renderer2::setWireframeRendering()
{

}

void Renderer2::setDepthRendering()
{

}

void Renderer2::setNormalRendering()
{

}

void Renderer2::setTexcoordRendering()
{

}

void Renderer2::setBRDF(int brdf_choice)
{

}

void Renderer2::setNormalMode(bool normal_mode)
{

}

void Renderer2::setFilteringMode(bool enabled)
{

}

void Renderer2::setDiffuseEnabled(bool enabled)
{

}

void Renderer2::setSpecularDirectEnabled(bool enabled)
{

}

void Renderer2::setSpecularEnvEnabled(bool enabled)
{

}

void Renderer2::setDiffuseDirectEnabled(bool enabled)
{

}

void Renderer2::setDiffuseEnvEnabled(bool enabled)
{

}
