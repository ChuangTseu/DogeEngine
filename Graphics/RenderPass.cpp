#include "RenderPass.h"

#include "Renderer2.h"

void RenderPass::operator=(RenderPass&& other)
{
	swap(other);
}

RenderPass::RenderPass(RenderPass&& other)
{
	swap(other);
}

RenderPass::RenderPass(const RenderPassDesc& desc)
{
	LoadFromDesc(desc);
}

RenderPass::RenderPass()
{

}

void RenderPass::swap(RenderPass& other)
{
	using std::swap;

	swap(m_pipelineState, other.m_pipelineState);
	swap(m_shader, other.m_shader);
	swap(m_inputType, other.m_inputType);
	swap(m_inputVisibility, other.m_inputVisibility);
	swap(m_bIsLightingPass, other.m_bIsLightingPass);

	swap(m_texOutputsInfo, other.m_texOutputsInfo);
	swap(m_glFboId, other.m_glFboId);
	swap(m_numFBOAttachements, other.m_numFBOAttachements);
	swap(m_glColorTexIds, other.m_glColorTexIds);
	swap(m_glDepthStencilTexId, other.m_glDepthStencilTexId);
	swap(m_texOutputsInfo, other.m_texOutputsInfo);
	swap(m_texOutputsInfo, other.m_texOutputsInfo);
}

bool ValidateDesc_FBO(const RenderPassFBODesc& fboDesc)
{
	if (fboDesc.bUseDefaultColor && fboDesc.FBOColorEntries.size() > 0)
	{
		return false;
	}

	for (size_t i = 0; i < fboDesc.FBOColorEntries.size(); ++i)
	{
		const FBOColorEntryDesc& FBOEntry = fboDesc.FBOColorEntries[i];

		if (FBOEntry.semantic.empty())
		{
			return false;
		}

		if (false && /*testValidityOf*/ FBOEntry.format)
		{
			return false;
		}

		if (!IsUniqueByPred(fboDesc.FBOColorEntries.begin() + i + 1, fboDesc.FBOColorEntries.end(),
			[&](const FBOColorEntryDesc& entry) { return entry.semantic == FBOEntry.semantic; }))
		{
			return false;
		}
	}

	return true;
}

bool ValidateDesc_TexOuts(const std::vector<RenderPassTextureOutputDesc>& texOutsDesc)
{
	for (size_t i = 0; i < texOutsDesc.size(); ++i)
	{
		const RenderPassTextureOutputDesc& texOutDesc = texOutsDesc[i];

		if (texOutDesc.semantic.empty())
		{
			return false;
		}

		if (texOutDesc.slotTo >= GetMaxAllowed_TextureBindSlots())
		{
			return false;
		}

		if (false && /*testValidityOf*/ texOutDesc.format)
		{
			return false;
		}

		if (!IsUniqueByPred(texOutsDesc.begin() + i + 1, texOutsDesc.end(),
			[&](const RenderPassTextureOutputDesc& out) { return out.slotTo == texOutDesc.slotTo; }))
		{
			return false;
		}

		if (!IsUniqueByPred(texOutsDesc.begin() + i + 1, texOutsDesc.end(),
			[&](const RenderPassTextureOutputDesc& out) { return out.semantic == texOutDesc.semantic; }))
		{
			return false;
		}
	}

	return true;
}

bool ValidateDesc_TexIns(const std::vector<RenderPassTextureInputDesc>& texInsDesc)
{
	for (size_t i = 0; i < texInsDesc.size(); ++i)
	{
		const RenderPassTextureInputDesc& texInDesc = texInsDesc[i];

		if (texInDesc.semantic.empty())
		{
			return false;
		}

		if (texInDesc.slotIn >= GetMaxAllowed_TextureBindSlots())
		{
			return false;
		}

		if (false && /*testValidityOf*/ texInDesc.format)
		{
			return false;
		}

		if (!IsUniqueByPred(texInsDesc.begin() + i + 1, texInsDesc.end(),
			[&](const RenderPassTextureInputDesc& in) { return in.slotIn == texInDesc.slotIn; }))
		{
			return false;
		}

		if (!IsUniqueByPred(texInsDesc.begin() + i + 1, texInsDesc.end(),
			[&](const RenderPassTextureInputDesc& in) { return in.semantic == texInDesc.semantic; }))
		{
			return false;
		}
	}

	return true;
}

bool ValidateDescs_FBOAgainstTexOuts(const RenderPassFBODesc& fboDesc, const std::vector<RenderPassTextureOutputDesc>& texOutsDesc)
{
	u32 numFboAttachment = fboDesc.bUseDefaultColor ? 0 : fboDesc.FBOColorEntries.size();

	for (size_t i = 0; i < texOutsDesc.size(); ++i)
	{
		const RenderPassTextureOutputDesc& texOutDesc = texOutsDesc[i];

		if (!(texOutDesc.attachmentFrom < numFboAttachment))
		{
			return false;
		}

		const FBOColorEntryDesc& associatedFboEntry = fboDesc.FBOColorEntries[texOutDesc.attachmentFrom];

		if (!(texOutDesc.semantic == associatedFboEntry.semantic))
		{
			return false;
		}
	}

	return true;
}

