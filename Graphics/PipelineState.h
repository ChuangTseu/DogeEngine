#pragma once

#include "GL.h"

struct DepthStencilState
{
	DepthStencilState()
	{
		bDepthMask = true;

		bDepthTest = false;
		depthTest.depthFunc = EGlFunc_LESS;

		bStencilTest = false;

		stencilTest.front.stencilFunc = EGlFunc_ALWAYS;
		stencilTest.front.stencilRef = 0;
		stencilTest.front.stencilMask = 0xFFFFFFFF;

		stencilTest.front.opStencilFail = EGlFailAction_KEEP;
		stencilTest.front.opDepthOnlyFail = EGlFailAction_KEEP;
		stencilTest.front.opStencilDepthPass = EGlFailAction_KEEP;

		stencilTest.back.stencilFunc = EGlFunc_ALWAYS;
		stencilTest.back.stencilRef = 0;
		stencilTest.back.stencilMask = 0xFFFFFFFF;

		stencilTest.back.opStencilFail = EGlFailAction_KEEP;
		stencilTest.back.opDepthOnlyFail = EGlFailAction_KEEP;
		stencilTest.back.opStencilDepthPass = EGlFailAction_KEEP;
	}

	bool bDepthMask = true;

	bool bDepthTest = false;
	struct {
		EGlFunc depthFunc = EGlFunc_LESS;
	} depthTest;

	bool bStencilTest = false;
	struct {
		struct {
			EGlFunc stencilFunc = EGlFunc_ALWAYS;
			int stencilRef = 0;
			unsigned int stencilMask = 0xFFFFFFFF;

			EGlFailAction opStencilFail = EGlFailAction_KEEP;
			EGlFailAction opDepthOnlyFail = EGlFailAction_KEEP;
			EGlFailAction opStencilDepthPass = EGlFailAction_KEEP;
		} front;

		struct {
			EGlFunc stencilFunc = EGlFunc_ALWAYS;
			int stencilRef = 0;
			unsigned int stencilMask = 0xFFFFFFFF;

			EGlFailAction opStencilFail = EGlFailAction_KEEP;
			EGlFailAction opDepthOnlyFail = EGlFailAction_KEEP;
			EGlFailAction opStencilDepthPass = EGlFailAction_KEEP;
		} back;
	} stencilTest;

	void BindState() const
	{
		GL(glDepthMask(bDepthMask));

		if (bStencilTest) {
			GL(glEnable(GL_STENCIL_TEST));
			GL(glStencilFuncSeparate(GL_FRONT, stencilTest.front.stencilFunc, stencilTest.front.stencilRef, stencilTest.front.stencilMask));
			GL(glStencilOpSeparate(GL_FRONT, stencilTest.front.opStencilFail, stencilTest.front.opDepthOnlyFail, stencilTest.front.opStencilDepthPass));

			GL(glStencilFuncSeparate(GL_BACK, stencilTest.back.stencilFunc, stencilTest.back.stencilRef, stencilTest.back.stencilMask));
			GL(glStencilOpSeparate(GL_BACK, stencilTest.back.opStencilFail, stencilTest.back.opDepthOnlyFail, stencilTest.back.opStencilDepthPass));
		}
		else { GL(glDisable(GL_STENCIL_TEST)); }

		if (bDepthTest) {
			GL(glEnable(GL_DEPTH_TEST));
			GL(glDepthFunc(depthTest.depthFunc));
		}
		else { GL(glDisable(GL_DEPTH_TEST)); }
	}
};

struct RasterizerState
{
	RasterizerState()
	{
		fillMode = EGlFillMode_FILL;

		bAntialiasedLine = false;

		bCullFace = false;
		cullFace.cullMode = EGlCullMode_BACK;
		cullFace.cullOrientation = EGlCullOrientation_CCW;

		bPolygonOffsetFill = false;
		polygonOffsetFill.factor = 0.f;
		polygonOffsetFill.units = 0.f;

		bScissorTest = false;

		bMultiSample = false;
	}

	EGlFillMode fillMode = EGlFillMode_FILL;

	bool bAntialiasedLine = false;

	bool bCullFace = false;
	struct {
		EGlCullMode cullMode = EGlCullMode_BACK;
		EGlCullOrientation cullOrientation = EGlCullOrientation_CCW;
	} cullFace;

	bool bPolygonOffsetFill = false;
	struct {
		float factor = 0.f;
		float units = 0.f;
	} polygonOffsetFill;

	bool bScissorTest = false;

	bool bMultiSample = false;

