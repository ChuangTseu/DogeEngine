#pragma once

#include "Doge.h"

#include <vector>

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

	void LoadFromDesc(const RenderGraphDesc& desc);

	void AddPassFromPassDesc(const RenderPassDesc& desc);

	RenderPass& AddPassLoadManually();

	int GetPassCount() const;

	const RenderPass& GetPass(int i) const;

	const std::vector<RenderPass>& GetAllPasses() const;

	void BlitLastFBOToMainDefaultIfAny() const;

	void Resize(int width, int height);

	std::vector<RenderPass> m_renderPasses;
};
