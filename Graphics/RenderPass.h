#pragma once

#include "Doge.h"

#include "Visibility.h"
#include "shader.h"
#include "PipelineState.h"

#include <vector>

enum EPassInputType
{
	EPassInputType_3DScene,
	EPassInputType_ScreenQuadById,
	EPassInputType_ScreenQuadByVertices,

	EPassInputType_COUNT
};

enum
{
	INFO_CONTIGUOUS_TEXTURES,
	INFO_NONE
};

struct RenderPassTextureInputDesc // For validation purposes only (and maybe "auto" adaptation)
{
	u32 slotIn;

	std::string semantic;
	int format;
};

struct RenderPassTextureOutputDesc
{
	u32 attachmentFrom;
	u32 slotTo;

	std::string semantic;
	int format;
};

enum EFBOTargetClearMode {
	EFBOTargetClearMode_CLEAR_BEFORE_USE,
	EFBOTargetClearMode_NO_CLEAR
};

enum EFBOTargetSizeMode {
	EFBOTargetSizeMode_USE_SCREEN_SIZE,

	EFBOTargetSizeMode_USE_SCREEN_DOWNSCALED_2x2,
	EFBOTargetSizeMode_USE_SCREEN_DOWNSCALED_4x4,
	EFBOTargetSizeMode_USE_SCREEN_DOWNSCALED_8x8,
	EFBOTargetSizeMode_USE_SCREEN_DOWNSCALED_CUSTOM,

	EFBOTargetSizeMode_USE_PREVIOUS_SIZE,

	EFBOTargetSizeMode_USE_PREVIOUS_DOWNSCALED_2x2,
	EFBOTargetSizeMode_USE_PREVIOUS_DOWNSCALED_4x4,
	EFBOTargetSizeMode_USE_PREVIOUS_DOWNSCALED_8x8,
	EFBOTargetSizeMode_USE_PREVIOUS_DOWNSCALED_CUSTOM,

	EFBOTargetSizeMode_CUSTOM_SIZE
};

inline bool SizeModeImpliesDownscale(EFBOTargetSizeMode eSizeMode)
{
	return
		(eSizeMode >= EFBOTargetSizeMode_USE_SCREEN_DOWNSCALED_2x2 && eSizeMode <= EFBOTargetSizeMode_USE_SCREEN_DOWNSCALED_CUSTOM) ||
		(eSizeMode >= EFBOTargetSizeMode_USE_PREVIOUS_DOWNSCALED_2x2 && eSizeMode <= EFBOTargetSizeMode_USE_PREVIOUS_DOWNSCALED_CUSTOM)
		;
}

enum EFBOTargetBeforeResizeModifier {
	EFBOTargetBeforeResizeModifier_NONE,

	EFBOTargetBeforeResizeModifier_ROUND_NEXT_W8_H8,
	EFBOTargetBeforeResizeModifier_ROUND_NEXT_W4_H4,
	EFBOTargetBeforeResizeModifier_ROUND_NEXT_W2_H2,

	EFBOTargetBeforeResizeModifier_ROUND_NEXT_CUSTOM,
};

struct Dim2D
{
	int width;
	int height;
};

struct Dim3D
{
	int width;
	int height;
	int depth;
};

struct FBOSizeHeuristic
{
	EFBOTargetSizeMode eSizeMode;
	Dim2D customDim;
	Dim2D customDownscale;

	EFBOTargetBeforeResizeModifier eBeforeResizeModifier;
	Dim2D customMultiples;
};

struct FBOColorEntryDesc
{
	std::string semantic;
	int format;

	EFBOTargetClearMode eClearMode;
	float clearColor[4];
};

struct FBODepthStencilEntryDesc
{
	int format;

	EFBOTargetClearMode eClearMode;
	float clearDepth;
};

struct RenderPassFBODesc
{
	bool bUseDefaultColor;
	bool bUseDefaultDepthStencil;

	FBOSizeHeuristic sizeHeuristic;

	std::vector<FBOColorEntryDesc> FBOColorEntries;
	FBODepthStencilEntryDesc FBODepthStencilEntry;
};

enum ELightingMode {
	ELightingMode_NO_LIGHTING,
	ELightingMode_LIGHTING_LOOP_IN_SHADER,
	ELightingMode_LIGHTING_LOOP_CLIENT_SIDE
};

