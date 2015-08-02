//
//  arkParamsPanel.cpp
//  arkGUI
//
//  Created by anthonycouret on 17/02/2015.
//  Copyright (c) 2015 ac. All rights reserved.
//

#include "arkParamsPanel.h"

arkParamsPanelShPtr arkParamsPanel::create( arkAbstractMediatorShPtr mediator_shptr )
{
    arkParamsPanelShPtr arkParamsPanel_sh_ptr( new arkParamsPanel( mediator_shptr ) );
    arkParamsPanel_sh_ptr->m_weak_ptr = arkParamsPanel_sh_ptr;
    return arkParamsPanel_sh_ptr;
}

arkParamsPanel::arkParamsPanel( arkAbstractMediatorShPtr mediator_shptr ) :
QWidget(),
arkMediatorWidget( mediator_shptr )
{
    m_layout = new QGridLayout();
    setLayout( m_layout );
    
    m_tesselation_factor = new QSpinBox();
    m_disp_factor = new QSpinBox();
    m_nbSamples = new QSpinBox();
    m_disp_factor->setValue(0);

    m_brdf_choice = new QComboBox();
    m_brdf_choice->addItem( tr("LEADR") );
    m_brdf_choice->addItem( tr("Classic Microfacets") );
    m_brdf_choice->addItem( tr("Classic Phong") );
    
//    m_normal_mode = new QComboBox();
//    m_normal_mode->addItem( tr("Enabled") );
//    m_normal_mode->addItem( tr("Disabled") );

    m_filtering = new QCheckBox("Filtering", this);

    m_diffuse = new QCheckBox("Diffuse", this);
    m_specular_direct = new QCheckBox("Specular PointLights", this);
    m_specular_env = new QCheckBox("Specular Environment", this);
    m_diffuse_direct = new QCheckBox("Diffuse PointLights", this);
    m_diffuse_env = new QCheckBox("Diffuse Environment", this);

    m_filtering->setCheckState(Qt::Checked);

    m_diffuse->setCheckState(Qt::Checked);
    m_specular_direct->setCheckState(Qt::Checked);
    m_specular_env->setCheckState(Qt::Checked);
    m_diffuse_direct->setCheckState(Qt::Checked);
    m_diffuse_env->setCheckState(Qt::Checked);

    m_reload_button = new QPushButton( tr("Reload Shader") );

    m_reload_shortcut = new QShortcut(QKeySequence("Ctrl+R"), this);


    m_roughness_offset = new QDoubleSpinBox();
    m_roughness_offset->setMinimum(0);
    m_roughness_offset->setMaximum(100);
    m_roughness_offset->setSingleStep(0.01);

    m_fresnel0 = new QDoubleSpinBox();
    m_fresnel0->setMinimum(0);
    m_fresnel0->setMaximum(1);
    m_fresnel0->setSingleStep(0.01);
    m_fresnel0->setValue(0.05);


    m_layout->addWidget( new QLabel( tr("tesselation factor : ") ), 0, 0, 2, 8 );
    m_layout->addWidget( m_tesselation_factor, 0, 8, 2, 2 );
    m_tesselation_factor->setValue(1);
    m_tesselation_factor->setMinimum(1);

    m_layout->addWidget( new QLabel( tr("displacement factor : ") ), 1, 0, 2, 8 );
    m_layout->addWidget( m_disp_factor, 1, 8, 2, 2 );
    
    m_layout->addWidget( new QLabel( tr("nb samples : ") ), 2, 0, 2, 8 );
    m_layout->addWidget( m_nbSamples, 2, 8, 2, 2 );
    m_nbSamples->setValue(1);
    m_nbSamples->setMinimum(1);
    m_nbSamples->setMaximum(256);

    m_layout->addWidget( new QLabel( tr("Roughness offset : ") ), 3, 0, 2, 8 );
    m_layout->addWidget( m_roughness_offset, 3, 8, 2, 2 );

    m_layout->addWidget( new QLabel( tr("Fresnel0 : ") ), 4, 0, 2, 8 );
    m_layout->addWidget( m_fresnel0, 4, 8, 2, 2 );
    
    m_layout->addWidget( new QLabel( tr("BRDF : ") ), 6, 0, 2, 6 );
    m_layout->addWidget( m_brdf_choice, 6, 6, 2, 4 );

    // ROBIN'S LAYOUT
    m_layout->addWidget( m_filtering, 8, 0, 2, 6 );

//    m_layout->addWidget( m_diffuse, 9, 0, 2, 6 );
    m_layout->addWidget( m_diffuse_direct, 9, 0, 2, 6 );
    m_layout->addWidget( m_diffuse_env, 9, 6, 2, 6 );
    m_layout->addWidget( m_specular_direct, 10, 0, 2, 6 );
    m_layout->addWidget( m_specular_env, 10, 6, 2, 6 );
    
    m_layout->addWidget( m_reload_button, 12, 0, 2, 6 );


    // ANTHOS'S LAYOUT PROPOSITION
//    m_layout->addWidget( m_filtering, 8, 0, 2, 6 );

//   m_layout->addWidget( m_diffuse, 9, 0, 2, 6 );
//   m_layout->addWidget( m_specular_direct, 10, 0, 2, 8 );
//   m_layout->addWidget( m_specular_env, 11, 0, 2, 8 );

//   m_layout->addWidget( m_reload_button, 12, 0, 2, 6 );

    connect
    ( m_tesselation_factor, SIGNAL( valueChanged(int) ), this, SLOT( updateTesselationFactor(int) ) );
    
    connect( m_disp_factor, SIGNAL( valueChanged(int) ), this, SLOT( updateDispFactor(int) ) );
    
    connect( m_nbSamples, SIGNAL( valueChanged(int) ), this, SLOT( updateNbSamples(int) ) );

    connect( m_roughness_offset, SIGNAL( valueChanged(double) ), this, SLOT( updateRoughnessOffset(double) ) );

    connect( m_fresnel0, SIGNAL( valueChanged(double) ), this, SLOT( updateFresnel0(double) ) );
    
    connect( m_brdf_choice, SIGNAL( currentIndexChanged(int) ), this, SLOT( setBRDF(int) ) );
    
//    connect( m_normal_mode, SIGNAL( currentIndexChanged(int) ), this, SLOT( setNormalMode(int) ) );

    connect( m_reload_button, SIGNAL( released() ), this, SLOT( reloadShader() ) );

    QObject::connect(m_reload_shortcut, SIGNAL(activated()), this, SLOT( reloadShader() ) );

    QObject::connect ( m_filtering, SIGNAL(stateChanged(int)), this, SLOT(setFilteringMode(int)) );

    connect ( m_diffuse, SIGNAL(stateChanged(int)), this, SLOT(setDiffuseEnabled(int)) );
    connect ( m_specular_direct, SIGNAL(stateChanged(int)), this, SLOT(setSpecularDirectEnabled(int)) );
    connect ( m_specular_env, SIGNAL(stateChanged(int)), this, SLOT(setSpecularEnvEnabled(int)) );
    connect ( m_diffuse_direct, SIGNAL(stateChanged(int)), this, SLOT(setDiffuseDirectEnabled(int)) );
    connect ( m_diffuse_env, SIGNAL(stateChanged(int)), this, SLOT(setDiffuseEnvEnabled(int)) );
}

