//
//  arkOpenGLWidget.h
//  arkGUI
//
//  Created by anthonycouret on 17/02/2015.
//  Copyright (c) 2015 ac. All rights reserved.
//

#ifndef __arkGUI__arkOpenGLWidget__
#define __arkGUI__arkOpenGLWidget__

#include <memory>
#include <QGLWidget>
#include <QKeyEvent>

#include "arkMediatorWidget.h"

class arkOpenGLWidget;
typedef std::shared_ptr<arkOpenGLWidget> arkOpenGLWidgetShPtr;
typedef std::weak_ptr<arkOpenGLWidget> arkOpenGLWidgetWkPtr;

class arkOpenGLWidget : public QGLWidget, public arkMediatorWidget
{
public :
    
    static arkOpenGLWidgetShPtr create( QGLFormat format, arkAbstractMediatorShPtr mediator_shptr );
    ~arkOpenGLWidget();
    
private :
    
    arkOpenGLWidget( QGLFormat format, arkAbstractMediatorShPtr mediator_shptr );
    arkOpenGLWidgetWkPtr m_weak_ptr;
    
    QTimer* m_realTimeTimer;
    QPoint m_lastMousePos;
    
protected:
    
    void initializeGL(void);
    void resizeGL( int width, int height );
    void paintGL( void );
    
    void mousePressEvent( QMouseEvent * event );
    void mouseMoveEvent( QMouseEvent * event );
    void mouseReleaseEvent( QMouseEvent * event );
    void wheelEvent( QWheelEvent * event );
    void keyPressEvent( QKeyEvent * event );
};

#endif /* defined(__arkGUI__arkOpenGLWidget__) */