struct RenderPassDesc
{
	std::vector<RenderPassTextureInputDesc> texInputsDesc;
	//std::vector<RenderPassTextureOutputDesc> texOutputsDesc;

	RenderPassFBODesc FBODesc;

	ShaderDesc shaderDesc;

	PipelineState pipelineState;

	EPassInputType inputType;
	EVisibility inputVisibility; // Not used for ScreenPasses

	ELightingMode eLightingMode;
};

struct SemanticColorTargetDesc
{
	int passIndexFrom;

	std::string semantic;
	int slot;
};

struct RenderGraphDependentPassDesc
{
	Dim2D previousPassDimensions;

	std::vector<SemanticColorTargetDesc> allPreviousSemanticColorTargets;
};

struct FBOAttachmentToSlot
{
	int attachmentFrom;
	int slotTo;
};

struct ColorTargetInputInfo
{
	std::string semantic;
	int slot;
};

template<class IN_IT, class PRED>
inline bool IsUniqueByPred(IN_IT itFirst, IN_IT itLast, PRED pred)
{
	return std::find_if(itFirst, itLast, pred) == itLast;
}

template<class IN_IT, class PRED>
inline bool IsPresentByPred(IN_IT itFirst, IN_IT itLast, PRED pred)
{
	return std::find_if(itFirst, itLast, pred) != itLast;
}

inline u32 GetMaxAllowed_TextureBindSlots()
{
	return 32; // BOB_TODO : do something real here
}

bool ValidateDesc_FBO(const RenderPassFBODesc& fboDesc);

bool ValidateDesc_TexOuts(const std::vector<RenderPassTextureOutputDesc>& texOutsDesc);

bool ValidateDesc_TexIns(const std::vector<RenderPassTextureInputDesc>& texInsDesc);

bool ValidateDescs_FBOAgainstTexOuts(const RenderPassFBODesc& fboDesc,
	const std::vector<RenderPassTextureOutputDesc>& texOutsDesc);

bool ValidateDescs_SingleTexOutAgainstSingleTexIn(const RenderPassTextureOutputDesc& texOutDesc, 
	const RenderPassTextureInputDesc& texInDesc);

bool ValidateDescs_TexOutsAgainstTexIns(const std::vector<RenderPassTextureOutputDesc>& texOutsDesc, 
	const std::vector<RenderPassTextureInputDesc>& texInsDesc);

Dim2D ComputeSizeFromHeuristic(FBOSizeHeuristic sizeHeuristic, const Dim2D* maybePreviousDim = nullptr);
Dim2D GetDimFromFBOSizeHeuristic(FBOSizeHeuristic sizeHeuristic);

class RenderPass
{
public:
	RenderPass();

	RenderPass(const RenderPassDesc& desc, Dim2D FBODim);

	void swap(RenderPass& other) = delete;
	RenderPass(RenderPass&& other) = delete;
	void operator=(RenderPass&& other) = delete;

	RenderPass(const RenderPass& other) = delete;
	void operator=(const RenderPass& other) = delete;

	void LoadFromDesc(const RenderPassDesc& desc, Dim2D FBODim);
	void ContributeToRenderGraphDependentPassDesc(RenderGraphDependentPassDesc& desc);

	void Resize(Dim2D FBODim);

	void SetupFBOFromDesc(const RenderPassFBODesc& desc, Dim2D FBODim);

	void BindFBO() const;
	void UnbindFBO() const;

	EPassInputType GetInputType() const;

	EVisibility GetInputVisibility() const;

	bool IsLightingPass() const;
	ELightingMode GetLightingMode() const;

	void PreRender() const;

	void PostRender() const;

	const std::vector<ColorTargetInputInfo>& GetColorTargetInputInfos() const;

	//std::vector<FBOAttachmentToSlot> m_texOutputsInfo;
	std::vector<ColorTargetInputInfo> m_texInputsInfo;

	PipelineState m_pipelineState;
	Shader m_shader;

	Dim2D m_FBODim;

	GLuint m_glFboId = 0;
	u32 m_numFBOAttachements;

	std::vector<GLuint> m_glColorTexIds;
	GLuint m_glDepthStencilTexId = 0;

	std::vector<vec4> m_glColorTexDimensions;

	EPassInputType m_inputType;
	EVisibility m_inputVisibility; // Not used for ScreenPasses

	bool m_bIsLightingPass;
	ELightingMode m_lightingMode;

	RenderPassDesc m_renderPassDesc;
};