	void BindState() const
	{
		GL(glPolygonMode(GL_FRONT_AND_BACK, fillMode));

		if (bCullFace) {
			GL(glEnable(GL_CULL_FACE));
			GL(glCullFace(cullFace.cullMode));
			GL(glFrontFace(cullFace.cullOrientation));
		}
		else { GL(glDisable(GL_CULL_FACE)); }

		if (bAntialiasedLine) {
			GL(glEnable(GL_LINE_SMOOTH));
			GL(glHint(GL_LINE_SMOOTH_HINT, GL_NICEST));
		}
		else { GL(glDisable(GL_LINE_SMOOTH)); }

		if (bScissorTest) {
			GL(glEnable(GL_SCISSOR_TEST));
		}
		else { GL(glDisable(GL_SCISSOR_TEST)); }

		if (bPolygonOffsetFill) {
			GL(glEnable(GL_POLYGON_OFFSET_FILL));
			GL(glPolygonOffset(polygonOffsetFill.factor, polygonOffsetFill.units));
		}
		else { GL(glDisable(GL_POLYGON_OFFSET_FILL)); }

		if (bMultiSample) { GL(glEnable(GL_MULTISAMPLE)); }
		else { GL(glDisable(GL_MULTISAMPLE)); }
	}
};

struct BlendState
{
	BlendState()
	{
		//bAlphaTest = false;
		//alphaTest.alphaFunc = EGlFunc_ALWAYS;
		//alphaTest.alphaRef = 0.f;

		bIndependentBlend = false; // If you want this true, you'll have to set up to N (maybe 8 ?) blendArgs structs, ala DX11

		bBlend = false;
		blend.srcFactorRgb = EGlSrcFactor_ONE;
		blend.dstFactorRgb = EGlDstFactor_ZERO;

		blend.srcFactorAlpha = EGlSrcFactor_ONE;
		blend.dstFactorAlpha = EGlDstFactor_ZERO;

		blend.opRgb = EGlBlendOp_FUNC_ADD;
		blend.opAlpha = EGlBlendOp_FUNC_ADD;

		blend.factor.r = 0.f;
		blend.factor.g = 0.f;
		blend.factor.b = 0.f;
		blend.factor.a = 0.f;

		blendMask.r = true;
		blendMask.g = true;
		blendMask.b = true;
		blendMask.a = true;

		bAlphaToCoverage = false;
	}

	//bool bAlphaTest = false;
	//struct {
	//	EGlFunc alphaFunc = EGlFunc_ALWAYS;
	//	float alphaRef = 0.f;
	//} alphaTest;	

	bool bIndependentBlend = false; // If you want this true, you'll have to set up to N (maybe 8 ?) blendArgs structs, ala DX11

	bool bBlend = false;
	struct {
		EGlSrcFactor srcFactorRgb = EGlSrcFactor_ONE;
		EGlDstFactor dstFactorRgb = EGlDstFactor_ZERO;

		EGlSrcFactor srcFactorAlpha = EGlSrcFactor_ONE;
		EGlDstFactor dstFactorAlpha = EGlDstFactor_ZERO;

		EGlBlendOp opRgb = EGlBlendOp_FUNC_ADD;
		EGlBlendOp opAlpha = EGlBlendOp_FUNC_ADD;

		struct {
			float r = 0.f;
			float g = 0.f;
			float b = 0.f;
			float a = 0.f;
		} factor;
	} blend;

	struct {
		bool r = true;
		bool g = true;
		bool b = true;
		bool a = true;
	} blendMask;

	bool bAlphaToCoverage = false;


	void BindState() const
	{
		if (bAlphaToCoverage) { GL(glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE)); }
		else { GL(glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE)); }

		if (bBlend) {
			GL(glEnable(GL_BLEND));
			GL(glBlendFuncSeparate(blend.srcFactorRgb, blend.dstFactorRgb, blend.srcFactorAlpha, blend.srcFactorAlpha));
			GL(glBlendEquationSeparate(blend.opRgb, blend.opAlpha));

			GL(glBlendColor(blend.factor.r, blend.factor.g, blend.factor.b, blend.factor.a));
		}
		else { GL(glDisable(GL_BLEND)); }

		GL(glColorMask(blendMask.r, blendMask.g, blendMask.b, blendMask.a));

		//if (bAlphaTest) {
		//	GL(glEnable(GL_ALPHA_TEST));
		//	GL(glAlphaFunc(alphaTest.alphaFunc, alphaTest.alphaRef));
		//}
		//else { GL(glDisable(GL_ALPHA_TEST)); }
	}
};

struct PipelineState
{
public:
	BlendState m_blendState;
	RasterizerState m_rasterizerState;
	DepthStencilState m_depthStencilState;

	void BindPipelineState() const
	{
		m_blendState.BindState();
		m_rasterizerState.BindState();
		m_depthStencilState.BindState();
	}
};

