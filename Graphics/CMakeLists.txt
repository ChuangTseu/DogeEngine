SET(LIBRARY_VERSION "0.1.0")
SET(LIBRARY_NAME "GRAPHICS")

FILE(GLOB src *)

find_package(Qt5Core)
find_package(Qt5Widgets)
find_package(Qt5Gui)
find_package(Qt5OpenGL)

include_directories(
        ${Qt5Core_INCLUDE_DIRS}
        ${Qt5Widgets_INCLUDE_DIRS}
        ${Qt5Gui_INCLUDE_DIRS}
        ${Qt5OpenGL_INCLUDE_DIRS}
)

ADD_DEFINITIONS(${QT_DEFINITIONS})


ADD_LIBRARY(GRAPHICS STATIC ${src})

#target_link_libraries(GRAPHICS ${GLEW_LIBRARY} ${OPENGL_LIBRARIES})

SET_TARGET_PROPERTIES(GRAPHICS PROPERTIES VERSION ${LIBRARY_VERSION})