arkParamsPanel::~arkParamsPanel()
{
    
}

void arkParamsPanel::setBRDF( int index )
{
    m_mediator_shptr->setBRDF( index );
}

void arkParamsPanel::setNormalMode( int index )
{
    m_mediator_shptr->setNormalMode( index == 0 ? true : false );
}

void arkParamsPanel::updateTesselationFactor(int value)
{
    m_mediator_shptr->setTesselationFactor( value );
}

void arkParamsPanel::updateNbSamples( int value )
{
    m_mediator_shptr->setNbSamples( value );
}

void arkParamsPanel::updateDispFactor( int value )
{
    m_mediator_shptr->setDispFactor( value );
}

void arkParamsPanel::updateRoughnessOffset( double value )
{
    m_mediator_shptr->setRoughnessOffset(value);
}

void arkParamsPanel::updateFresnel0( double value )
{
    m_mediator_shptr->setFresnel0(value);
}

void arkParamsPanel::reloadShader()
{
    m_mediator_shptr->reloadShader();
}

void arkParamsPanel::setFilteringMode( int state )
{
    bool enabled = (state == Qt::Checked);

    m_mediator_shptr->setFilteringMode(enabled);
}

void arkParamsPanel::setDiffuseEnabled(int state )
{
    bool enabled = (state == Qt::Checked);

    m_mediator_shptr->setDiffuseEnabled(enabled);
}

void arkParamsPanel::setSpecularDirectEnabled( int state )
{
    bool enabled = (state == Qt::Checked);

    m_mediator_shptr->setSpecularDirectEnabled(enabled);
}

void arkParamsPanel::setSpecularEnvEnabled(int state)
{
    bool enabled = (state == Qt::Checked);

    m_mediator_shptr->setSpecularEnvEnabled(enabled);
}

void arkParamsPanel::setDiffuseDirectEnabled( int state )
{
    bool enabled = (state == Qt::Checked);

    m_mediator_shptr->setDiffuseDirectEnabled(enabled);
}

void arkParamsPanel::setDiffuseEnvEnabled(int state)
{
    bool enabled = (state == Qt::Checked);

    m_mediator_shptr->setDiffuseEnvEnabled(enabled);
}

