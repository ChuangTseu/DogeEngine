//
//  arkMediatorWidget.h
//  arkGUI
//
//  Created by anthonycouret on 17/02/2015.
//  Copyright (c) 2015 ac. All rights reserved.
//

#ifndef __arkGUI__arkMediatorWidget__
#define __arkGUI__arkMediatorWidget__

#include <iostream>

#include "arkAbstractMediator.h"
#include <QWidget>

class arkMediatorWidget;
typedef std::shared_ptr<arkMediatorWidget> arkMediatorWidgetShPtr;
typedef std::weak_ptr<arkMediatorWidget> arkMediatorWidgetWkPtr;

class arkMediatorWidget
{
    protected :
    
    arkMediatorWidget( arkAbstractMediatorShPtr mediator );
    arkAbstractMediatorShPtr m_mediator_shptr;
    
    public :
    
    ~arkMediatorWidget();
};


#endif /* defined(__arkGUI__arkMediatorWidget__) */
