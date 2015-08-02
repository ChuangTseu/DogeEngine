//
//  arkMediator.h
//  arkGUI
//
//  Created by anthonycouret on 17/02/2015.
//  Copyright (c) 2015 ac. All rights reserved.
//

#ifndef __arkGUI__arkMediator__
#define __arkGUI__arkMediator__

#include <memory>
#include <vector>

#include "arkQTGUI/arkAbstractMediator.h"
#include "Graphics/renderer.h"

template <class TRENDERER>
class arkMediator : public arkAbstractMediator
{
public :
		 
	typedef std::shared_ptr<arkMediator<TRENDERER>> ShPtr;
	typedef std::weak_ptr<arkMediator<TRENDERER>> WkPtr;

	static ShPtr create(TRENDERER * renderer)
	{
		ShPtr arkMediator_sh_ptr(new arkMediator<TRENDERER>(renderer));
		arkMediator_sh_ptr->m_weak_ptr = arkMediator_sh_ptr;
		return arkMediator_sh_ptr;
	}
    
	void setTesselationFactor(float tesselation_factor)
	{
		m_tesselation_factor = tesselation_factor;
		m_renderer->setTesselationFactor(m_tesselation_factor);
	}

	void setDispFactor(float disp_factor)
	{
		m_disp_factor = disp_factor;
		m_renderer->setDispFactor(m_disp_factor);
	}

	void setNbSamples(int nb_samples)
	{
		m_nb_samples = nb_samples;
		m_renderer->setNbSamples(m_nb_samples);
	}

	void setRoughnessOffset(double roughnessOffset)
	{
		m_renderer->setRoughnessOffset(roughnessOffset);
	}

	void setFresnel0(double fresnel0)
	{
		m_renderer->setFresnel0(fresnel0);
	}

	void loadModel(const std::string &model_path)
	{
		m_renderer->loadModel(model_path);
	}

	void loadLEADRTexture(const std::string & leadr)
	{
		m_renderer->loadLEADRTexture(leadr);
	}

	void loadTexture(const std::string & texture)
	{
		m_renderer->loadTexture(texture);
	}

	void loadDispMap(const std::string & disp_map)
	{
		m_renderer->loadDispMap(disp_map);
	}

	void loadNormalMap(const std::string & normal_map)
	{
		m_renderer->loadTexture(normal_map);
	}

	void loadEnvTexture(const std::string & env_path)
	{
		m_renderer->loadEnvTexture(env_path);
	}

	void loadIrradianceMap(const std::string & irr_path)
	{
		m_renderer->loadIrradianceMap(irr_path);
	}

	void setStandardRendering()
	{
		m_renderer->setFinalFboTarget(0);
	}

	void setWireframeRendering()
	{
		m_renderer->setWireframeRendering();
	}

	void setDepthRendering()
	{
		m_renderer->setDepthRendering();
	}

	void setNormalRendering()
	{
		m_renderer->setFinalFboTarget(1);
	}

	void setTexcoordRendering()
	{
		m_renderer->setFinalFboTarget(2);
	}

	void setBRDF(int brdf_choice)
	{
		m_renderer->setBRDF(brdf_choice);
	}

	void setNormalMode(bool normal_mode)
	{
		m_renderer->setNormalMode(normal_mode);
	}

	void reloadShader()
	{
		m_renderer->reloadShader();
	}

	void setFilteringMode(bool enabled)
	{
		m_renderer->setFilteringMode(enabled);
	}

	void setDiffuseEnabled(bool enabled)
	{
		m_renderer->setDiffuseEnabled(enabled);
	}

	void setSpecularDirectEnabled(bool enabled)
	{
		m_renderer->setSpecularDirectEnabled(enabled);
	}

	void setSpecularEnvEnabled(bool enabled)
	{
		m_renderer->setSpecularEnvEnabled(enabled);
	}

	void setDiffuseDirectEnabled(bool enabled)
	{
		m_renderer->setDiffuseDirectEnabled(enabled);
	}

	void setDiffuseEnvEnabled(bool enabled)
	{
		m_renderer->setDiffuseEnvEnabled(enabled);
	}

	void initializeGL()
	{
		m_renderer->initializeGL();
	}

	void resizeGL(int width, int height)
	{
		m_renderer->resizeGL(width, height);
	}

	void paintGL()
	{
		m_renderer->paintGL();
	}

	void rotateCamera(int mouse_x_rel, int mouse_y_rel)
	{
		m_renderer->rotateCamera(mouse_x_rel, mouse_y_rel);
	}


	void translateCamera(int mouse_x_rel, int mouse_y_rel, int mouse_z_rel)
	{
		m_renderer->translateCamera(mouse_x_rel, mouse_y_rel, mouse_y_rel);
	}

	void onKeyPress(int qt_key)
	{
		m_renderer->onKeyPress(qt_key);
	}

private :
    

	arkMediator(TRENDERER * renderer)
	{
		m_renderer = renderer;
	}

    WkPtr m_weak_ptr;
    
	TRENDERER * m_renderer;
    
    float m_tesselation_factor;
    float m_disp_factor;
    float m_nb_samples;
    
    
};

#endif /* defined(__arkGUI__arkMediator__) */
