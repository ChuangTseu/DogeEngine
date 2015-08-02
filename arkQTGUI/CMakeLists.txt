SET(LIBRARY_VERSION "0.1.0")
SET(LIBRARY_NAME "arkQTGUI")

FILE(GLOB src *)

# Qt
set(CMAKE_AUTOMOC ON)

find_package(Qt5Core)
find_package(Qt5Widgets)
find_package(Qt5Gui)
find_package(Qt5OpenGL)

find_package(OpenGL REQUIRED) # OpenGL

include_directories(
        ${Qt5Core_INCLUDE_DIRS}
        ${Qt5Widgets_INCLUDE_DIRS}
        ${Qt5Gui_INCLUDE_DIRS}
        ${Qt5OpenGL_INCLUDE_DIRS}

        ${CMAKE_CURRENT_BINARY_DIR}
        .
)

if(Qt5_POSITION_INDEPENDENT_CODE)
  set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

set(QT_USE_OPENGL TRUE)

ADD_DEFINITIONS(${QT_DEFINITIONS})

ADD_LIBRARY(${LIBRARY_NAME} STATIC ${src})

target_link_libraries(${LIBRARY_NAME} Qt5::Core Qt5::Widgets Qt5::Gui Qt5::OpenGL ${OPENGL_LIBRARIES})

SET_TARGET_PROPERTIES(${LIBRARY_NAME} PROPERTIES VERSION ${LIBRARY_VERSION})


