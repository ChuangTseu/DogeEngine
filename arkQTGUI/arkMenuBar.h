//
//  arkMenuBar.h
//  arkGUI
//
//  Created by anthonycouret on 17/02/2015.
//  Copyright (c) 2015 ac. All rights reserved.
//

#ifndef __arkGUI__arkMenuBar__
#define __arkGUI__arkMenuBar__

#include <memory>
#include <QMenuBar>
#include <QFileDialog>
#include "arkMediatorWidget.h"

class arkMenuBar;
typedef std::shared_ptr<arkMenuBar> arkMenuBarShPtr;
typedef std::weak_ptr<arkMenuBar> arkMenuBarWkPtr;

class arkMenuBar : public QMenuBar, public arkMediatorWidget
{
    
    Q_OBJECT
    
    public :
    
    static arkMenuBarShPtr create( arkAbstractMediatorShPtr mediator_shptr );
    ~arkMenuBar();
    
    private :
    
    arkMenuBar( arkAbstractMediatorShPtr mediator_shptr );
    arkMenuBarWkPtr m_weak_ptr;
    
    public slots :
    
    void loadModel();
    
    void loadLEADRTexture();
    void loadTexture();
    void loadDispMap();
    void loadNormalMap();

    void loadEnvTexture();
    void loadIrradianceMap();
    
    void setStandardRendering();
    void setWireframeRendering();
    void setDepthRendering();
    void setNormalRendering();
    void setTexcoordRendering();
};

#endif /* defined(__arkGUI__arkMenuBar__) */
