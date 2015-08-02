//
//  arkMainWindow.h
//  arkGUI
//
//  Created by anthonycouret on 17/02/2015.
//  Copyright (c) 2015 ac. All rights reserved.
//

#ifndef __arkGUI__arkMainWindow__
#define __arkGUI__arkMainWindow__

#include <memory>
#include <QMainWindow>

#include "arkMediatorWidget.h"
#include "arkAbstractMediator.h"
#include "arkMenuBar.h"
#include "arkOpenGLWidget.h"
#include "arkParamsPanel.h"

class arkMainWindow;
typedef std::shared_ptr<arkMainWindow> arkMainWindowShPtr;
typedef std::weak_ptr<arkMainWindow> arkMainWindowWkPtr;


class arkMainWindow : public QMainWindow, public arkMediatorWidget
{
    public :
    
    static arkMainWindowShPtr create( int width, int height, arkAbstractMediatorShPtr mediator_shptr );
    
    ~arkMainWindow();
    
    private :
    
    arkMainWindow( int width, int height, arkAbstractMediatorShPtr mediator_shptr );
    arkMainWindowWkPtr m_weak_ptr;
    
    arkMenuBarShPtr m_menu_bar;
    arkOpenGLWidgetShPtr m_ogl_widget;
    arkParamsPanelShPtr m_params_panel;
};

#endif /* defined(__arkGUI__arkMainWindow__) */
