#include "RenderGraph.h"

#include "Renderer2.h"

RenderGraph::RenderGraph(const RenderGraphDesc& desc)
{
	LoadFromDesc(desc);
}

RenderGraph::RenderGraph()
{

}

bool ValidateDesc_RenderGraph(const RenderGraphDesc& desc)
{
	if (desc.passesDescs.size() == 0)
	{
		return false;
	}

	// BOT_TODO : Maybe validate all the passes desc here directly rather than waiting the RenderPasses individual loading
	if (desc.passesDescs.size() > 1)
	{
		for (size_t i = 1; i < desc.passesDescs.size(); ++i)
		{
			const auto& out = desc.passesDescs[i - 1];
			const auto& in = desc.passesDescs[i];

			if (!ValidateDescs_TexOutsAgainstTexIns(out.texOutputsDesc, in.texInputsDesc))
			{
				return false;
			}
		}
	}

	return true;
}

void RenderGraph::LoadFromDesc(const RenderGraphDesc& desc)
{
	DogeAssert(ValidateDesc_RenderGraph(desc));

	m_renderPasses.reserve(desc.passesDescs.size());

	for (const auto& passDesc : desc.passesDescs)
	{
		AddPassFromPassDesc(passDesc);
	}
}

void RenderGraph::AddPassFromPassDesc(const RenderPassDesc& desc)
{
	m_renderPasses.emplace_back(desc);
}

RenderPass& RenderGraph::AddPassLoadManually()
{
	m_renderPasses.emplace_back();
	return m_renderPasses.back();
}

int RenderGraph::GetPassCount() const
{
	return m_renderPasses.size();
}

const RenderPass& RenderGraph::GetPass(int i) const
{
	return m_renderPasses[i];
}

const std::vector<RenderPass>& RenderGraph::GetAllPasses() const
{
	return m_renderPasses;
}

static RenderPassDesc BlitFBO_ScreenPassDesc{
	{}, {}, {},
	// Shader
	{
		// Active stages
		{ true, false, false, false, true },
		// Stages filenames
		{ "screenQuadById.vert", "", "", "", "screenQuadById.frag" }
	},

	// Pipeline state
	PipelineState(),

	// Input type and its visibility if any
	EPassInputType_ScreenQuadById,
	EVisibility_Any,

	// Is lighting
	false
};

static const RenderPass& GetDefaultPass_BlitFBO()
{
	static RenderPass defaultPass_BlitFBO(BlitFBO_ScreenPassDesc);

	return defaultPass_BlitFBO;
}

void RenderGraph::BlitLastFBOToMainDefaultIfAny() const
{
	if (m_renderPasses.size() == 0 || m_renderPasses.back().m_numFBOAttachements == 0)
	{
		return;
	}

	GL(glBindFramebuffer(GL_FRAMEBUFFER, 0));

	const RenderPass& pass = GetDefaultPass_BlitFBO();

	pass.m_pipelineState.BindPipelineState();
	pass.m_shader.use();

	BindTextureAndSamplerToSlot(m_renderPasses.back().m_glColorTexIds[0], G_RENDERER2.GetDefaultDiffuseSamplerId(), 0);

	GL(glGenerateMipmap(GL_TEXTURE_2D));

	G_RENDERER2.DrawScreenQuadById();

	UnbindTextureSlot(0);
	Shader::unbind();
}

void RenderGraph::Resize(int width, int height)
{
	for (RenderPass& renderPass : m_renderPasses)
	{
		renderPass.Resize(width, height);
	}
}
