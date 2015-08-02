//
//  arkMediatorWidget.cpp
//  arkGUI
//
//  Created by anthonycouret on 17/02/2015.
//  Copyright (c) 2015 ac. All rights reserved.
//

#include "arkMediatorWidget.h"

arkMediatorWidget::arkMediatorWidget( arkAbstractMediatorShPtr mediator_shptr )
{
    m_mediator_shptr = mediator_shptr;
}

arkMediatorWidget::~arkMediatorWidget()
{
    
}
