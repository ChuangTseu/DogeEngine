//
//  arkMainWindow.cpp
//  arkGUI
//
//  Created by anthonycouret on 17/02/2015.
//  Copyright (c) 2015 ac. All rights reserved.
//

#include "arkMainWindow.h"

#include <QDockWidget>
#include <QSettings>

arkMainWindowShPtr arkMainWindow::create
( int width, int height, arkAbstractMediatorShPtr mediator_shptr )
{
    arkMainWindowShPtr arkMainWindow_sh_ptr( new arkMainWindow( width, height, mediator_shptr ) );
    arkMainWindow_sh_ptr->m_weak_ptr = arkMainWindow_sh_ptr;
    return arkMainWindow_sh_ptr;
}

arkMainWindow::arkMainWindow
( int width, int height, arkAbstractMediatorShPtr mediator_shptr ) :
QMainWindow(),
arkMediatorWidget( mediator_shptr )
{
    resize(width, height);
    
    m_menu_bar = arkMenuBar::create( m_mediator_shptr );
    setMenuBar( m_menu_bar.get() );

    //UGLY, BUT SHOULD WORK FOR OUR MODEST NEEDS
#ifdef WIN32
    unsigned int opengl_version_flag = QGLFormat::OpenGL_Version_4_2;
#else
    unsigned int opengl_version_flag = QGLFormat::OpenGL_Version_4_1;
#endif
    
    QGLFormat fmt;
    fmt.setDoubleBuffer(true);
    fmt.setDirectRendering(true);
    fmt.setRgba(true);
    fmt.setStencil(false);
    fmt.setDepth(true);
    fmt.setAlpha(false);

    if (opengl_version_flag == QGLFormat::OpenGL_Version_4_0)
        fmt.setVersion(4, 0);
    else if (opengl_version_flag == QGLFormat::OpenGL_Version_4_1)
        fmt.setVersion(4, 1);
	else if (opengl_version_flag == QGLFormat::OpenGL_Version_4_2)
		fmt.setVersion(4, 2);
	else if (opengl_version_flag == QGLFormat::OpenGL_Version_4_3)
		fmt.setVersion(4, 3);
    else {
        std::cout << "Asked OpenGL version is unsupported. \n";
        exit(EXIT_FAILURE);
    }

    fmt.setProfile(QGLFormat::CoreProfile);
    
    if (!((QGLFormat::openGLVersionFlags() & opengl_version_flag) && (QGLFormat::openGLVersionFlags() & QGLFormat::CoreProfile)))
    {
        if (opengl_version_flag == QGLFormat::OpenGL_Version_4_0)
            std::cout << "Failed to create a valid Core Opengl 4.0 Profile. \n";
        else if (opengl_version_flag == QGLFormat::OpenGL_Version_4_1)
            std::cout << "Failed to create a valid Core Opengl 4.1 Profile. \n";
		else if (opengl_version_flag == QGLFormat::OpenGL_Version_4_2)
			std::cout << "Failed to create a valid Core Opengl 4.2 Profile. \n";
		else if (opengl_version_flag == QGLFormat::OpenGL_Version_4_3)
			std::cout << "Failed to create a valid Core Opengl 4.3 Profile. \n";

        exit(EXIT_FAILURE);
    }
    
    QDockWidget * dock_ogl_widget = new QDockWidget( "OpenGL Widget" );
    m_ogl_widget = arkOpenGLWidget::create( fmt, m_mediator_shptr );
    dock_ogl_widget->setWidget( m_ogl_widget.get() );
    addDockWidget(Qt::LeftDockWidgetArea, dock_ogl_widget );
    dock_ogl_widget->setAllowedAreas(Qt::LeftDockWidgetArea);
//    dock_ogl_widget->setMaximumWidth(width - 50);

//    setDockNestingEnabled(true);
//    setCentralWidget(0);
//    setDockOptions(QMainWindow::AnimatedDocks | QMainWindow::AllowNestedDocks |
//                   QMainWindow::AllowTabbedDocks | QMainWindow::ForceTabbedDocks |
//                   QMainWindow::VerticalTabs);

    QDockWidget * dock_params_widget = new QDockWidget( "Params Widget" );
    m_params_panel = arkParamsPanel::create( m_mediator_shptr );
    dock_params_widget->setWidget( m_params_panel.get() );
    addDockWidget(Qt::RightDockWidgetArea, dock_params_widget );
    dock_params_widget->setMaximumWidth( width / 3 );
    dock_params_widget->setAllowedAreas(Qt::RightDockWidgetArea);
}

arkMainWindow::~arkMainWindow()
{
    
}


