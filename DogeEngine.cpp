// DogeEngine.cpp : Defines the entry point for the console application.
//


#include <iostream>
#include "Graphics/renderer.h"
#include "Graphics/Renderer2.h"

#ifdef USE_QT
#include <QApplication>
#include "arkMediator.h"
#include "arkQTGUI/arkMainWindow.h"
#endif

#ifdef USE_SDL2
#include "Sdl2GUI/mainwindow.h"
#endif

int main(int argc, char * argv[])
{
	int width = 640 + 89 + 45; int height = 480 + 75;

	Renderer renderer(width, height);

	Renderer2* renderer2 = Renderer2::GetInstance();
	renderer2->SetDimensions(width, height);

#ifdef USE_SDL2    
	(void) argc;
	(void)argv;

	MainWindow w("DogeGL Next Gen", width, height);

	w.setRenderer(&renderer);

	w.runLoop();
#endif


#ifdef USE_QT    
	QApplication application(argc, argv);

	arkMediator<Renderer>::ShPtr mediator_shptr = arkMediator<Renderer>::create(&renderer);

	arkMediator<Renderer2>::ShPtr mediator_shptr2 = arkMediator<Renderer2>::create(renderer2);

	//arkMainWindowShPtr main_window_shptr = arkMainWindow::create(width, height, mediator_shptr);
	arkMainWindowShPtr main_window_shptr = arkMainWindow::create(width, height, mediator_shptr2);

	main_window_shptr->show();

	return application.exec();
#endif

	return 0;
}
