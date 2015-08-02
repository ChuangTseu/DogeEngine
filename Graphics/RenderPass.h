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
};

struct FBOColorEntryDesc
{
	std::string semantic;
	int format;

	FBOSizeHeuristic sizeHeuristic;

	EFBOTargetClearMode eClearMode;
	float clearColor[4];
};

struct FBODepthStencilEntryDesc
{
	int format;

	FBOSizeHeuristic sizeHeuristic;

	EFBOTargetClearMode eClearMode;
	float clearDepth;
};

struct RenderPassFBODesc
{
	bool bUseDefaultColor;
	bool bUseDefaultDepthStencil;

	std::vector<FBOColorEntryDesc> FBOColorEntries;
	FBODepthStencilEntryDesc FBODepthStencilEntry;
};

struct RenderPassDesc
{
	std::vector<RenderPassTextureInputDesc> texInputsDesc;
	std::vector<RenderPassTextureOutputDesc> texOutputsDesc;

	RenderPassFBODesc FBODesc;

	ShaderDesc shaderDesc;

	PipelineState pipelineState;

	EPassInputType inputType;
	EVisibility inputVisibility; // Not used for ScreenPasses

	bool bIsLightingPass;
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

class RenderPass
{
public:
	RenderPass();

	RenderPass(const RenderPassDesc& desc);

	void swap(RenderPass& other);

	RenderPass(RenderPass&& other);

	void operator=(RenderPass&& other);

	RenderPass(const RenderPass& other) = delete;
	void operator=(const RenderPass& other) = delete;

	void LoadFromDesc(const RenderPassDesc& desc);

	void Resize(int width, int height);

	void SetupFBOFromDesc(const RenderPassFBODesc& desc);

	void BindFBO() const;
	void UnbindFBO() const;

	EPassInputType GetInputType() const;

	EVisibility GetInputVisibility() const;

	bool IsLightingPass() const;

	void PreRender() const;

	void PostRender() const;

	struct FBOAttachmentToSlot
	{
		int attachmentFrom;
		int slotTo;
	};

	std::vector<FBOAttachmentToSlot> m_texOutputsInfo;

	PipelineState m_pipelineState;
	Shader m_shader;

	GLuint m_glFboId = 0;
	u32 m_numFBOAttachements;

	std::vector<GLuint> m_glColorTexIds;
	GLuint m_glDepthStencilTexId = 0;

	std::vector<vec4> m_glColorTexDimensions;

	EPassInputType m_inputType;
	EVisibility m_inputVisibility; // Not used for ScreenPasses

	bool m_bIsLightingPass;

	RenderPassDesc m_renderPassDesc;
};

