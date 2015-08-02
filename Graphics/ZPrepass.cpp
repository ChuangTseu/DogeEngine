#include "ZPrepass.h"


ZPrepass::ZPrepass()
{
	DepthStencilState& ds = m_pipelineState.m_depthStencilState;

	ds.bDepthTest = true;
	ds.depthTest.depthFunc = EGlFunc_LEQUAL;

	ds.bDepthMask = true;

	ds.bStencilTest = false;

	RasterizerState& rs = m_pipelineState.m_rasterizerState;

	// WARNING, THIS ONE MIGHT BETTER BE THE MESH/MATERIAL RESPONSIBILITY
	rs.bCullFace = true;
	rs.cullFace.cullMode = EGlCullMode_FRONT;
	rs.cullFace.cullOrientation = EGlCullOrientation_CCW;

	BlendState& bs = m_pipelineState.m_blendState;

	bs.bBlend = false;
	bs.blendMask = { false, false, false, false }; // COLOR WRITE DISABLED
}


ZPrepass::~ZPrepass()
{
}
