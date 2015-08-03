#pragma once

#include "Doge.h"

#include <vector>
#include <map>
#include <string>

#include "RenderPass.h"

struct RenderGraphDesc
{
	std::vector<RenderPassDesc> passesDescs;
};

class RenderGraph
{
public:
	RenderGraph();

	RenderGraph(const RenderGraphDesc& desc);

	void ComputePassesDimensions(const RenderGraphDesc& desc);
	void LoadFromDesc(const RenderGraphDesc& desc);

	void SetPassFromPassDescAndDim(int nPass, const RenderPassDesc& desc, Dim2D FBODim);

	//RenderPass& AddPassLoadManually();

	int GetPassCount() const;

	const RenderPass& GetPass(int i) const;

	void BeginPass(int i) const;
	void EndPass(int i) const;

	const std::vector<RenderPass>& GetAllPasses() const;

	void BlitLastFBOToMainDefaultIfAny() const;

	void Resize(int width, int height);

	std::vector<RenderPass> m_renderPasses;

	std::map<std::string, SemanticColorTargetDesc> m_availableSemantics;
	std::vector<Dim2D> m_passesDims;

	RenderGraphDesc m_desc;
};
