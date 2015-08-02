//
//  arkParamsPanel.h
//  arkGUI
//
//  Created by anthonycouret on 17/02/2015.
//  Copyright (c) 2015 ac. All rights reserved.
//

#ifndef __arkGUI__arkParamsPanel__
#define __arkGUI__arkParamsPanel__

#include <memory>
#include <QWidget>
#include <QSpinBox>
#include <QComboBox>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QShortcut>
#include <QRadioButton>
#include <QCheckBox>
#include "arkMediatorWidget.h"

class arkParamsPanel;
typedef std::shared_ptr<arkParamsPanel> arkParamsPanelShPtr;
typedef std::weak_ptr<arkParamsPanel> arkParamsPanelWkPtr;

class arkParamsPanel : public QWidget, public arkMediatorWidget
{
    Q_OBJECT
    
    public :
    
    static arkParamsPanelShPtr create( arkAbstractMediatorShPtr mediator_shptr );
    ~arkParamsPanel();
    
    private :
    
    arkParamsPanel( arkAbstractMediatorShPtr mediator_shptr );
    arkParamsPanelWkPtr m_weak_ptr;
    
    QGridLayout * m_layout;
    
    QSpinBox * m_tesselation_factor;
    QSpinBox * m_nbSamples;
    QSpinBox * m_disp_factor;
    
    QComboBox * m_brdf_choice;
    QComboBox * m_normal_mode;

    QPushButton * m_reload_button;

    QShortcut *m_reload_shortcut;

    QCheckBox * m_filtering;

    QCheckBox * m_diffuse;
    QCheckBox * m_specular_direct;
    QCheckBox * m_specular_env;
    QCheckBox * m_diffuse_direct;
    QCheckBox * m_diffuse_env;


    QDoubleSpinBox * m_roughness_offset;

    QDoubleSpinBox * m_fresnel0;
    
    public slots :
    
    void setBRDF(int);
    void setNormalMode(int);
    void updateTesselationFactor(int);
    void updateNbSamples(int);
    void updateDispFactor(int);
    void updateRoughnessOffset(double);
    void reloadShader();
    void setFilteringMode(int state);
    void setDiffuseEnabled(int state);
    void setSpecularDirectEnabled(int state);
    void setSpecularEnvEnabled(int state);
    void updateFresnel0(double);
    void setDiffuseDirectEnabled(int state);
    void setDiffuseEnvEnabled(int state);
};

#endif /* defined(__arkGUI__arkParamsPanel__) */
