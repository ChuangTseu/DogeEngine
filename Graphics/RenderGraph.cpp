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
		}
	}

	return true;
}

void RenderGraph::ComputePassesDimensions(const RenderGraphDesc& desc)
{
	m_passesDims.clear();

	for (size_t nPassDesc = 0; nPassDesc < desc.passesDescs.size(); ++nPassDesc)
	{
		const auto& passDesc = desc.passesDescs[nPassDesc];

		Dim2D dim = ComputeSizeFromHeuristic(passDesc.FBODesc.sizeHeuristic, nPassDesc == 0 ? nullptr : &m_passesDims[nPassDesc - 1]);
		m_passesDims.push_back(dim);
	}
}

void RenderGraph::LoadFromDesc(const RenderGraphDesc& desc)
{
	DogeAssert(ValidateDesc_RenderGraph(desc));

	m_desc = desc;

	//m_renderPasses.reserve(desc.passesDescs.size());

	//m_renderPasses.resize(desc.passesDescs.size());

	new (&m_renderPasses) std::vector<RenderPass>(desc.passesDescs.size());

	//m_renderPasses.

	ComputePassesDimensions(desc);

	for (size_t nPassDesc = 0; nPassDesc < desc.passesDescs.size(); ++nPassDesc)
	{
		const auto& passDesc = desc.passesDescs[nPassDesc];

		//DogeAssert(ValidateDesc_FBO(passDesc.FBODesc));
		//DogeAssert(ValidateDesc_TexIns(passDesc.texInputsDesc));

		if (!passDesc.FBODesc.bUseDefaultColor)
		{
			int m_numFBOAttachements = passDesc.FBODesc.FBOColorEntries.size();

			for (int nFBOEntry = 0; nFBOEntry < m_numFBOAttachements; ++nFBOEntry)
			{
				const FBOColorEntryDesc& colorEntryDesc = passDesc.FBODesc.FBOColorEntries[nFBOEntry];

				if (!colorEntryDesc.semantic.empty())
				{
					//DogeAssert(m_availableSemantics.find(colorEntryDesc.semantic) == m_availableSemantics.end());
					// Replace the by the last one

					m_availableSemantics.emplace(std::make_pair(colorEntryDesc.semantic, 
						SemanticColorTargetDesc{ nPassDesc, colorEntryDesc.semantic, nFBOEntry }));
				}
				else
				{
					// Nothing to do here, beginpass will auto bind previous color target 0
					//DogeAssertAlways(); // TODO SUPPORT
				}
				
				// vec4{ (float)dim.width, (float)dim.height, 1.f / dim.width, 1.f / dim.height };
			}
		}

		for (const auto& texInDesc : passDesc.texInputsDesc)
		{
			//m_texOutputsInfo.emplace_back(FBOAttachmentToSlot{ texOutDesc.attachmentFrom, texOutDesc.slotTo });
		}

		SetPassFromPassDescAndDim(nPassDesc, passDesc, m_passesDims[nPassDesc]);
	}
}

void RenderGraph::SetPassFromPassDescAndDim(int nPass, const RenderPassDesc& desc, Dim2D FBODim)
{
	m_renderPasses[nPass].LoadFromDesc(desc, FBODim);
}

//RenderPass& RenderGraph::AddPassLoadManually()
//{
//	m_renderPasses.emplace_back();
//	return m_renderPasses.back();
//}

int RenderGraph::GetPassCount() const
{
	return m_renderPasses.size();
}

const RenderPass& RenderGraph::GetPass(int i) const
{
	return m_renderPasses[i];
}

void RenderGraph::BeginPass(int i) const
{
	// Provided required input textures
	for (const ColorTargetInputInfo& inputInfo : GetPass(i).GetColorTargetInputInfos())
	{
		if (!inputInfo.semantic.empty())
		{
			const auto& it = m_availableSemantics.find(inputInfo.semantic);
			DogeAssert(it != m_availableSemantics.end());

			auto colorTargetFrom = it->second;

			DogeAssert(colorTargetFrom.passIndexFrom < i);

			BindTextureAndSamplerToSlot(
				GetPass(colorTargetFrom.passIndexFrom).m_glColorTexIds[colorTargetFrom.slot], 
				G_RENDERER2.GetDefaultPointSamplerId(), 
				inputInfo.slot);
		}
		else
		{
			DogeAssert(i > 0);
			DogeAssert(GetPass(i - 1).m_glColorTexIds.size() > 0);

			BindTextureAndSamplerToSlot(
				GetPass(i - 1).m_glColorTexIds[0],
				G_RENDERER2.GetDefaultPointSamplerId(),
				inputInfo.slot);
		}
	}

	GetPass(i).PreRender();
}

void RenderGraph::EndPass(int i) const
{
	GetPass(i).PostRender();
}

const std::vector<RenderPass>& RenderGraph::GetAllPasses() const
{
	return m_renderPasses;
}

static RenderPassDesc BlitFBO_ScreenPassDesc{
	{}, {},
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
	static RenderPass defaultPass_BlitFBO(BlitFBO_ScreenPassDesc, G_RENDERER2.GetDim());

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

	//BindTextureAndSamplerToSlot(m_renderPasses.back().m_glColorTexIds[0], G_RENDERER2.GetDefaultDiffuseSamplerId(), 0);
	BindTextureAndSamplerToSlot(m_renderPasses.back().m_glColorTexIds[0], G_RENDERER2.GetDefaultPointSamplerId(), 0);

	GL(glGenerateMipmap(GL_TEXTURE_2D));

	G_RENDERER2.DrawScreenQuadById();

	UnbindTextureSlot(0);
	Shader::unbind();
}

void RenderGraph::Resize(int width, int height)
{
	ComputePassesDimensions(m_desc);

	for (size_t i = 0; i < m_renderPasses.size(); ++i)
	{
		m_renderPasses[i].Resize(m_passesDims[i]);
	}
}
