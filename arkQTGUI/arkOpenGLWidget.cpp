//
//  arkOpenGLWidget.cpp
//  arkGUI
//
//  Created by anthonycouret on 17/02/2015.
//  Copyright (c) 2015 ac. All rights reserved.
//

#include "arkOpenGLWidget.h"
#include <QTimer>
arkOpenGLWidgetShPtr arkOpenGLWidget::create( QGLFormat format, arkAbstractMediatorShPtr mediator_shptr )
{
    arkOpenGLWidgetShPtr arkOpenGLWidget_sh_ptr( new arkOpenGLWidget( format, mediator_shptr ) );
    arkOpenGLWidget_sh_ptr->m_weak_ptr = arkOpenGLWidget_sh_ptr;
    return arkOpenGLWidget_sh_ptr;
}

arkOpenGLWidget::arkOpenGLWidget( QGLFormat format, arkAbstractMediatorShPtr mediator_shptr ) :
QGLWidget( format ),
arkMediatorWidget( mediator_shptr )
{
    setFocusPolicy(Qt::StrongFocus);
}

arkOpenGLWidget::~arkOpenGLWidget()
{
    
}

void arkOpenGLWidget::initializeGL(void) {
    
    m_mediator_shptr->initializeGL();
    
    m_realTimeTimer = new QTimer(this);
    m_realTimeTimer->setInterval(8);
//    
    connect(m_realTimeTimer, SIGNAL(timeout()), this, SLOT(update()));
//    
    m_realTimeTimer->start();
}

void arkOpenGLWidget::resizeGL( int width, int height )
{
    m_mediator_shptr->resizeGL( width, height );
}

void arkOpenGLWidget::paintGL(void) {
//    Timer timer;
//    timer.start();
    
    m_mediator_shptr->paintGL();
//    
//    timer.stop();
//    
//    getMainWindow()->displayRenderTime(timer.getElapsedTimeInMilliSec());
}

void arkOpenGLWidget::mousePressEvent(QMouseEvent* event)
{
    m_lastMousePos.rx() = event->x();
    m_lastMousePos.ry() = event->y();
}

void arkOpenGLWidget::mouseMoveEvent(QMouseEvent* event)
{
    QPoint currentMousePos(event->x(), event->y());
    
    
    QPoint diff = currentMousePos - m_lastMousePos;
    
    if (event->modifiers() & Qt::ControlModifier) {
        m_mediator_shptr->translateCamera( diff.x(), diff.y(), 0 );
        m_mediator_shptr->translateCamera(diff.x(), diff.y(), 0);
    }
    else if (event->modifiers() & Qt::ShiftModifier) {
        m_mediator_shptr->translateCamera(0, 0, -diff.y());
    }
    else {
        m_mediator_shptr->rotateCamera(diff.x(), diff.y());
    }
    
    m_lastMousePos = currentMousePos;
}

void arkOpenGLWidget::mouseReleaseEvent(QMouseEvent* event)
{
}

void arkOpenGLWidget::wheelEvent(QWheelEvent* event)
{
    m_mediator_shptr->translateCamera(0, 0, event->delta() / 10);
}

void arkOpenGLWidget::keyPressEvent(QKeyEvent* event)
{
    m_mediator_shptr->onKeyPress(event->key());
}


//void OpenGLWidget::reloadShader()
//{
//    m_renderer->reloadShader();
//}
//
//void OpenGLWidget::setFinalFboTarget(int targetIndex)
//{
//    m_renderer->setFinalFboTarget(targetIndex);
//}
//
//void OpenGLWidget::keyValueChanged(double value)
//{
//    m_renderer->keyValueChanged(value);
//}

