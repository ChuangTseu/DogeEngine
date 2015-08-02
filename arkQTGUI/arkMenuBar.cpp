//
//  arkMenuBar.cpp
//  arkGUI
//
//  Created by anthonycouret on 17/02/2015.
//  Copyright (c) 2015 ac. All rights reserved.
//

#include "arkMenuBar.h"

arkMenuBarShPtr arkMenuBar::create( arkAbstractMediatorShPtr mediator_shptr )
{
    arkMenuBarShPtr arkMenuBar_sh_ptr( new arkMenuBar( mediator_shptr ) );
    arkMenuBar_sh_ptr->m_weak_ptr = arkMenuBar_sh_ptr;
    return arkMenuBar_sh_ptr;
}

arkMenuBar::arkMenuBar( arkAbstractMediatorShPtr mediator_shptr ) :
QMenuBar(),
arkMediatorWidget( mediator_shptr )
{
    setObjectName( QString::fromUtf8( "LEADR menu bar" ) );
    
    QMenu * file_menu = new QMenu( tr( "&File" ), this );
    QMenu * rendering_menu = new QMenu( tr( "&Rendering" ), this );
    
    QAction * load_model_action = file_menu->addAction( tr( "&Load Model" ) );
    
//    QAction * load_texture_action = file_menu->addAction( tr( "&Load Texture" ) );
//    QAction * load_displacement_action = file_menu->addAction( tr( "&Load Disp Map" ) );
//    QAction * load_normal_action = file_menu->addAction( tr( "&Load Normal Map" ) );

    QAction * load_leadr_action = file_menu->addAction( tr( "&Load LEADR Texture" ) );

    QAction * load_env_action = file_menu->addAction( tr( "&Load Env" ) );
    QAction * load_irr_map_action = file_menu->addAction( tr( "&Load Irr Map" ) );
    
    QAction * standard_action = rendering_menu->addAction( tr( "&Standard" ) ,
                                                           this, SLOT(setStandardRendering()), QKeySequence(Qt::CTRL + Qt::Key_0));
    QAction * normal_action = rendering_menu->addAction( tr( "&Normal" ) ,
                                                           this, SLOT(setNormalRendering()), QKeySequence(Qt::CTRL + Qt::Key_1));
    QAction * texcoord_action = rendering_menu->addAction( tr( "&Texcoord" ) ,
                                                           this, SLOT(setTexcoordRendering()), QKeySequence(Qt::CTRL + Qt::Key_2));
    QAction * wireframe_action = rendering_menu->addAction( tr( "&Wireframe" ) ,
                                                            this, SLOT(setWireframeRendering()), QKeySequence(Qt::CTRL + Qt::Key_F));
    QAction * depth_action = rendering_menu->addAction( tr( "&Depth" ) );
    
    
    connect( load_model_action, SIGNAL( triggered() ), this, SLOT( loadModel() ) );

//    connect( load_texture_action, SIGNAL( triggered() ), this, SLOT( loadTexture() ) );
//    connect( load_normal_action, SIGNAL( triggered() ), this, SLOT( loadNormalMap() ) );
//    connect( load_displacement_action, SIGNAL( triggered() ), this, SLOT( loadDispMap() ) );

    connect( load_leadr_action, SIGNAL( triggered() ), this, SLOT( loadLEADRTexture() ) );
    
    connect( load_env_action, SIGNAL( triggered() ), this, SLOT( loadEnvTexture() ) );
    connect( load_irr_map_action, SIGNAL( triggered() ), this, SLOT( loadIrradianceMap() ) );

//    connect( standard_action, SIGNAL( triggered() ), this, SLOT( setStandardRendering() ) );
//    connect( normal_action, SIGNAL( triggered() ), this, SLOT( setNormalRendering() ) );
//    connect( texcoord_action, SIGNAL( triggered() ), this, SLOT( setTexcoordRendering() ) );
//    connect( wireframe_action, SIGNAL( triggered() ), this, SLOT( setWireframeRendering() ) );
//    connect( depth_action, SIGNAL( triggered() ), this, SLOT( setDepthRendering() ) );
    
    addMenu( file_menu );
    addMenu( rendering_menu );
}

arkMenuBar::~arkMenuBar()
{
    
}

void arkMenuBar::loadModel()
{
    QString file_path =
    QFileDialog::getOpenFileName( this, tr( "Select Model file" ), "",
                                  tr( "Model Files (*.obj *.dae *.blend *.3ds )" ) );

    if ( ! file_path.isEmpty() )
    {
        m_mediator_shptr->loadModel( file_path.toStdString() );
    }
}

void arkMenuBar::loadTexture()
{
    QString file_path =
    QFileDialog::getOpenFileName( this, tr( "Select texture" ), "", tr( "Texture ( *.jpg )" ) );
    
    if ( ! file_path.isEmpty() )
    {
        m_mediator_shptr->loadTexture( file_path.toStdString() );
    }
}

void arkMenuBar::loadDispMap()
{
    QString file_path =
    QFileDialog::getOpenFileName( this, tr( "Select displacement Map" ), "", tr( "Displacement map ( *.jpg )" ) );
    
    if ( ! file_path.isEmpty() )
    {
        m_mediator_shptr->loadDispMap( file_path.toStdString() );
    }
}

void arkMenuBar::loadNormalMap()
{
    QString file_path =
    QFileDialog::getOpenFileName( this, tr( "Select normal Map" ), "", tr( "Normal map ( *.jpg )" ) );
    
    if ( ! file_path.isEmpty() )
    {
        m_mediator_shptr->loadNormalMap( file_path.toStdString() );
    }
}

void arkMenuBar::loadLEADRTexture()
{
    QString file_path =
    QFileDialog::getOpenFileName( this, tr( "Select LEADR texture" ), "", tr( "LEADR texture ( *.leadr )" ) );

    if ( ! file_path.isEmpty() )
    {
        m_mediator_shptr->loadLEADRTexture( file_path.toStdString() );
    }
}

void arkMenuBar::loadEnvTexture()
{

    QString file_path = QFileDialog::getOpenFileName( this, tr( "Select Environnement file" ), "", tr( "Model Files (*.hdr *.float)" ) );
    
    if ( ! file_path.isEmpty() )
    {
        m_mediator_shptr->loadEnvTexture( file_path.toStdString() );
    }
}

void arkMenuBar::loadIrradianceMap()
{
    QString file_path =
    QFileDialog::getOpenFileName( this, tr( "Select Irradiance file" ), "", tr( "LEADR Irr file (*.shc)" ) );

    if ( ! file_path.isEmpty() )
    {
        m_mediator_shptr->loadIrradianceMap( file_path.toStdString() );
    }
}

void arkMenuBar::setStandardRendering()
{
    m_mediator_shptr->setStandardRendering();
}

void arkMenuBar::setWireframeRendering()
{
    m_mediator_shptr->setWireframeRendering();
}

void arkMenuBar::setDepthRendering()
{
    m_mediator_shptr->setDepthRendering();
}

void arkMenuBar::setNormalRendering()
{
    m_mediator_shptr->setNormalRendering();
}

void arkMenuBar::setTexcoordRendering()
{
    m_mediator_shptr->setTexcoordRendering();
}