bool ValidateDescs_SingleTexOutAgainstSingleTexIn(const RenderPassTextureOutputDesc& texOutDesc, const RenderPassTextureInputDesc& texInDesc)
{
	return texOutDesc.slotTo == texInDesc.slotIn 
		&& texOutDesc.semantic == texInDesc.semantic;
	// BOB_TODO : Maybe validate format also ? although not necessary for OpenGL/D3D validity since texIn is a very vague concept as long as TEX_2D
}

bool ValidateDescs_TexOutsAgainstTexIns(const std::vector<RenderPassTextureOutputDesc>& texOutsDesc, const std::vector<RenderPassTextureInputDesc>& texInsDesc)
{
	if (texOutsDesc.size() != texInsDesc.size())
	{
		return false;
	}

	for (size_t i = 0; i < texInsDesc.size(); ++i)
	{
		const RenderPassTextureInputDesc& texInDesc = texInsDesc[i];

		const auto it = std::find_if(texOutsDesc.begin(), texOutsDesc.end(),
			[&](const RenderPassTextureOutputDesc& out) { return out.slotTo == texInDesc.slotIn; });

		if (it == texOutsDesc.end())
		{
			return false;
		}

		if (!ValidateDescs_SingleTexOutAgainstSingleTexIn(*it, texInDesc))
		{
			return false;
		}
	}

	return true;
}

void RenderPass::LoadFromDesc(const RenderPassDesc& desc)
{
	DogeAssert(ValidateDesc_FBO(desc.FBODesc));
	DogeAssert(ValidateDesc_TexOuts(desc.texOutputsDesc));
	DogeAssert(ValidateDesc_TexIns(desc.texInputsDesc));
	DogeAssert(ValidateDescs_FBOAgainstTexOuts(desc.FBODesc, desc.texOutputsDesc));

	SetupFBOFromDesc(desc.FBODesc);

	for (const auto& texOutDesc : desc.texOutputsDesc)
	{
		m_texOutputsInfo.emplace_back(FBOAttachmentToSlot{ texOutDesc.attachmentFrom, texOutDesc.slotTo });
	}

	m_shader.LoadFromDesc(desc.shaderDesc);

	m_pipelineState = desc.pipelineState;

	m_inputType = desc.inputType;
	m_inputVisibility = desc.inputVisibility;

	m_bIsLightingPass = desc.bIsLightingPass;


	m_renderPassDesc = desc;
}

void RenderPass::Resize(int width, int height)
{
	glDeleteFramebuffers(1, &m_glFboId);
	glDeleteTextures(1, &m_glDepthStencilTexId);
	glDeleteTextures(m_glColorTexIds.size(), m_glColorTexIds.data());

	m_glColorTexIds.clear();
	m_glColorTexDimensions.clear();

	SetupFBOFromDesc(m_renderPassDesc.FBODesc);
}

//
// Note : a bit hacky, but any standard downscaling (screen 2x2,4x4,8x8) will base itself upon the screen dim rounded to the next multiple of 8
// This allows for perfect size matching between consecutive standard downscaled sizes
// AGAIN, this feels very hacky and might benefit from more thinking about the problem at hand
Dim2D GetDimFromFBOSizeHeuristic(FBOSizeHeuristic sizeHeuristic)
{
	Dim2D dim;

	if (sizeHeuristic.eSizeMode == EFBOTargetSizeMode_USE_SCREEN_SIZE)
	{
		dim = G_RENDERER2.GetDim();
	}
	else if (sizeHeuristic.eSizeMode == EFBOTargetSizeMode_USE_SCREEN_DOWNSCALED_2x2)
	{
		dim.width = NextMultiple8(G_RENDERER2.GetWidth()) / 2;
		dim.height = NextMultiple8(G_RENDERER2.GetHeight()) / 2;
	}
	else if (sizeHeuristic.eSizeMode == EFBOTargetSizeMode_USE_SCREEN_DOWNSCALED_4x4)
	{
		dim.width = NextMultiple8(G_RENDERER2.GetWidth()) / 4;
		dim.height = NextMultiple8(G_RENDERER2.GetHeight()) / 4;
	}
	else if (sizeHeuristic.eSizeMode == EFBOTargetSizeMode_USE_SCREEN_DOWNSCALED_8x8)
	{
		dim.width = NextMultiple8(G_RENDERER2.GetWidth()) / 8;
		dim.height = NextMultiple8(G_RENDERER2.GetHeight()) / 8;
	}
	else if (sizeHeuristic.eSizeMode == EFBOTargetSizeMode_CUSTOM_SIZE)
	{
		dim = sizeHeuristic.customDim;
	}
	else
	{
		DogeAssertAlways(); // Todo support other modes, implies refacto
	}

	return dim;
}

void RenderPass::SetupFBOFromDesc(const RenderPassFBODesc& desc)
{
	GL(glGenFramebuffers(1, &m_glFboId));
	GL(glBindFramebuffer(GL_FRAMEBUFFER, m_glFboId));

	if (desc.bUseDefaultColor)
	{
		m_numFBOAttachements = 0;
	}
	else
	{
		m_numFBOAttachements = desc.FBOColorEntries.size();

		for (size_t i = 0; i < desc.FBOColorEntries.size(); ++i)
		{
			const FBOColorEntryDesc& colorEntryDesc = desc.FBOColorEntries[i];

			m_glColorTexIds.emplace_back();
			GLuint& glColorTexId = m_glColorTexIds.back();

			GL(glGenTextures(1, &glColorTexId));

			GL(glBindTexture(GL_TEXTURE_2D, glColorTexId));

			Dim2D dim = GetDimFromFBOSizeHeuristic(colorEntryDesc.sizeHeuristic);

			m_glColorTexDimensions.emplace_back(vec4{ (float)dim.width, (float)dim.height, 1.f / dim.width, 1.f / dim.height });

			GL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, dim.width, dim.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0));
			GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
			GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));

			GL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, glColorTexId, 0));
		}
	}

	if (desc.bUseDefaultDepthStencil)
	{
		GL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, G_RENDERER2.GetDefaultGLDepthStencilId()));
	}
	else
	{
		const FBODepthStencilEntryDesc& depthStencilEntryDesc = desc.FBODepthStencilEntry;

		m_glDepthStencilTexId;

		GL(glGenTextures(1, &m_glDepthStencilTexId));

		GL(glBindTexture(GL_TEXTURE_2D, m_glDepthStencilTexId));

		Dim2D dim = GetDimFromFBOSizeHeuristic(depthStencilEntryDesc.sizeHeuristic);

		GL(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, dim.width, dim.height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0));

		GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0));
		GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0));

		GL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_glDepthStencilTexId, 0));
	}

	DogeAssertMessage(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Error. Frame buffer INCOMPLETE.");

	GL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

void RenderPass::BindFBO() const
{
	static const GLuint allDrawBuffersAttachments[] = {
		GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4,
		GL_COLOR_ATTACHMENT5, GL_COLOR_ATTACHMENT6, GL_COLOR_ATTACHMENT7, GL_COLOR_ATTACHMENT8, GL_COLOR_ATTACHMENT9,
		GL_COLOR_ATTACHMENT10, GL_COLOR_ATTACHMENT11, GL_COLOR_ATTACHMENT12, GL_COLOR_ATTACHMENT13, GL_COLOR_ATTACHMENT14,
		GL_COLOR_ATTACHMENT15 };

	GL(glBindFramebuffer(GL_FRAMEBUFFER, m_glFboId));
	GL(glDrawBuffers(m_numFBOAttachements, allDrawBuffersAttachments));

	for (size_t i = 0; i < m_numFBOAttachements; ++i)
	{
		const FBOColorEntryDesc& colorEntryDesc = m_renderPassDesc.FBODesc.FBOColorEntries[i];

		if (colorEntryDesc.eClearMode == EFBOTargetClearMode_CLEAR_BEFORE_USE)
		{
			GL(glClearBufferfv(GL_COLOR, i, colorEntryDesc.clearColor));
		}

		SystemCBuffersManager::GetMutablePerFboCB().FboDimensions[i] = m_glColorTexDimensions[i];
	}

	if (m_numFBOAttachements > 0)
	{
		GL(glViewport(0, 0, (int)m_glColorTexDimensions[0].x, (int)m_glColorTexDimensions[0].y));
		SystemCBuffersManager::CommitPerFboCB();
	}

	const FBODepthStencilEntryDesc& depthStencilEntryDesc = m_renderPassDesc.FBODesc.FBODepthStencilEntry;

	if (depthStencilEntryDesc.eClearMode == EFBOTargetClearMode_CLEAR_BEFORE_USE)
	{
		GL(glClearBufferfv(GL_DEPTH, 0, &depthStencilEntryDesc.clearDepth));
	}
}

void RenderPass::UnbindFBO() const
{
	GL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

EPassInputType RenderPass::GetInputType() const
{
	return m_inputType;
}

EVisibility RenderPass::GetInputVisibility() const
{
	return m_inputVisibility;
}

bool RenderPass::IsLightingPass() const
{
	return m_bIsLightingPass;
}

void RenderPass::PreRender() const
{
	m_pipelineState.BindPipelineState();
	m_shader.use();
	BindFBO();
}

void RenderPass::PostRender() const
{
	Shader::unbind();
	UnbindFBO();

	for (const FBOAttachmentToSlot& texOutInfo : m_texOutputsInfo)
	{
		BindTextureAndSamplerToSlot(m_glColorTexIds[texOutInfo.attachmentFrom], G_RENDERER2.GetDefaultPointSamplerId(), texOutInfo.slotTo);
	}
}
